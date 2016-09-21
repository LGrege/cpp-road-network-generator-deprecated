#pragma once
#pragma once

#ifndef __LIGHTING_GENERATOR__
#define __LIGHTING_GENERATOR__

#include "CGALDefs.h"

using namespace std;

/**
  * @class LightingGenerator
  *
  * @brief Used for placing Lights around the city. Different patterns for
  *        the lighting can be chosen here
  *
  * @note Only \b random lighting placement along the lot-edges implemented currently
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
class LightingGenerator {

public:

	/** @brief Main method which palces the lights
	  *
	  * Generates street lights alongisde the boundaries of a lot (random)
	  * and return them for further computation (drawing and export)
	  *
	  * @param vector<Point_2> lot, Coordinates of the lots vertices
	  *
	  * @return vector<Point_2> positions of the lamps along the road
	  *
	  */
	static vector<Point_2> generateStreetLights(vector<Point_2> lot);

private:

};


#endif
