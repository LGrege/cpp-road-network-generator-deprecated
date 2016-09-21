#pragma once

#ifndef __L_GENERATOR__
#define __L_GENERATOR__

#include <iostream>
#include <string>
#include "CGALDefs.h"
#include <iostream>
#include <string>
#include <stack>
#include <math.h>
#include "IInterpretable.h"
#include "PostScriptIntWriter.h"
#include "CGALDefs.h"

using namespace std;

/**
  * @class LGenerator
  *
  * @brief LGenerator class for creating and managing used L-System
  *
  * The LGenerator class is used to generate the \b L-System
  * that is later handed over to the interpreter to
  * generate the output.
  *
  * @note Uses members if NonTerminalSymbol class for generation
  * @note L-System implemented as self replacing linkes list
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
class LGenerator {

public:

	LGenerator();
	~LGenerator();

	/** @brief Main method for generating the road network
	  *
	  *
	  * Method is used to generate the output that will be handed
	  * to the interpreter. Does so by using L-Systems (Self replacing
	  * linked list of NonTerminalSymbol members).
	  *
	  * @param string file_name, name of the output-ps file
	  */
	void generateRoadNetwork(string file_name);

private:
	/// Interpretable-writer which will create our output
	PostScriptIntWriter int_writer_;

	/// Streets segments container 
	static vector<Segment_2> all_street_segments_;
	/// Highway segments container 
	static vector<Segment_2> all_highway_segments_;

	/** @brief Calls handlers for IInterpretables
	  *
	  * Calls the handlers of the IInterpretable, writer for the
	  * different containers within the EntityContainer-Class
	  *
	  * @param string file_name Name of the output file
	  */
	void interpret(string file_name);

	/** @brief Checks for segment intersection
	  *
	  * Called at the end of the generation of the segments.
	  * Checks if any of the segments intersects with another and
	  * if so calls a method in the interpreter to draw a new segment.
	  */
	void handleCrossings();

	/** @brief Handles the end-points of the highway segments (branch points)
	  *
	  * Method is used to handle (draw) the highway exits saved in
	  * a vector inside of the EntityContainer class
	  */
	void handleHighwayExits();

	/** @brief Handles the goal points of highway generation
	  *
	  * Used to draw all the high density points set by
	  * the user (or at least calls the handler in the interpreter).
	  *
	  */
	void handleHighDensePoints();

	/** @brief Handles the start points of the highways
	  *
	  * Used to draw all the highway-start points set by
	  * the user (or at least calls the handler in the interpreter).
	  *
	  */
	void handleHighwayStartPoints();

	/** @brief Used to generate the rifers bevore highway creation
	  *
	  * Generates river_count-Rivers, which start at arbitrary positions
	  * on the edge of the generation and branch towards the other side.
	  *
	  * @param unsigned river_count Number of rivers
	  *
	  */
	void generateRivers(unsigned river_count);
};

#endif