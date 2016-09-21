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

#include "LightingGenerator.h"

//---------------------------------------------------------------------------
std::vector<Point_2> LightingGenerator::generateStreetLights(std::vector<Point_2> lot) {

	vector<Point_2> all_lights;

	for (unsigned counter = 1; counter < lot.size(); counter++) {
		Point_2 start = lot.at(counter - 1);
		Point_2 end = lot.at(counter);

		double distance = sqrt(CGAL::to_double(CGAL::squared_distance(start, end)));
		double tmp_distance = distance;

		while ((tmp_distance -= MIN_LIGHT_DISTANCE) > MIN_LIGHT_DISTANCE)
		{
			// Calculate the next lamp position based on the current
			// start points, always shifting it by MIN_LIGHT_DISTANCE 
			// towards the goal
			Vector_2 direction(start, end);
			Point_2 new_lamp = start + MIN_LIGHT_DISTANCE*(direction / distance);
			all_lights.push_back(new_lamp);
		}
	}

	return all_lights;
}
