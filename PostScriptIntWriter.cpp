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

#include "PostScriptIntWriter.h"
#include "EntityContainer.h"
#include "CGALDefs.h"

//---------------------------------------------------------------------------
PostScriptIntWriter::PostScriptIntWriter() {
}

//---------------------------------------------------------------------------
PostScriptIntWriter::~PostScriptIntWriter() {
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::openFile(std::string file_name) {
	output_file_.open(file_name);
	output_file_ << "% !\n";
	output_file_ << "%%BeginSetup\n";
	output_file_ << "  << /PageSize [" << EntityContainer::getInstance()->MAX_X << " " 
		<< EntityContainer::getInstance()->MAX_Y << "] /Orientation 0 >> setpagedevice\n";
	output_file_ << "%%EndSetup\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::closeFile() {
	output_file_ << "showpage             % Seite ausgeben\n";

	if (output_file_.is_open())
		output_file_.close();
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleInterception(CGAL::MP_Float x, CGAL::MP_Float y) {
	CGAL::MP_Float x_new = (x < 0.001f) ? 0.0f : x;
	CGAL::MP_Float y_new = (y < 0.001f) ? 0.0f : y;

	output_file_ << x_new << " " << y_new << " 1 0 360 arc closepath\n";
	output_file_ << "0.7 0 0 0 setrgbcolor fill\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleHighwayInterception(CGAL::MP_Float x, CGAL::MP_Float y) {
	CGAL::MP_Float x_new = (x < 0.001f) ? 0.0f : x;
	CGAL::MP_Float y_new = (y < 0.001f) ? 0.0f : y;

	output_file_ << x_new << " " << y_new << " 5 0 360 arc closepath\n";
	output_file_ << "0.7 0 0 1 setrgbcolor fill\n";
}


//---------------------------------------------------------------------------
void PostScriptIntWriter::handleHighDensityPoint(CGAL::MP_Float x, CGAL::MP_Float y, int max_diameter) {
	CGAL::MP_Float x_new = (x < 0.001f) ? 0.0f : x;
	CGAL::MP_Float y_new = (y < 0.001f) ? 0.0f : y;

	output_file_ << x_new << " " << y_new << " " << max_diameter << " 0 360 arc closepath\n";
	output_file_ << x_new << " " << y_new << " " << max_diameter / 2 + 2 << " 360 0 arcn closepath\n";
	output_file_ << "0.7 0.91 0.91 0.91 setrgbcolor fill\n";
	output_file_ << x_new << " " << y_new << " " << max_diameter / 2 << " 0 360 arc closepath\n";
	output_file_ << x_new << " " << y_new << " " << max_diameter / 10 + 2 << " 360 0 arcn closepath\n";
	output_file_ << "0.7 0.71 0.71 0.81 setrgbcolor fill\n";
	output_file_ << x_new << " " << y_new << " " << max_diameter / 10 << " 0 360 arc closepath\n";
	output_file_ << "0.7 1 0.61 0.61 setrgbcolor fill\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::startNetwork() {
	output_file_ << "\nnewpath\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::closeNetwork() {
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleStreetSegments(std::vector<Segment_2> streets) {
	output_file_ << "\nnewpath\n";
	output_file_ << "0.67 0.67 0.67 setrgbcolor\n";
	for (Segment_2 curr_street : streets) {
		output_file_ << CGAL::to_double(curr_street.source().x()) << " " << CGAL::to_double(curr_street.source().y()) << " moveto\n";
		output_file_ << CGAL::to_double(curr_street.target().x()) << " " << CGAL::to_double(curr_street.target().y()) << " lineto\n";
	}
	output_file_ << "1 setlinewidth\n";
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleHighwaySegments(std::vector<Segment_2> highways) {
	output_file_ << "\nnewpath\n";
	output_file_ << "1 0 0 setrgbcolor\n";
	for (Segment_2 curr_street : highways) {
		output_file_ << CGAL::to_double(curr_street.source().x()) << " " << CGAL::to_double(curr_street.source().y()) << " moveto\n";
		output_file_ << CGAL::to_double(curr_street.target().x()) << " " << CGAL::to_double(curr_street.target().y()) << " lineto\n";
	}
	output_file_ << "4 setlinewidth\n";
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleRiver(std::vector<Segment_2> river) {
	output_file_ << "\nnewpath\n";
	output_file_ << "0 0 1 setrgbcolor\n";
	for (Segment_2 curr_street : river) {
		output_file_ << curr_street.source().x().num << " " << curr_street.source().y().num << " moveto\n";
		output_file_ << curr_street.target().x().num << " " << curr_street.target().y().num << " lineto\n";
	}
	output_file_ << "3 setlinewidth\n";
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleError(std::vector<Segment_2> river) {
	output_file_ << "\nnewpath\n";
	output_file_ << "1 1 0 setrgbcolor\n";
	for (Segment_2 curr_street : river) {
		output_file_ << CGAL::to_double(curr_street.source().x()) << " " << CGAL::to_double(curr_street.source().y()) << " moveto\n";
		output_file_ << CGAL::to_double(curr_street.target().x()) << " " << CGAL::to_double(curr_street.target().y()) << " lineto\n";
	}
	output_file_ << "3 setlinewidth\n";
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleBridges(std::vector<Segment_2> bridges) {
	output_file_ << "\nnewpath\n";
	output_file_ << 213.0f / 256.0f << " " << 196.0f / 256.0f << " "<< 161.0f / 256.0f << " setrgbcolor\n";
	for (Segment_2 curr_bridge : bridges) {
		output_file_ << CGAL::to_double(curr_bridge.source().x()) << " " << CGAL::to_double(curr_bridge.source().y()) << " moveto\n";
		output_file_ << CGAL::to_double(curr_bridge.target().x()) << " " << CGAL::to_double(curr_bridge.target().y()) << " lineto\n";
	}
	output_file_ << "4 setlinewidth\n";
	output_file_ << "stroke closepath\n\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleTrees(std::vector<Point_2> trees) {
	for (Point_2 curr_tree : trees) {		
		CGAL::MP_Float x_new = (CGAL::to_double(curr_tree.x()) < 0.001f) ? 0.0f : CGAL::to_double(curr_tree.x());
		CGAL::MP_Float y_new = (CGAL::to_double(curr_tree.y()) < 0.001f) ? 0.0f : CGAL::to_double(curr_tree.y());

		output_file_ << x_new << " " << y_new << " 1.5 0 360 arc closepath\n";
		output_file_ << "1 0 0.7 0 setrgbcolor fill\n";
	}
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleLights(std::vector<Point_2> lights) {
	for (Point_2 curr_light : lights) {
		CGAL::MP_Float x_new = (CGAL::to_double(curr_light.x()) < 0.001f) ? 0.0f : CGAL::to_double(curr_light.x());
		CGAL::MP_Float y_new = (CGAL::to_double(curr_light.y()) < 0.001f) ? 0.0f : CGAL::to_double(curr_light.y());

		output_file_ << x_new << " " << y_new << " 0.5 0 360 arc closepath\n";
		output_file_ << "1 1 1 1 setrgbcolor fill\n";
	}
}


//---------------------------------------------------------------------------
void PostScriptIntWriter::handleNewHighwayStart(CGAL::MP_Float x, CGAL::MP_Float y) {
	output_file_ << x << " " << y << " 12 0 360 arc closepath\n";
	output_file_ << x << " " << y << " 9 360 0 arcn closepath\n";
	output_file_ << "0.7 0 1 0 setrgbcolor	 fill\n";
	output_file_ << x << " " << y << " 5 0 360 arc closepath\n";
	output_file_ << "0.7 0 1 0 setrgbcolor	 fill\n";
	output_file_ << "0 0 0 setrgbcolor\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handleHighwayExit(CGAL::MP_Float x, CGAL::MP_Float y){
	CGAL::MP_Float x_new = (x < 0.001f) ? 0.0f : x;
	CGAL::MP_Float y_new = (y < 0.001f) ? 0.0f : y;

	output_file_ << x_new << " " << y_new << " 3 0 360 arc closepath\n";
	output_file_ << "0.3 0 0 0 setrgbcolor fill\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::handlePolygon(std::vector<Point_2> polygon, unsigned r, unsigned g, unsigned b) {
	output_file_ << "\nnewpath\n";
	output_file_ << r / 255.0f << " " << g / 255.0f << " " << b / 255.0f << " setrgbcolor\n";

	bool first = true;

	for (Point_2 curr_point : polygon) {

		double curr_x = CGAL::to_double(curr_point.x());
		double curr_y = CGAL::to_double(curr_point.y());

		if (first) {
			output_file_ << curr_x << " " << curr_y << " moveto\n";
			first = false;
		}

		output_file_ << curr_x << " " << curr_y << " lineto\n";
	}

	output_file_ << "closepath\n";
	output_file_ << "fill\n";
}

//---------------------------------------------------------------------------
void PostScriptIntWriter::setBackgroundColor(unsigned r, unsigned g, unsigned b) {
	output_file_ << r / 255.0f << " " << g / 255.0f << " " << b / 255.0f << " setrgbcolor clippath fill\n";
}
