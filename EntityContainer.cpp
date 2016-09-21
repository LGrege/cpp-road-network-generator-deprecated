/*
 * -----------------------------------------------------------------
 * Copyright (C) Lukas Gregori, contact@lukasgregori.com
 *
 * This file is part of the road network generator RoadGen.
 *
 * RoadGen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * RoadGen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------
*/

#include "EntityContainer.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Point_set_2<K>::Vertex_handle  Vertex_handle;

EntityContainer* EntityContainer::instance_ = 0;

//--------------------------------------------------------------------------
EntityContainer* EntityContainer::getInstance() {
	if (instance_ == 0x0)
		instance_ = new EntityContainer();
	return instance_;
}

//--------------------------------------------------------------------------
EntityContainer::EntityContainer() {
	HIGHWAY_COUNT = 0;
}

//--------------------------------------------------------------------------
EntityContainer::~EntityContainer() {
}

//--------------------------------------------------------------------------
Point_2 EntityContainer::getGridCell(CGAL::MP_Float x, CGAL::MP_Float y) {
	return Point_2(floor(CGAL::to_double(x) / (MAX_X / 100.0f)), floor(CGAL::to_double(y) / (MAX_Y / 100.0f)));
}

//--------------------------------------------------------------------------
void EntityContainer::insertStreetSegment(Segment_2 new_segment) {
	all_street_segments_.push_back(new_segment);
	Point_2 mid = CGAL::midpoint(new_segment.source(),new_segment.target());

	int xcell = (int)floor(CGAL::to_double(mid.x()) / (MAX_X / 100.0f));
	int ycell = (int)floor(CGAL::to_double(mid.y()) / (MAX_Y / 100.0f));

	if (xcell > 0 && ycell > 0 && xcell < 100 && ycell < 100)
	  street_segment_grid[xcell][ycell].push_back(new_segment);
}

//--------------------------------------------------------------------------
void EntityContainer::insertIntersection(Point_2 intersection) {
	all_intersection_.push_back(intersection);

	int xcell = (int)floor(CGAL::to_double(intersection.x()) / (MAX_X / 100.0f));
	int ycell = (int)floor(CGAL::to_double(intersection.y()) / (MAX_X / 100.0f));

	if(xcell > 0 &&  ycell > 0 && xcell < 100 && ycell < 100)
	  intersection_grid[xcell][ycell].push_back(intersection);
}

//--------------------------------------------------------------------------
void EntityContainer::insertRiverSegment(Segment_2 new_segment) {
	EntityContainer::getInstance()->river_segments_.push_back(new_segment);
	Point_2 mid = CGAL::midpoint(new_segment.source(), new_segment.target());

	int xcell = (int)floor(CGAL::to_double(mid.x()) / (MAX_X / 100.0f));
	int ycell = (int)floor(CGAL::to_double(mid.y()) / (MAX_Y / 100.0f));

	if (xcell > 0 && ycell > 0 && xcell < 100 && ycell < 100)
		river_segment_grid[xcell][ycell].push_back(new_segment);
}

