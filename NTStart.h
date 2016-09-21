#pragma once

#ifndef __NT_START__
#define __NT_START__

#include "NonTerminalSymbol.h"
#include "CGALDefs.h"

/**
  * Class NTStart
  *
  * @brief Start point of the overall creation
  *
  * @note Only one start point
  *
  * @author Lukas Gregori
  * @version $Revision: 1.0 $
  *
  * Contact: lukas.gregori@student.tugraz.at
  *
  * Class implemented as singleton
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
class NTStart : public NonTerminalSymbol {

public:

	NTStart(std::vector<Point_2>* high_dens_points, std::vector<Point_2>* start_points);
	~NTStart();

	// See NonTerminalSymbol.h
	NonTerminalSymbol* replace();

private:

	/// All high density points gotten from user
	std::vector<Point_2>* high_dens_points_;
	/// All start points gotten form user
	std::vector<Point_2>* start_points_;

};

#endif