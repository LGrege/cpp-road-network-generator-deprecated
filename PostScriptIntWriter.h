#pragma once

#ifndef __POST_SCRIPT_INT_WRITER__
#define __POST_SCRIPT_INT_WRITER__

#include "IInterpretable.h"
#include "CGALDefs.h"
#include <iostream>
#include <fstream>

using namespace std;

/**
  * @class PostScriptIntWriter
  *
  * @brief Generates Output (.pst File) from generated information
  * 
  * Simples interpreter, implements the interpretable
  * interface and prints the events to a postscript file
  *
  * @note Contains handlers for all information gathered during creation
  * @note Most handlers triggered by JSONGenerator 
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
class PostScriptIntWriter : public IInterpretable {

  public:

	PostScriptIntWriter();
	~PostScriptIntWriter();

	// Handlers, see IInterpretable for further informations
	void handleInterception(CGAL::MP_Float x, CGAL::MP_Float y);
	void handleHighwayInterception(CGAL::MP_Float x, CGAL::MP_Float y);
	void handleHighDensityPoint(CGAL::MP_Float x, CGAL::MP_Float y, int max_diameter);

	void handleStreetSegments(vector<Segment_2> streets);
	void handleHighwaySegments(vector<Segment_2> highways);
	void handleRiver(vector<Segment_2> river);
	void handleError(vector<Segment_2> river);
	void handleBridges(vector<Segment_2> bridges);
	void handleTrees(vector<Point_2> trees);
	void handleLights(vector<Point_2> lights);

	void handleNewHighwayStart(CGAL::MP_Float x, CGAL::MP_Float y);
	void handleHighwayExit(CGAL::MP_Float x, CGAL::MP_Float y);

	void startNetwork();
	void closeNetwork();

	void openFile(string file_name);
	void closeFile();

	void setBackgroundColor(unsigned r, unsigned g, unsigned b);
	void handlePolygon(vector<Point_2> polygon, unsigned r, unsigned g, unsigned b);

  private:

	/// File used by the class to read/write to
	ofstream output_file_;
};

#endif