//---------------------------------------------------------------------------
Point_2 EntityContainer::segmentIntersected(CGAL::MP_Float x, CGAL::MP_Float y, 
	CGAL::MP_Float x_new, CGAL::MP_Float y_new, unsigned old_angle, int x_off, int y_off) {
	// Needed later, we will try to connect to this intersection
	// instead of creating a new one
	Point_2 nearest_intersection_start = getNearestIntersection(x, y);

	// Two lists needed as we need to check the intersections of the 
	// segments bevore and after inserting the new segment to make sure
	// we can detect a new intersection
	std::vector<Point_2> possible_intersections_before;
	std::vector<Point_2> possible_intersections_after;

	std::vector<Segment_2> relevant_street_segments;

	// Only interested in segments within a given range
	Point_2 mid = CGAL::midpoint(Point_2(x, y), Point_2(x_new, y_new));
	Point_2 grid_cell = getGridCell(CGAL::to_double(mid.x()), CGAL::to_double(mid.y()));
	int x_cell = (int)CGAL::to_double(grid_cell.x());
	int y_cell = (int)CGAL::to_double(grid_cell.y());

	// Get the relevant segments by iterating over the
	// cell the base segment is in + additional offset cells
	// around it
	for (int cel_count = -x_off; cel_count < x_off; cel_count++) {
		for (int row_count = -y_off; row_count < y_off; row_count++) {
			int curr_x = x_cell + cel_count;
			int curr_y = y_cell + row_count;

			if (curr_x > 0 && curr_y > 0 &&
				curr_x < 100 && curr_y < 100 && street_segment_grid[curr_x][curr_y].size() > 0) {
				relevant_street_segments.insert(relevant_street_segments.end(),
					street_segment_grid[curr_x][curr_y].begin(),
					street_segment_grid[curr_x][curr_y].end());
			}
		}
	}

	// If no segments in range, no intersection possible
	if (relevant_street_segments.size() == 0)
		return Point_2(0, 0);

	// Calculate intersections bevore insertion
	CGAL::compute_intersection_points(relevant_street_segments.begin(), relevant_street_segments.end(),
		std::back_inserter(possible_intersections_before));

	// Insert new segment and compute intersections again
	relevant_street_segments.push_back(Segment_2(Point_2(x, y), Point_2(x_new, y_new)));

	CGAL::compute_intersection_points(relevant_street_segments.begin(), relevant_street_segments.end(),
		std::back_inserter(possible_intersections_after));

	std::cout << "1,SEGMENT CREATION: " <<  EntityContainer::getInstance()->MAX_SEG_COUNT << "/" << EntityContainer::getInstance()->MAX_SEG_COUNT_INIT << "\n";

	if (possible_intersections_after.size() > possible_intersections_before.size()) {

		// Now check if we found a nearby intersection, and if so check
		// if the angle is not to big (no angle smaller than 40 degrees)
		if (nearest_intersection_start.x().num != 0 || nearest_intersection_start.y().num != 0) {

			// Check angle first
			double deltaX = CGAL::to_double(nearest_intersection_start.x().num - x);
			double deltaY = CGAL::to_double(nearest_intersection_start.y().num - y);
			double intersection_angle_new = atan2(deltaY, deltaX) * 180 / M_PI;

			if (abs(intersection_angle_new - old_angle) < MAX_ANGLE)
				return nearest_intersection_start;
		}

		// If bevore there were no intersection, we can be sure
		// that this is the only one and thus take it
		if (possible_intersections_after.size() == 1 && possible_intersections_before.size() == 0)
			return possible_intersections_after[0];


		// Now we need to calculate the difference between the old
		// and new intersections. Those would be all intersections that
		// would occur if we would insert the segment. Now we just get 
		// the closest of them and connect to it.
		std::vector<Point_2> intersection_diff;

		// Find difference
		for (unsigned counter_after = 0; counter_after < possible_intersections_after.size(); counter_after++) {
			Point_2 curr = possible_intersections_after[counter_after];
			bool found = false;

			for (unsigned counter_before = 0; counter_before < possible_intersections_before.size(); counter_before++)
				if (curr == possible_intersections_before[counter_before]) {
					found = true;
					break;
				}

			if (found == false)
				intersection_diff.push_back(curr);
		}

		CGAL::Point_set_2<K> PSet;
		std::list<K::Point_2> all_intersections;

		// Get closest intersection
		// Convert the intersections from Point_2 list to K::Point_2 list
		for (Point_2 curr : intersection_diff)
			all_intersections.push_back(K::Point_2(CGAL::to_double(curr.x()), (CGAL::to_double(curr.y()))));

		PSet.insert(all_intersections.begin(), all_intersections.end());

		Vertex_handle nearest_vertex_handle = PSet.nearest_neighbor(K::Point_2(CGAL::to_double(x), CGAL::to_double(y)));
		Point_2 nearest_intersection = Point_2(nearest_vertex_handle->point().x(), nearest_vertex_handle->point().y());

		// std::cout << "INTERSECTION AT: (" << CGAL::to_double(nearest_intersection.x()) << "," << CGAL::to_double(nearest_intersection.y()) << ")\n";
		return nearest_intersection;
	}

	return Point_2(0, 0);
}

