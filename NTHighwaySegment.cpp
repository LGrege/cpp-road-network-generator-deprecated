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

#include "NTHighwaySegment.h"
#include "EntityContainer.h"
#include "NTStreet.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <CGAL/Point_set_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Point_set_2<K>::Vertex_handle  Vertex_handle;

//---------------------------------------------------------------------------
NTHighwaySegment::NTHighwaySegment(CGAL::MP_Float x, CGAL::MP_Float y,
	CGAL::MP_Float x_old, CGAL::MP_Float y_old, unsigned total_distance, Point_2 goal, bool create_street) {
	x_ = x;
	y_ = y;
	x_old_ = x_old;
	y_old_ = y_old;
	total_distance_ = total_distance;
	goal_ = goal;
	create_street_ = create_street;
}

//---------------------------------------------------------------------------
NTHighwaySegment::~NTHighwaySegment() {
}

//---------------------------------------------------------------------------
NonTerminalSymbol* NTHighwaySegment::replace() {
	terminated_ = true;

	// Check if we are still allowed to create new street segments
	if (--EntityContainer::getInstance()->MAX_SEG_COUNT <= 0) {
		EntityContainer::getInstance()->HIGHWAY_COUNT--;
		return this;
	}

	double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), goal_)));

	// Using a vector from our current point to the goal 
	// and a distance, we calculate the next point within
	// range (on direct line to goal) -> Everytime a
	// tenth of the total distance closer.
	Vector_2 direction(Point_2(x_, y_), goal_);
	Point_2 next_point = Point_2(x_, y_) + total_distance_*(0.1f)*(direction / current_distance);

	// Every point will get some offset (randomized seed initialized already),
	// so that the creation of the highways does look more natural
	float x_offset = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (current_distance / 10.0f)));
	float y_offset = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (current_distance / 10.0f)));

	Point_2 new_position(CGAL::to_double(next_point.x()) + x_offset, CGAL::to_double(next_point.y()) + y_offset);
	Point_2 old_position(x_, y_);

	// The new highway segment that may be used (only if it does
	// not intersect with any of the other segments
	Segment_2 new_highway_segment(old_position, new_position);

	double deltaX = CGAL::to_double(CGAL::to_double(new_position.x()) - x_);
	double deltaY = CGAL::to_double(CGAL::to_double(new_position.y()) - y_);
	double old_angle = atan2(deltaY, deltaX) * 180 / M_PI;

	if (old_angle < 0)
		old_angle += 360;

	vector<Point_2> river_intersections = EntityContainer::getInstance()->riverIntersected(x_, y_,
		CGAL::to_double(new_position.x()), CGAL::to_double(new_position.y()), 10, 10);

	// Function returns true if segment terminates as new segment was created
	// on other riverbank
	if (checkForRiverIntersection(river_intersections, new_position, old_position))
		return this;
	
	// If we are close enough to the end point, connect directly 
	// to it. Otherwise random segment creation
	if (current_distance < HIGHWAY_SNAP_DISTANCE) {
		// Add new segment to intersection and terminate
		double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), goal_)));
		Vector_2 direction(Point_2(x_, y_), goal_);
		Point_2 next_point = Point_2(x_, y_) + current_distance*(1.5f)*(direction / current_distance);

		EntityContainer::getInstance()->all_highway_poly_segments_.push_back(Segment_2(Point_2(x_, y_), next_point));
		EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), next_point));

		EntityContainer::getInstance()->all_highway_segments_.push_back(Segment_2(Point_2(x_, y_), goal_));
		EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), goal_));

		x_ = CGAL::to_double(goal_.x());
		y_ = CGAL::to_double(goal_.y());
		EntityContainer::getInstance()->HIGHWAY_COUNT--;
		return this;
	}

	// Calculate the nearest intersection that would happen if
	// we inserted the new segment. If the return is != P(0,0),
	// we know that there exists an intersection
	Point_2 possible_intersection = EntityContainer::getInstance()->segmentIntersected(x_, y_,
		CGAL::to_double(new_position.x()), CGAL::to_double(new_position.y()), old_angle, 10, 10);

	if (CGAL::to_double(possible_intersection.x()) != 0 && CGAL::to_double(possible_intersection.y()) != 0) {
		// Add new segment to intersection and terminate
		double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), possible_intersection)));
		Vector_2 direction(Point_2(x_, y_), possible_intersection);
		Point_2 next_point = Point_2(x_, y_) + current_distance*(1.4f)*(direction / current_distance);
		EntityContainer::getInstance()->all_highway_poly_segments_.push_back(Segment_2(old_position,next_point));
		EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(old_position, next_point));

		x_ = CGAL::to_double(possible_intersection.x());
		y_ = CGAL::to_double(possible_intersection.y());

		EntityContainer::getInstance()->all_highway_segments_.push_back(Segment_2(old_position, Point_2(x_, y_)));
		EntityContainer::getInstance()->insertStreetSegment(Segment_2(old_position, Point_2(x_, y_)));

		Point_2 new_intersection(x_, y_);
		EntityContainer::getInstance()->all_highway_intersections_.push_back(new_intersection);
		EntityContainer::getInstance()->insertIntersection(new_intersection);

		EntityContainer::getInstance()->HIGHWAY_COUNT--;
		return this;
	} else {
		EntityContainer::getInstance()->all_highway_segments_.push_back(new_highway_segment);
		EntityContainer::getInstance()->insertStreetSegment(new_highway_segment);
		EntityContainer::getInstance()->all_highway_poly_segments_.push_back(new_highway_segment);
		EntityContainer::getInstance()->all_street_poly_segments_.push_back(new_highway_segment);

		CGAL::MP_Float x_old_tmp = x_;
		CGAL::MP_Float y_old_tmp = y_;
		x_ = CGAL::to_double(next_point.x()) + x_offset;
		y_ = CGAL::to_double(next_point.y()) + y_offset;

		// Add a new highway exit to our list of points
		EntityContainer::getInstance()->all_highway_exits_.push_back(Point_2(x_, y_));

		// Now create a new highway segment and insert it into the list
		NTHighwaySegment* new_segment = new NTHighwaySegment(x_, y_, x_old_tmp,
			y_old_tmp, total_distance_, goal_, !create_street_);

		new_segment->next_ = next_;

		// Only create more street segments if streets left (check max_seg_count-variable first)
		if (EntityContainer::getInstance()->MAX_SEG_COUNT > 0) {
			// Create Street handling here (We currently take every 
			// new segment start as a highway exit and create streets  
			// form there
			float x_offset = ((float)rand()) / RAND_MAX * 10.0f - 5.0f;
			float y_offset = ((float)rand()) / RAND_MAX * 10.0f - 5.0f;

			Point_2 possible_intersection_branch = EntityContainer::getInstance()->
				segmentIntersected(x_, y_, x_ + x_offset, y_ + y_offset, old_angle, 5, 5);

			if (!(CGAL::to_double(possible_intersection_branch.x()) != 0 && CGAL::to_double(possible_intersection_branch.y()) != 0)) {
				NTStreet* new_street = new NTStreet(x_ + x_offset, y_ + y_offset, x_, y_);

				EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), Point_2(x_ + x_offset, y_ + y_offset)));
				EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), Point_2(x_ + x_offset, y_ + y_offset)));

				new_segment->pre_ = new_street;
				new_street->next_ = new_segment;

				if (new_segment->next_ != 0x0)
					new_segment->next_->pre_ = new_segment;

				this->next_ = new_street;
				new_street->pre_ = this;
			} else {
				new_segment->pre_ = this;
				new_segment->next_ = next_;

				if (next_ != 0x0)
					next_->pre_ = new_segment;

				next_ = new_segment;
			}
		} else {
			new_segment->pre_ = this;
			new_segment->next_ = next_;

			if (next_ != 0x0)
				next_->pre_ = new_segment;

			next_ = new_segment;
		}
		return this;
	}
}

