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

#include "JSONGenerator.h"
#include <boost/filesystem.hpp>
#include "TreeGenerator.h"
#include "LightingGenerator.h"

//---------------------------------------------------------------------------
void JSONGenerator::writeJSONOut(string file_name) {
	for (Segment_2 curr_river_segment : EntityContainer::getInstance()->river_segments_)
		EntityContainer::getInstance()->all_street_poly_segments_.push_back(curr_river_segment);

	// First generate the Polygons from the
	// line-segments already created by the L-system
	generatePolygons();

	boost::filesystem::path dir("Generated");

	if (!(boost::filesystem::exists(dir)))
		if (!boost::filesystem::create_directory(dir)) {
			cout << "ERROR: Could not create dir..." << endl;
			return;
		}

	writePolyToJSON("Generated\\Lots", "lot", EntityContainer::getInstance()->all_polygons_);

	writePolyToJSON("Generated\\Trees", "tree", EntityContainer::getInstance()->all_trees_);
	writePolyToTxt("Generated\\Trees", "T", EntityContainer::getInstance()->all_trees_);

	writePolyToJSON("Generated\\Rivers", "river", EntityContainer::getInstance()->all_rivers_);
	writePolyToTxt("Generated\\Rivers", "R", EntityContainer::getInstance()->all_rivers_);
	
	vector<vector<Point_2>> all_bridges;

	for (Segment_2 curr_bridge : EntityContainer::getInstance()->bridges_) {
		vector<Point_2> new_bridge;
		new_bridge.push_back(Point_2(curr_bridge.source()));
		new_bridge.push_back(Point_2(curr_bridge.target()));
		all_bridges.push_back(new_bridge);
	}

	writePolyToJSON("Generated\\Bridges", "bridge", all_bridges);
	writePolyToTxt("Generated\\Bridges", "B", all_bridges);

	writePolyToJSON("Generated\\ParkingLots", "parkinglot", EntityContainer::getInstance()->all_parking_lots_);
	writePolyToTxt("Generated\\ParkingLots", "PL", EntityContainer::getInstance()->all_parking_lots_);

	writePolyToJSON("Generated\\LightSources", "lightsource", EntityContainer::getInstance()->all_lights_);
	writePolyToTxt("Generated\\LightSources", "LI", EntityContainer::getInstance()->all_lights_);

	writePolyToJSON("Generated\\GreenSpaces", "greenspace", EntityContainer::getInstance()->all_green_spaces_);
	writePolyToTxt("Generated\\GreenSpaces", "G", EntityContainer::getInstance()->all_green_spaces_);

	std::vector<std::vector<Point_2>> all_parks;
	for (std::map<Point_2, std::vector<Point_2>>::reverse_iterator iter = EntityContainer::getInstance()->all_parks_.rbegin();
	iter != EntityContainer::getInstance()->all_parks_.rend(); ++iter)
		all_parks.push_back(iter->second);

	writePolyToJSON("Generated\\Parks", "park", all_parks);
	writePolyToTxt("Generated\\Parks", "P", all_parks);
}

