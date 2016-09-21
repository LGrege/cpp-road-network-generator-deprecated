#pragma once

#ifndef _IINTERPRETABLE_
#define _IINTERPRETABLE_

#include <CGAL/MP_Float.h>
#include "CGALDefs.h"

/**
  * @Interface IInterpretable
  *
  * @brief Interface declares interpretors for the gotten results.
  *
  * Different interpretors could be added (Different output formats). Currently only the
  * PostScriptIntWriter makes use of this interface
  *
  * Contains different handler functions for the different data collected throughout the
  * generation, as well as helper functions for creating the network.
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
class IInterpretable
{
  public:
	  virtual ~IInterpretable() {};

	  // Handlers for the different possible 
	  // events that may occur during parsing
	  virtual void handleRiver(std::vector<Segment_2> river) = 0;
	  virtual void handleError(std::vector<Segment_2> error) = 0;
	  virtual void handleBridges(std::vector<Segment_2> bridges) = 0;
	  virtual void handleStreetSegments(std::vector<Segment_2> streets) = 0;
	  virtual void handleHighwayExit(CGAL::MP_Float x, CGAL::MP_Float y) = 0;
	  virtual void handleHighwaySegments(std::vector<Segment_2> highways) = 0;
	  virtual void handleNewHighwayStart(CGAL::MP_Float x, CGAL::MP_Float y) = 0;
	  virtual void handleHighDensityPoint(CGAL::MP_Float x, CGAL::MP_Float y, int max_diameter) = 0;
	  virtual void handleHighwayInterception(CGAL::MP_Float x, CGAL::MP_Float y) = 0;
	  virtual void handleInterception(CGAL::MP_Float new_x, CGAL::MP_Float new_y) = 0;
	  virtual void handlePolygon(std::vector<Point_2> polygon, unsigned r, unsigned g, unsigned b) = 0;

	  // Starts and closes a path/network used for 
	  // generating the output (simples case = open new path in file)
	  virtual void closeNetwork() = 0;
	  virtual void startNetwork() = 0;
	  virtual void closeFile() = 0;
	  virtual void openFile(std::string file_name) = 0;

	  /// Set background color of the output map
	  virtual void setBackgroundColor(unsigned r, unsigned g, unsigned b) = 0;
};


#endif