//--------------------------------------------------------------------------
bool NTHighwaySegment::checkForRiverIntersection(vector<Point_2> river_intersections, Point_2 new_position, Point_2 old_position) {
	if (river_intersections.size() > 0) {
		// Easy case, detected intersections on both sides of the river, can thus
		// easily build a bridge between the points and am done
		if (river_intersections.size() == 2) {
			Point_2 closest_intersection = (CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(0)) <
				CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(1))) ? river_intersections.at(0) : river_intersections.at(1);

			Point_2 farther_intersection = (CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(0)) >
				CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(1))) ? river_intersections.at(0) : river_intersections.at(1);

			handleRiverIntersection(closest_intersection, farther_intersection, old_position);
			return true;
		}
		else if (river_intersections.size() == 1) {

			double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), new_position)));
			Vector_2 direction(Point_2(x_, y_), new_position);

			// Shift end of highway segment further into the river
			// along the intersection segment to get the second river bank
			unsigned counter = 2;
			Point_2 next_point = Point_2(x_, y_) + current_distance*counter*(direction / current_distance);

			while (sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), next_point))) < 2.0f*MAX_RIVER_WIDTH)
				next_point = Point_2(x_, y_) + current_distance*++counter*(direction / current_distance);

			vector<Point_2> river_intersections_second_try = EntityContainer::getInstance()->riverIntersected(x_, y_,
				CGAL::to_double(next_point.x()), CGAL::to_double(next_point.y()), 10, 10);

			if (river_intersections_second_try.size() == 2) {
				Point_2 closest_intersection = (CGAL::squared_distance(Point_2(x_, y_), river_intersections_second_try.at(0)) <
					CGAL::squared_distance(Point_2(x_, y_), river_intersections_second_try.at(1))) ? river_intersections_second_try.at(0) : river_intersections_second_try.at(1);

				Point_2 farther_intersection = (CGAL::squared_distance(Point_2(x_, y_), river_intersections_second_try.at(0)) >
					CGAL::squared_distance(Point_2(x_, y_), river_intersections_second_try.at(1))) ? river_intersections_second_try.at(0) : river_intersections_second_try.at(1);

				handleRiverIntersection(closest_intersection, farther_intersection, old_position);
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------------------
void NTHighwaySegment::handleRiverIntersection(Point_2 closest_intersection, Point_2 farther_intersection,Point_2 old_position) {
	// Add new bridge, will be used later by out-writer to be displayed
	EntityContainer::getInstance()->bridges_.push_back(Segment_2(closest_intersection, farther_intersection));

	// Add new segment to intersection and terminate
	double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), closest_intersection)));
	Vector_2 direction(Point_2(x_, y_), closest_intersection);
	Point_2 next_point = Point_2(x_, y_) + current_distance*(1.4f)*(direction / current_distance);
	EntityContainer::getInstance()->all_highway_poly_segments_.push_back(Segment_2(old_position, next_point));
	EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(old_position, next_point));

	x_ = CGAL::to_double(closest_intersection.x());
	y_ = CGAL::to_double(closest_intersection.y());

	EntityContainer::getInstance()->all_highway_segments_.push_back(Segment_2(old_position, Point_2(x_, y_)));
	EntityContainer::getInstance()->insertStreetSegment(Segment_2(old_position, Point_2(x_, y_)));

	// Now create a new segment on the other side of the river
	NTHighwaySegment* new_segment = new NTHighwaySegment(CGAL::to_double(farther_intersection.x()), CGAL::to_double(farther_intersection.y()), 0, 0,
		sqrt(CGAL::to_double(CGAL::squared_distance(farther_intersection, goal_))), goal_, false);

	if (this->next_)
		this->next_->pre_ = new_segment;

	new_segment->pre_ = this;
	new_segment->next_ = next_;
	next_ = new_segment;
}