//---------------------------------------------------------------------------
Point_2 EntityContainer::getNearestIntersection(CGAL::MP_Float x, CGAL::MP_Float y) {
	// Check if there are intersections at all, if not continue
	if (all_intersection_.size() > 0) {

		CGAL::Point_set_2<K> PSet;
		std::list<Point_2> tmp_intersections;
		std::list<K::Point_2> all_intersections;

		Point_2 grid_cell = getGridCell(x, y);

		int xcell = (int)CGAL::to_double(grid_cell.x());
		int ycell = (int)CGAL::to_double(grid_cell.y());

		// Get all intersection within a 3x3 field of the
		// intersection grid.
		for (int cel_count = -X_OFFSET; cel_count < X_OFFSET; cel_count++) {
			for (int row_count = -Y_OFFSET; row_count < Y_OFFSET; row_count++) {
				int curr_x = xcell + cel_count;
				int curr_y = ycell + row_count;

				if (curr_x > 0 && curr_y > 0 &&
					curr_x < 100 && curr_y < 100 && intersection_grid[curr_x][curr_y].size() > 0) {
					tmp_intersections.insert(tmp_intersections.end(),
						intersection_grid[curr_x][curr_y].begin(),
						intersection_grid[curr_x][curr_y].end());
				}
			}
		}

		// If no points in neighborhood, return
		if (tmp_intersections.size() == 0)
			return Point_2(0, 0);

		// Convert the intersections from Point_2 list to K::Point_2 list
		for (Point_2 curr : tmp_intersections)
			all_intersections.push_back(K::Point_2(CGAL::to_double(curr.x()), (CGAL::to_double(curr.y()))));

		PSet.insert(all_intersections.begin(), all_intersections.end());

		Vertex_handle nearest_vertex_handle = PSet.nearest_neighbor(K::Point_2(CGAL::to_double(x), CGAL::to_double(y)));
		Point_2 nearest_intersection_start = Point_2(nearest_vertex_handle->point().x(), nearest_vertex_handle->point().y());

		// If we find an intersection somewhere near we know that we
		// are done and set our x-coordinate to that new point
		if (sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x, y), nearest_intersection_start))) < 10)
			return nearest_intersection_start;
	}

	return Point_2(0, 0);
}


//---------------------------------------------------------------------------
std::vector<Point_2> EntityContainer::riverIntersected(CGAL::MP_Float x, CGAL::MP_Float y,
	CGAL::MP_Float x_new, CGAL::MP_Float y_new, int x_off, int y_off) {
	std::vector<Point_2> intersection_diff;

	// Two lists needed as we need to check the intersections of the 
	// segments bevore and after inserting the new segment to make sure
	// we can detect a new intersection
	std::vector<Point_2> possible_intersections_before;
	std::vector<Point_2> possible_intersections_after;

	std::vector<Segment_2> relevant_street_segments;

	// Only interested in segments within a given range
	Point_2 mid = CGAL::midpoint(Point_2(x, y), Point_2(x_new, y_new));
	CGAL::MP_Float xtmp = CGAL::to_double(mid.x());
	CGAL::MP_Float ytmp = CGAL::to_double(mid.y());

	Point_2 grid_cell = getGridCell(xtmp, ytmp);
	int xcell = (int)CGAL::to_double(grid_cell.x());
	int ycell = (int)CGAL::to_double(grid_cell.y());

	for (int cel_count = -x_off; cel_count < x_off; cel_count++) {
		for (int row_count = -y_off; row_count < y_off; row_count++) {
			int curr_x = xcell + cel_count;
			int curr_y = ycell + row_count;

			if (curr_x > 0 && curr_y > 0 &&
				curr_x < 100 && curr_y < 100 && river_segment_grid[curr_x][curr_y].size() > 0) {
				relevant_street_segments.insert(relevant_street_segments.end(),
					river_segment_grid[curr_x][curr_y].begin(),
					river_segment_grid[curr_x][curr_y].end());
			}
		}
	}

	// If no segments in ranged, no intersection possible
	if (relevant_street_segments.size() == 0)
		return intersection_diff;

	CGAL::compute_intersection_points(relevant_street_segments.begin(), relevant_street_segments.end(),
		std::back_inserter(possible_intersections_before));

	// Insert new segment and compute intersections again
	relevant_street_segments.push_back(Segment_2(Point_2(x, y), Point_2(x_new, y_new)));

	CGAL::compute_intersection_points(relevant_street_segments.begin(), relevant_street_segments.end(),
		std::back_inserter(possible_intersections_after));

	std::cout << "1,SEGMENT CREATION: " << EntityContainer::getInstance()->MAX_SEG_COUNT << "/" << EntityContainer::getInstance()->MAX_SEG_COUNT_INIT << "\n";

	if (possible_intersections_after.size() > possible_intersections_before.size()) {
		// If bevore there were no intersection, we can be sure
		// that this is the only one and thus take it
		if (possible_intersections_after.size() == 1 && possible_intersections_before.size() == 0)
			return possible_intersections_after;

		// Now we need to calculate the difference between the old
		// and new intersections. Those would be all intersections that
		// would occur if we would insert the segment. Now we just get 
		// the closest of them and connect to it.

		// Find difference
		for (unsigned counter_after = 0; counter_after < possible_intersections_after.size(); counter_after++) {
			Point_2 curr = possible_intersections_after[counter_after];
			bool found = false;

			for (unsigned counter_before = 0; counter_before < possible_intersections_before.size(); counter_before++)
				if (curr == possible_intersections_before[counter_before]) {
					found = true;
					break;
				}

			if (found == false)
				intersection_diff.push_back(curr);
		}
	}

	return intersection_diff;
}