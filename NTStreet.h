#pragma once

#ifndef __NT_STREET__
#define __NT_STREET__

#include "NonTerminalSymbol.h"
#include "CGALDefs.h"


/**
  * Class NTStreet
  *
  * @brief Smalles unit of street
  *
  * Class used for handling the streets. Each street grows outwards
  * with no specific goal (unlike the highways). The streets
  * only keep the previous angle in mind (Do not want to steep
  * angles of streets, should look natural).
  *
  * Streets terminate at collision with any other media (including rivers), and
  * branch based on a predefined probability (See CGALDefs File).
  *
  * @note No street segment subclass exists as with highways
  * @note Streets have no target they grow to
  * @note Terminate at collission
  * @note Also terminate at river contact, no bridges
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
class NTStreet : public NonTerminalSymbol {

public:

	NTStreet(CGAL::MP_Float x, CGAL::MP_Float y, CGAL::MP_Float x_old, CGAL::MP_Float y_old);
	~NTStreet();

	// See NonTerminalSymbol.h
	NonTerminalSymbol* replace();

private:

	/// Current X-coordinate of street
	CGAL::MP_Float x_;
	/// Current Y-coordinate of street
	CGAL::MP_Float y_;

	/// Old X-coordinate of street
	CGAL::MP_Float x_old_;
	/// Old Y-coordinate of street
	CGAL::MP_Float y_old_;

	/** @brief Checks if coordinate is within boundaries
	  *
	  * Checks if the coordinate given (x,y), lies
	  * within the bonds othe the current plane size
	  * that we have. If not, false is returned, true otherwise
	  *
	  * Also the coordinate that is < 0 is set to 0
 	  *
	  * @param CGAL::MP_Float x_new X-Coordinate to check
	  * @param CGAL::MP_Float y_new Y-Coordinate to check
 	  *
	  * @return true if inside of the plane, false otherwises
	  */
	bool coordinateInField(CGAL::MP_Float x_new, CGAL::MP_Float y_new);
};

#endif