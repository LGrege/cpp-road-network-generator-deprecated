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

#include "TreeGenerator.h"
#include "Clipper.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>

using namespace ClipperLib;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_2<K>         Triangulation;
typedef Triangulation::Point             Point;

//---------------------------------------------------------------------------
std::vector<Point_2> TreeGenerator::generateTrees(std::vector<Point_2> park) {
	vector<Point_2> all_trees;

	// First clip the park as we do not want trees
	// directly on the parks outlines
	Path subj;
	Paths solution;

	for (Point_2 curr_vert : park)
		subj << IntPoint((int)CGAL::to_double(curr_vert.x()), (int)CGAL::to_double(curr_vert.y()));

	ClipperOffset co;
	co.AddPath(subj, jtRound, etClosedPolygon);
	co.Execute(solution, CLIPPING_FACTOR);

	// As by clipping the park could be split into multiple
	// polygons, we need to iterate all of them.
	for (unsigned poly_counter = 0; poly_counter < solution.size(); poly_counter++) {
		Polygon_2 park_polygon;
		Triangulation park_triangulation;

		for (unsigned counter = 0; counter < solution.at(poly_counter).size(); counter++) {
			double curr_x = (double)solution.at(poly_counter).at(counter).X;
			double curr_y = (double)solution.at(poly_counter).at(counter).Y;

			park_triangulation.insert(Point(curr_x, curr_y));
			park_polygon.push_back(Traits::Point_2(curr_x, curr_y));
		}

		// Now use the above generated triangulation of the park to randomly place
		// a point inside it
		for (Triangulation::Finite_faces_iterator fi = park_triangulation.finite_faces_begin(); fi != park_triangulation.finite_faces_end(); fi++) {
			Point_2 point0 = Point_2(fi->vertex(0)->point().x(), fi->vertex(0)->point().y());
			Point_2 point1 = Point_2(fi->vertex(1)->point().x(), fi->vertex(1)->point().y());
			Point_2 point2 = Point_2(fi->vertex(2)->point().x(), fi->vertex(2)->point().y());

			double r1 = sqrt(((double)rand() / (RAND_MAX)));
			double r2 = ((double)rand() / (RAND_MAX));

			CGAL::MP_Float rand_x = (1 - sqrt(r1)) * CGAL::to_double(point0.x()) + (sqrt(r1) * (1 - r2)) * CGAL::to_double(point1.x()) + (sqrt(r1)*r2) * CGAL::to_double(point2.x());
			CGAL::MP_Float rand_y = (1 - sqrt(r1)) * CGAL::to_double(point0.y()) + (sqrt(r1) * (1 - r2)) * CGAL::to_double(point1.y()) + (sqrt(r1)*r2) * CGAL::to_double(point2.y());
			Point_2 new_tree = Point_2(rand_x, rand_y);

			// Check for nearby trees, don't want them to clutter and 
			// also check if point inside park (triangulation could lead to
			// points outside of park).
			if (!treeNearby(new_tree, all_trees) && park_polygon.has_on_bounded_side(
				Traits::Point_2(CGAL::to_double(new_tree.x()), CGAL::to_double(new_tree.y()))))
				all_trees.push_back(new_tree);
		}
	}

	return all_trees;
}

//---------------------------------------------------------------------------
bool TreeGenerator::treeNearby(Point_2 tree, vector<Point_2> all_trees) {

	for (Point_2 curr_tree : all_trees)
		if (sqrt(CGAL::to_double(CGAL::squared_distance(curr_tree, tree))) < MIN_TREE_DISTANCE)
			return true;

	return false;
}