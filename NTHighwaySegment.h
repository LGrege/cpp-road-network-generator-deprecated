#pragma once

#ifndef __NT_HIGHWAY_SEGMENT__
#define __NT_HIGHWAY_SEGMENT__

#include "NonTerminalSymbol.h"
#include "CGALDefs.h"

/**
  * @class NTHighwaySegment
  *
  * @brief Smallest unit of highway.
  *
  * The segments that make up the actual highway. Highways always grow
  * towards a certain goal point (high density point in this case). 
  *
  * @author Lukas Gregori
  * @version $Revision: 1.0 $
  *
  * Contact: lukas.gregori@student.tugraz.at
  *
  *
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
  *
  */
class NTHighwaySegment : public NonTerminalSymbol {

public:

	NTHighwaySegment(CGAL::MP_Float x, CGAL::MP_Float y, 
		CGAL::MP_Float x_old, CGAL::MP_Float y_old, unsigned total_distance, Point_2 goal, bool create_street);
	~NTHighwaySegment();

	// See NonTerminalSymbol.h
	NonTerminalSymbol* replace();

	/** @brief  Handles intersections of highway segments with rivers
	  *
	  * If a highway intersects with a river, the segment is extended further
	  * to reach the other side of the river. The two opposite points can
	  * now be connected with a bridge
	  *
	  * At the other side of the river a new segment is created that
	  * can grow further (thus rivers do not automatically terminate
	  * highways).
	  *
	  * @param Point_2 closest_intersection First conflict point
	  * @param Point_2 farther_intersection Second conflict point got by extension
	  * @param Point_2 old_position Old start position before conlission
 	  *
	  */
	void handleRiverIntersection(Point_2 closest_intersection, Point_2 farther_intersection, Point_2 old_position);

	/** @brief Checks if there are intersections with a river and if so handles them accordingly
	  *
	  * Checks if there are intersections of a highway segment with a river. If this is the case,
	  * the segment has to be handled accordingly (Call handleRierIntersection method).
	  *
	  * @param vector<Point_2> river_intersections Contains possible intersections
	  * @param Point_2 new_position Old end position before conlission
	  * @param Point_2 old_position Old start position before conlission
 	  *
	  */
	bool checkForRiverIntersection(vector<Point_2> river_intersections, Point_2 new_position, Point_2 old_position);

private:

	/// Current X-coordinate of segment
	CGAL::MP_Float x_;
	/// Current Y-coordinate of segment
	CGAL::MP_Float y_;

	/// Previous X-coordinate of segment
	CGAL::MP_Float x_old_;
	/// Previous Y-coordinate of segment
	CGAL::MP_Float y_old_;

	/// Total distance from the origin of highway to goal
	unsigned total_distance_;
	/// Desination of highway (High-Density-Point)
	Point_2 goal_;

	/** @brief If set, create new segment (branch)
	  *
	  * If true we create a new street segment,
	  * if not we set it to true (only every second
	  * segment is a highay_exit). */
	bool create_street_;
};

#endif