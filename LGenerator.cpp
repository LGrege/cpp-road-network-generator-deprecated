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

#include "LGenerator.h"
#include "EntityContainer.h"
#include "NTStart.h"
#include "NTHighway.h"

//---------------------------------------------------------------------------
LGenerator::LGenerator() {
	// Initialize pseudorandom seed
	srand(376);
}

//---------------------------------------------------------------------------
LGenerator::~LGenerator() {
}

//---------------------------------------------------------------------------
void LGenerator::generateRoadNetwork(std::string file_name) {
	NTStart* new_start = new NTStart(&EntityContainer::getInstance()->high_dens_points_,
		                             &EntityContainer::getInstance()->start_points_);

	EntityContainer::getInstance()->HIGHWAY_COUNT = EntityContainer::getInstance()->high_dens_points_.size()
		                              * EntityContainer::getInstance()->start_points_.size();

	// Generate the rivers bevore creating the road network
	generateRivers(EntityContainer::getInstance()->NUMB_RIVERS);

	// Iterate over the non-terminal symbols until
	// all of them have terminated or a maximum 
	// iteration count is reached.
	bool all_terminated = false;
	unsigned max_iter = 0;

	while (!all_terminated && max_iter++ < MAX_GENERATION_ITERS) {
		NonTerminalSymbol* tmp = new_start;
		NonTerminalSymbol* next = 0x0;

		all_terminated = true;

		while (tmp != 0x0) {
			next = tmp->next_;

			if (!tmp->terminated_) {
				tmp->replace();
				all_terminated = false;
			}

			tmp = next;
		}
	}

	interpret(file_name);
}

//---------------------------------------------------------------------------
void LGenerator::interpret(std::string file_name) {
	int_writer_.openFile(file_name);

	// Draw the high density and start points
	// first. Highways grow from start points towards
	// high-density points
	handleHighDensePoints();
	handleHighwayStartPoints();

	// Now hand all the stdreets and highways over
	// to our interpreter
	int_writer_.handleStreetSegments(EntityContainer::getInstance()->all_street_segments_);
	int_writer_.handleRiver(EntityContainer::getInstance()->river_segments_);
	int_writer_.handleHighwaySegments(EntityContainer::getInstance()->all_highway_segments_);

	// Now check for crossings as no new segments
	// will be added after this point for sure
	int_writer_.startNetwork();
	handleCrossings();
	handleHighwayExits();
	int_writer_.closeNetwork();

	int_writer_.handleBridges(EntityContainer::getInstance()->bridges_);
	int_writer_.closeFile();

	cout << "GENERATION DONE..." << endl;
}


//---------------------------------------------------------------------------
void LGenerator::handleCrossings() {
	// Print the result to the ps-file
	for (Point_2 curr_interception : EntityContainer::getInstance()->all_intersection_)
		int_writer_.handleInterception(CGAL::to_double(curr_interception.x()), CGAL::to_double(curr_interception.y()));

	for (Point_2 curr_interception : EntityContainer::getInstance()->all_highway_intersections_)
		int_writer_.handleHighwayInterception(CGAL::to_double(curr_interception.x()), CGAL::to_double(curr_interception.y()));
}

//---------------------------------------------------------------------------
void LGenerator::handleHighwayExits() {
	for (Point_2 curr_exit : EntityContainer::getInstance()->all_highway_exits_)
		int_writer_.handleHighwayExit(CGAL::to_double(curr_exit.x()), CGAL::to_double(curr_exit.y()));
}


//---------------------------------------------------------------------------
void LGenerator::handleHighDensePoints() {
	for (Point_2 curr_point : EntityContainer::getInstance()->high_dens_points_) 
		int_writer_.handleHighDensityPoint(curr_point.x().num, curr_point.y().num, (int)(EntityContainer::getInstance()->MAX_X / 6.0f));
}

//---------------------------------------------------------------------------
void LGenerator::handleHighwayStartPoints() {
	for (Point_2 curr_point : EntityContainer::getInstance()->start_points_)
		int_writer_.handleNewHighwayStart(curr_point.x().num, curr_point.y().num);
}

//---------------------------------------------------------------------------
void LGenerator::generateRivers(unsigned river_count) {

	while (river_count--) {
		vector<Point_2> curr_river;
		vector<Point_2> curr_river_off;

		// Generate river here, river may either start
		// somewhere on x or y axis
		double curr_x = 0.0f;
		double curr_y = 0.0f;
		double old_x = 0.0f;
		double old_y = 0.0f;
	
		double x_off = 0.0f;
		double y_off = 0.0f;

		if ((rand() % 100) < 50) {
			// On X-Axis
			double y_rand = rand() % (EntityContainer::getInstance()->MAX_Y - 100) + 100;
			old_x = 0.0f;
			curr_x = 1.0f;
			curr_y = old_y = y_rand;
			y_off = (rand() % MAX_RIVER_WIDTH - 10) + 20;
		} else {
			// On Y-Axis
			double x_rand = rand() % (EntityContainer::getInstance()->MAX_X - 100) + 100;
			old_y = 0.0f;
			curr_y = 1.0f;
			curr_x = old_x = x_rand;
			x_off = (rand() % MAX_RIVER_WIDTH - 10) + 20;;
		}

		curr_river.push_back(Point_2(curr_x,curr_y));
		curr_river_off.push_back(Point_2(old_x, old_y));

		bool river_done = false;
		while (!river_done) {
			// Calculate the angle between the last two points (do
			// not want to make roads with two small or to big angle)
			double deltaX = CGAL::to_double(curr_x - old_x);
			double deltaY = CGAL::to_double(curr_y - old_y);
			double old_angle = atan2(deltaY, deltaX) * 180 / M_PI;

			if (old_angle < 0)
				old_angle += 360;

			double new_angle = old_angle + (((float)rand()) / RAND_MAX * MAX_RIVER_ANGLE * 2.0f - MAX_RIVER_ANGLE);

			double x_new = RIVER_DISTANCE * cos(new_angle * M_PI / 180.0f) + curr_x;
			double y_new = RIVER_DISTANCE * sin(new_angle * M_PI / 180.0f) + curr_y;

			// If river grows outside of set boundaries, terminate loop
			if (x_new < 0 || x_new > EntityContainer::getInstance()->MAX_X 
				|| y_new < 0 || y_new > EntityContainer::getInstance()->MAX_Y)
				break;

			old_x = curr_x;
			old_y = curr_y;
			curr_x = x_new;
			curr_y = y_new;

			curr_river.push_back(Point_2(curr_x, curr_y));
			curr_river_off.push_back(Point_2(curr_x + x_off, curr_y + y_off));

			EntityContainer::getInstance()->insertRiverSegment(Segment_2(Point_2(old_x, old_y), Point_2(curr_x, curr_y)));
			EntityContainer::getInstance()->insertRiverSegment(Segment_2(Point_2(old_x + x_off, old_y + y_off), Point_2(curr_x + x_off, curr_y + y_off)));
		}

		// Now push the rivers (second halve) in reversed order,
		// so that the polygon-handle of the out-writer can print it later.
		for (unsigned counter = curr_river_off.size(); counter-- > 0; )
			curr_river.push_back(curr_river_off.at(counter));

		EntityContainer::getInstance()->all_rivers_.push_back(curr_river);
	}
}

