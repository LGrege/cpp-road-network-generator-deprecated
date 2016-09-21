#pragma once

#ifndef __NT_HIGHWAY__
#define __NT_HIGHWAY__

#include "NonTerminalSymbol.h"
#include "CGALDefs.h"

/**
  * @class NTHighway
  *
  * @brief Start for the highway creation
  *
  * Wrapper rule for the actual highway segments (NTHighwaySegment).
  * Creates as many segments as high density points gotten by the constructor
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
class NTHighway : public NonTerminalSymbol {

public:

	NTHighway(CGAL::MP_Float x, CGAL::MP_Float y, std::vector<Point_2>* high_dens_points);
	~NTHighway();

	// See NonTerminalSymbol.h
	NonTerminalSymbol* replace();

private:

	/// X-Coordinate of the highway start
	CGAL::MP_Float x_;
	/// Y-Coordinate of the highway start
	CGAL::MP_Float y_;

	/// Goals of the highway generation
	std::vector<Point_2>* high_dens_points_;
};

#endif