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

#include "NTStart.h"
#include "NTHighway.h"

//---------------------------------------------------------------------------
NTStart::NTStart(std::vector<Point_2>* high_dens_points, std::vector<Point_2>* start_points) {
	high_dens_points_ = high_dens_points;
	start_points_ = start_points;
}

//---------------------------------------------------------------------------
NTStart::~NTStart() {
}

//---------------------------------------------------------------------------
NonTerminalSymbol* NTStart::replace() {
	terminated_ = true;
	NTHighway* start = 0x0;
	NTHighway* last = 0x0;

	for (unsigned counter = 0; counter < start_points_->size(); counter++) {

		float new_x = CGAL::to_double(start_points_->at(counter).x());
		float new_y = CGAL::to_double(start_points_->at(counter).y());

		NTHighway* new_highway = new NTHighway(new_x,new_y,high_dens_points_);
		new_highway->pre_ = last;
		
		if (last)
			last->next_ = new_highway;

		if (start == 0x0)
			start = new_highway;

		last = new_highway;
	}

	insertSublist(start,this);
	return this;
}