//---------------------------------------------------------------------------
void JSONGenerator::generatePolygons() {
	// Now all is left is to extract the faces from the 
	// Segments gotten and convexify them
	Arrangement_2  arr;
	Walk_pl        pl(arr);

	// This takes the most time unfortunately
	unsigned counter = 0;
	unsigned total = EntityContainer::getInstance()->all_street_poly_segments_.size();
	for (Segment_2 curr : EntityContainer::getInstance()->all_street_poly_segments_) {
		cout << "2,INSERTING SEGMENT: " << counter++ << "/" << total << endl;
		insert(arr, curr, pl);
	}

	PostScriptIntWriter out_writer;
	Arrangement_2::Face_const_iterator fit;
	Arrangement_2::Ccb_halfedge_const_circulator  first, curr;
	unsigned face_counter = 0;

	// Configure new outwriter for the polygon-
	// postscript file
	out_writer.openFile("poly.ps");
	out_writer.setBackgroundColor(42, 41, 34);

	unsigned total_face_numb = arr.number_of_faces();

	for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit, face_counter++) {
		if (fit->has_outer_ccb()) {
			std::cout << "3,GENERATE POLYGONS: " << face_counter << "/" << total_face_numb << endl;
			curr = first = fit->outer_ccb();

			vector<Point_2> curr_polygon;
			Path subj;
			Paths solution;

			// Collect points of the polygon, remove points inside
			// polygons by checking the degree of them
			do {
				if (!curr->source()->is_at_open_boundary() && curr->source()->degree() >= 2) {
					curr_polygon.push_back(Point_2(CGAL::to_double(curr->source()->point().x()), CGAL::to_double(curr->source()->point().y())));
					subj << IntPoint((int)CGAL::to_double(curr->source()->point().x()), (int)CGAL::to_double(curr->source()->point().y()));
				}
			} while (++curr != first);

			// Block out rivers here (Dirty Fix)
			// Todo: Check why this is still a problem
			if (curr_polygon.size() > 70)
				continue;

			// Clip gotten polygon and iterate all solutions after clipping
			// (Polygon could be split into several parts by clipping)
			ClipperOffset co;
			co.AddPath(subj, jtRound, etClosedPolygon);
			co.Execute(solution, CLIPPING_FACTOR);

			for (unsigned poly_counter = 0; poly_counter < solution.size(); poly_counter++) {
				vector<Point_2> all_points_off;

				double circumference = 0.0f;

				for (unsigned counter = 0; counter < solution.at(poly_counter).size(); counter++) {
					Point_2 curr((double)solution.at(poly_counter).at(counter).X,
						(double)solution.at(poly_counter).at(counter).Y);

					all_points_off.push_back(curr);

					if (counter > 0)
						circumference += sqrt(CGAL::to_double(CGAL::squared_distance(Point_2((double)solution.at(poly_counter).at(counter - 1).X,
							(double)solution.at(poly_counter).at(counter - 1).Y), curr)));
				}

				// Polygons below a certain range become green-spaces
				if (circumference < MAX_GREEN_SPACE_SIZE) {
					out_writer.handlePolygon(all_points_off, 1, 80, 4);
					EntityContainer::getInstance()->all_green_spaces_.push_back(all_points_off);
				}
				else {
					// Put the new polygon into the park map. The n biggest 
					// polygons will later become parks
					EntityContainer::getInstance()->possible_parks_.
						insert(std::pair<double, std::vector<Point_2>>(circumference, all_points_off));
				}

			}
		}
	}

	classifyPolygons(out_writer);
}
//---------------------------------------------------------------------------
void JSONGenerator::classifyPolygons(PostScriptIntWriter& out_writer) {
	// The n biggest polygons will become parks,
	// all others are added as possible lots
	unsigned park_counter = 0;
	for (std::map<double, std::vector<Point_2>>::reverse_iterator iter = EntityContainer::getInstance()->possible_parks_.rbegin();
	  iter != EntityContainer::getInstance()->possible_parks_.rend(); ++iter) {

		// Calculate the approximate mid-point of
		// the current segment to compare it to existing
		// parks (do not want parks nearby)
		double x_mid = 0.0f;
		double y_mid = 0.0f;

		for (Point_2 curr : iter->second) {
			x_mid += (int)CGAL::to_double(curr.x());
			y_mid += (int)CGAL::to_double(curr.y());
		}

		x_mid /= iter->second.size();
		y_mid /= iter->second.size();

		// Gemerate NUMBER_OF_PARKS parks and check that no two parks
		// are directly next to each other (wouldn't look natural)
		if (park_counter < NUMBER_OF_PARKS) {

			bool park_nearby = false;
			for (std::map<Point_2, std::vector<Point_2>>::reverse_iterator iter = EntityContainer::getInstance()->all_parks_.rbegin();
			  iter != EntityContainer::getInstance()->all_parks_.rend(); ++iter)
				if (sqrt(CGAL::to_double(CGAL::squared_distance(Point_2(x_mid, y_mid), iter->first))) < MIN_PARK_DISTANCE)
					park_nearby = true;

			if (!park_nearby) {
				out_writer.handlePolygon(iter->second, 1, 80, 4);
				EntityContainer::getInstance()->all_parks_.insert(std::pair<Point_2, std::vector<Point_2>>(Point_2(x_mid, y_mid), iter->second));
				park_counter++;

				// New park found here, now create the trees inside the park
				std::vector<Point_2> all_trees = TreeGenerator::generateTrees(iter->second);
				out_writer.handleTrees(all_trees);
				EntityContainer::getInstance()->all_trees_.push_back(all_trees);
				continue;
			}
		}

		// Convexify the gotten polygons by using CGAL´s implemented
		// "optimal_convex_partition_2" functionality
		Polygon_2 curr_poly;

		Polygon_list partition_polys;
		Traits       partition_traits;

		// Insert Points into Polygon
		for (Point_2 curr_point : iter->second)
			curr_poly.push_back(Traits::Point_2(CGAL::to_double(curr_point.x()), CGAL::to_double(curr_point.y())));

		// Needed as Clipper-Lib sometimes produced non
		// simple polygons where e.g. one vertex was out 
		// of the rest of the polygon, if one is found 
		// we will make it to a greenspace.
		if (curr_poly.is_simple()) {

			// Handle lighting of outside polygon
			std::vector<Point_2> all_lights = LightingGenerator::generateStreetLights(iter->second);
			std::vector<Point_2> all_used_lights;

			for(Point_2 curr : all_lights)
				if ((unsigned)(rand() % 100) < EntityContainer::getInstance()->LIGHT_PROB)
					all_used_lights.push_back(curr);

			out_writer.handleLights(all_used_lights);
			EntityContainer::getInstance()->all_lights_.push_back(all_used_lights);

			CGAL::optimal_convex_partition_2(curr_poly.vertices_begin(),
				curr_poly.vertices_end(),
				std::back_inserter(partition_polys),
				partition_traits);

			for (Polygon_2 curr_convex_poly : partition_polys)
			{
				std::vector<Point_2> tmp;
				for (VertexIterator vi = curr_convex_poly.vertices_begin(); vi != curr_convex_poly.vertices_end(); ++vi)
					tmp.push_back(Point_2(vi->x(), vi->y()));

				// Check if gotten polygon it bigger than a minium size,
				// if not convert it to e.g. greenspace or parking lots.
				if (curr_convex_poly.area() > MIN_PARKING_LOT_AREA && curr_convex_poly.area() < MIN_LOT_AREA) {
					out_writer.handlePolygon(tmp, 109, 125, 126);
					EntityContainer::getInstance()->all_parking_lots_.push_back(tmp);
				} else if (curr_convex_poly.area() < MIN_PARKING_LOT_AREA) {
					out_writer.handlePolygon(tmp, 1, 142, 14);
					EntityContainer::getInstance()->all_green_spaces_.push_back(tmp);
				} else {
					out_writer.handlePolygon(tmp, 149, 165, 166);
					EntityContainer::getInstance()->all_polygons_.push_back(tmp);
				}
			}
		}
		else {
			out_writer.handlePolygon(iter->second, 1, 142, 14);
			EntityContainer::getInstance()->all_green_spaces_.push_back(iter->second);
		}
	}

	// Now print all the rivers
	for (vector<Point_2> curr_river_polygon : EntityContainer::getInstance()->all_rivers_)
		out_writer.handlePolygon(curr_river_polygon, 12, 77, 115);

	out_writer.handleBridges(EntityContainer::getInstance()->bridges_);
	out_writer.closeFile();
}


