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

#include "NTStreet.h"
#include "EntityContainer.h"

#include <CGAL/Point_set_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include "EntityContainer.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Point_set_2<K>::Vertex_handle  Vertex_handle;

//---------------------------------------------------------------------------
NTStreet::NTStreet(CGAL::MP_Float x, CGAL::MP_Float y,
	CGAL::MP_Float x_old, CGAL::MP_Float y_old) {
	x_ = x;
	y_ = y;
	x_old_ = x_old;
	y_old_ = y_old;
}

//---------------------------------------------------------------------------
NTStreet::~NTStreet() {
}

//---------------------------------------------------------------------------
NonTerminalSymbol* NTStreet::replace() {
	// Just to avoid that streets that are just created
	// are used right away. This would lead complications with
	// fill bundles at the start.
	if (EntityContainer::getInstance()->HIGHWAY_COUNT)
		return this;

	terminated_ = true;

	// Check if we are still allowed to create new street segments
	if (--EntityContainer::getInstance()->MAX_SEG_COUNT <= 0)
		return this;

	// Calculate the angle between the last two points (do
	// not want to make roads with two small or to big angle)
	double deltaX = CGAL::to_double(x_ - x_old_);
	double deltaY = CGAL::to_double(y_ - y_old_);
	double old_angle = atan2(deltaY, deltaX) * 180 / M_PI;

	if (old_angle < 0)
		old_angle += 360;

	double new_angle = old_angle + (((float)rand()) / RAND_MAX * 40.0f - 20.0f);

	CGAL::MP_Float x_new = DISTANCE * cos(new_angle * M_PI / 180.0f) + x_;
	CGAL::MP_Float y_new = DISTANCE * sin(new_angle * M_PI / 180.0f) + y_;

	// Now check if the newly calculated coordinates are still within
	// our boundaries (e.g. is not alowed to go below zero)
	if (!coordinateInField(x_new, y_new))
		return this;

	vector<Point_2> river_intersections = EntityContainer::getInstance()->riverIntersected(x_, y_,
		x_new, y_new, 10, 10);

	if (river_intersections.size() > 0) {
		// Add new segment to intersection and terminate
		double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(0))));
		Vector_2 direction(Point_2(x_, y_), river_intersections.at(0));
		Point_2 next_point = Point_2(x_, y_) + current_distance*(1.1f)*(direction / current_distance);

		EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), next_point));
		EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), river_intersections.at(0)));

		x_ = CGAL::to_double(river_intersections.at(0).x());
		y_ = CGAL::to_double(river_intersections.at(0).y());

		EntityContainer::getInstance()->insertIntersection(Point_2(x_, y_));
		return this;
	}

	// Calculate the nearest intersection that would happen if
	// we inserted the new segment. If the return is != P(0,0),
	// we know that there exists an intersection
	Point_2 possible_intersection = EntityContainer::getInstance()->segmentIntersected(x_, y_, x_new, y_new, old_angle, 5, 5);

	if (CGAL::to_double(possible_intersection.x()) != 0 && CGAL::to_double(possible_intersection.y()) != 0) {
		// Add new segment to intersection and terminate
		double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), possible_intersection)));
		Vector_2 direction(Point_2(x_, y_), possible_intersection);
		Point_2 next_point = Point_2(x_, y_) + current_distance*(1.1f)*(direction / current_distance);

		EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), next_point));
		EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), possible_intersection));

		x_ = CGAL::to_double(possible_intersection.x());
		y_ = CGAL::to_double(possible_intersection.y());

		EntityContainer::getInstance()->insertIntersection(Point_2(x_, y_));
	} else {
		// Othwise create a new street segment
		NTStreet* new_street = new NTStreet(x_new, y_new, x_, y_);
		new_street->next_ = next_;

		if (new_street->next_ != 0x0)
			new_street->next_->pre_ = new_street;

		this->next_ = new_street;
		new_street->pre_ = this;

		// Create new branch here (given a certain possibility)
		if ((unsigned)(rand() % 100) < EntityContainer::getInstance()->BRANCH_PROB) {

			// half of the branches shall branch right, half
			// left of the existing segment
			// Todo: Implement Branching to square with least neighbours
			if ((rand() % 100) < 50)
				new_angle = old_angle + 90.0f + (((float)rand()) / RAND_MAX * 20.0f - 10.0f);
			else
				new_angle = old_angle - 90.0f + (((float)rand()) / RAND_MAX * 20.0f - 10.0f);

			CGAL::MP_Float x_new_branch = DISTANCE * cos(new_angle * M_PI / 180.0f) + x_;
			CGAL::MP_Float y_new_branch = DISTANCE * sin(new_angle * M_PI / 180.0f) + y_;

			// Same check as bevore, need to make sure
			// that our new branch wont intersect with 
			// any of the existing segments
			Point_2 possible_intersection_branch = EntityContainer::getInstance()->
				segmentIntersected(x_, y_, x_new_branch, y_new_branch, old_angle, 5, 5);

			if (!(CGAL::to_double(possible_intersection_branch.x()) != 0 && CGAL::to_double(possible_intersection_branch.y()) != 0)) {

				vector<Point_2> river_intersections = EntityContainer::getInstance()->riverIntersected(x_, y_, x_new_branch, y_new_branch, 10, 10);

				if (river_intersections.size() > 0) {
					// Add new segment to intersection and terminate
					double current_distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), river_intersections.at(0))));
					Vector_2 direction(Point_2(x_, y_), river_intersections.at(0));
					Point_2 next_point = Point_2(x_, y_) + current_distance*(1.1f)*(direction / current_distance);

					EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), next_point));
					EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), river_intersections.at(0)));

					x_ = CGAL::to_double(river_intersections.at(0).x());
					y_ = CGAL::to_double(river_intersections.at(0).y());

					EntityContainer::getInstance()->insertIntersection(Point_2(x_, y_));
				} else {
					NTStreet* new_branch = new NTStreet(x_new_branch, y_new_branch, x_, y_);

					// Add root of branch to the intersection
					EntityContainer::getInstance()->insertIntersection(Point_2(x_, y_));

					new_street->pre_ = new_branch;
					new_branch->next_ = new_street;

					this->next_ = new_branch;
					new_branch->pre_ = this;

					EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), Point_2(x_new_branch, y_new_branch)));
					EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), Point_2(x_new_branch, y_new_branch)));
				}
			}
		}

		EntityContainer::getInstance()->all_street_poly_segments_.push_back(Segment_2(Point_2(x_, y_), Point_2(x_new, y_new)));
		EntityContainer::getInstance()->insertStreetSegment(Segment_2(Point_2(x_, y_), Point_2(x_new, y_new)));
	}

	return this;
}

//---------------------------------------------------------------------------
bool NTStreet::coordinateInField(CGAL::MP_Float x_new, CGAL::MP_Float y_new) {
	if ((x_new < 0 || x_new >(float) EntityContainer::getInstance()->MAX_X)) {
		x_ = (x_new < 0) ? 0.0f : x_;
		x_ = (x_new > (float)EntityContainer::getInstance()->MAX_X) ?
			(float)EntityContainer::getInstance()->MAX_X : x_;
		return false;
	}

	if ((y_new < 0 || y_new >(float) EntityContainer::getInstance()->MAX_Y)) {
		y_ = (y_new < 0) ? 0.0f : y_;
		y_ = (y_new > (float) EntityContainer::getInstance()->MAX_Y) ?
			(float)EntityContainer::getInstance()->MAX_Y : y_;
		return false;
	}

	return true;
}

