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

#include "NTHighway.h"
#include "NTHighwaySegment.h"

//---------------------------------------------------------------------------
NTHighway::NTHighway(CGAL::MP_Float x, CGAL::MP_Float y, std::vector<Point_2>* high_dens_points) :
	x_(x), y_(y), high_dens_points_(high_dens_points) {
	terminated_ = false;
}

//--------------------------------------------------------------------------
NTHighway::~NTHighway() {
}

//--------------------------------------------------------------------------
NonTerminalSymbol* NTHighway::replace() {
	terminated_ = true;

	NTHighwaySegment* start = 0x0;
	NTHighwaySegment* previous = 0x0;

	// For every high density point we create a new branch
	// (Segment) that will grow towards it
	for (unsigned counter = 0; counter < high_dens_points_->size(); counter++) {
		CGAL::MP_Float distance = sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_, y_), high_dens_points_->at(counter))));
		NTHighwaySegment* new_segment = new NTHighwaySegment(x_,y_,0,0,CGAL::to_double(distance), high_dens_points_->at(counter), false);

		new_segment->pre_ = previous;

		if (previous)
			previous->next_ = new_segment;

		if (start == 0x0)
			start = new_segment;

		previous = new_segment;
	}

	insertSublist(start, this);
	return this;
}