//---------------------------------------------------------------------------
void JSONGenerator::writePolyToJSON(string dir_name, string file_name, std::vector<std::vector<Point_2>> vec_out) {
	// Check if the Lots directory does already exist,
	// if not create it
	boost::filesystem::path dir(dir_name);

	if (!(boost::filesystem::exists(dir))) {
		if (!boost::filesystem::create_directory(dir)) {
			cout << "ERROR: Could not create dir..." << endl;
			return;
		}
	}

	// Every lot gets its own file
	unsigned lot_counter = 0;
	for (vector<Point_2> curr_lot : vec_out) {
		ofstream myfile;
		myfile.open(dir_name + "\\" + file_name + to_string(lot_counter++) + ".json");

		myfile << "{\n";

		// Calculate direction of longest expansion 
		// (longest distance between two points). Needed in GML later.
		if (file_name.compare("lot") == 0) {

            // Write Day/Nightmode to the file
			myfile << "  \"daymode\":[\n";
			myfile << "    {\"D\":\"" << EntityContainer::getInstance()->DAY_MODE
				<< "\"}\n";
			myfile << "  ],\n";

			myfile << "  \"norm\":[\n";

			unsigned poly_count = curr_lot.size();

			Point_2 first;
			Point_2 second;
			double max_dist = 0;

			for (unsigned i = 0; i < poly_count; i++) {
				Point_2 curr_point = curr_lot.at(i);
				Point_2 next_point = curr_lot.at((i + 1) % poly_count);

				double curr_dist = sqrt(CGAL::to_double(CGAL::squared_distance(curr_point, next_point)));
				if (max_dist < curr_dist) {
					max_dist = curr_dist;
					first = curr_point;
					second = next_point;
				}
			}

			double x1 = CGAL::to_double(first.x());
			double y1 = CGAL::to_double(first.y());
			double x2 = CGAL::to_double(second.x());
			double y2 = CGAL::to_double(second.y());
			double x_norm = 0.0f;
			double y_norm = 0.0f;

			if (x1 >= x2 && y1 >= y2) {
				x_norm = x1 - x2;
				y_norm = y1 - y2;
			}
			else {
				x_norm = x2 - x1;
				y_norm = y2 - y1;
			}

			myfile << "    {\"X\":\"" << x_norm
				<< "\", \"Y\":\"" << y_norm << "\"}\n";

			myfile << "  ],\n";
		}


		myfile << "  \"" + file_name + "\":[\n";

		for (Point_2 curr_vert : curr_lot) {
			myfile << "    {\"X\":\"" << (int)CGAL::to_double(curr_vert.x())
				<< "\", \"Y\":\"" << (int)CGAL::to_double(curr_vert.y()) << "\"}";

			if (curr_vert != curr_lot.back())
				myfile << ",";

			myfile << "\n";
		}

		myfile << "  ]\n}\n";
		myfile.close();
	}
}



//---------------------------------------------------------------------------
void JSONGenerator::writePolyToTxt(string dir_name, string file_name, std::vector<std::vector<Point_2>> vec_out) {
	unsigned counter = 0;
	for (vector<Point_2> curr_poly : vec_out) {
		if (curr_poly.size() > 0) {
			ofstream myfile(dir_name + "\\\\" + file_name + std::to_string(counter++) + ".txt");
			if (myfile.is_open())
			{
				for (Point_2 curr_vert : curr_poly)
					myfile << "" + std::to_string(CGAL::to_double(curr_vert.x())) + " " + std::to_string(CGAL::to_double(curr_vert.y())) + "\n";
				myfile.close();
			}
		}
	}
}
