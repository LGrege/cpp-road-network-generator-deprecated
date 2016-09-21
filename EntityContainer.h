#pragma once

#ifndef __ENTITY_CONTAINER__
#define __ENTITY_CONTAINER__

#include "CGALDefs.h"

// Search radius within the grid
#define X_OFFSET 1
#define Y_OFFSET 1

// Max street angle for intersection merging
#define MAX_ANGLE 40

/**
  * @class EntityContainer
  *
  * @brief Singleton Container class used by all NonTerminalSymbol classes.
  *
  * Holds all segments, all intersection and other needed informations. Class is a singleton
  * and is used allover the project as a \b shared \b information \b container (if necessary). 
  *
  * @note Also holds functions for intersection checking
  * @note Some variables will be read from input file (capitalized variables)
  * 
  * @author Lukas Gregori
  * @version $Revision: 1.0 $
  * 
  * Contact: lukas.gregori@student.tugraz.at
  *
  * Class implemented as singleton 
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
class EntityContainer {
public:

	/// Pobability that subsegment branches
	unsigned BRANCH_PROB = 88;

	/// Probability that lights will be created
	unsigned LIGHT_PROB = 33;

    /// Number of rivers
	unsigned NUMB_RIVERS = 0;

	/// X-Dimension of output grid
	unsigned MAX_X = 600;
	/// Y-Dimension of output grid
	unsigned MAX_Y = 800;

	/// Creation threshold
	int MAX_SEG_COUNT = 8000;
	int MAX_SEG_COUNT_INIT = 8000;

	/** Dependant on this variable an export flag is set 
	  * for the city generator (GML), which will then handle
	  * the textures for the windows differently (Will emitt light). */
	int DAY_MODE = true;

    /** Number of highways total. Used for
	  * street-start creation. Streets may only
	  * propagate ("grow") once all highways
	  * were closed. */
	int HIGHWAY_COUNT;

	static EntityContainer* getInstance();
	~EntityContainer();

	/// Highway goal points
	std::vector<Point_2> high_dens_points_;
	/// Highway start points
	std::vector<Point_2> start_points_;

	/// Container for all intersections (Street and Highway) 
	std::vector<Point_2>   all_intersection_;
	/// Container for all street segments
	std::vector<Segment_2> all_street_segments_;
	/// Container for all highway intersections
	std::vector<Point_2>   all_highway_intersections_;
	/// Container for all highway exits (branch points)
	std::vector<Point_2>   all_highway_exits_;
	/// Container for all highway segments
	std::vector<Segment_2> all_highway_segments_;
	/// Container for all river segments
	std::vector<Segment_2> river_segments_;
	/// Container for all bridges
	std::vector<Segment_2> bridges_;

	/** All polygons gotten from the JSON-Generator
	  * using the line segments from the ..._poly_segments
	  * vectors below. */
	std::vector<std::vector<Point_2>> all_polygons_;
	std::vector<std::vector<Point_2>> all_green_spaces_;
	std::vector<std::vector<Point_2>> all_lots_;
	std::vector<std::vector<Point_2>> all_parking_lots_;
	std::vector<std::vector<Point_2>> all_rivers_;
	std::vector<std::vector<Point_2>> all_trees_;
	std::vector<std::vector<Point_2>> all_lights_;
	std::map<Point_2, std::vector<Point_2>>  all_parks_;
	std::map<double, std::vector<Point_2>>  possible_parks_;

	/** @brief Segments for polygon creation
	  *
	  * Stores the segments needed for the polygon creation.
	  * We create a seperate vector which does not stop at
	  * intersections, but rather continues drawing a bit, so 
	  * that the 2D-Arrangements can detect an intersection */
	std::vector<Segment_2> all_highway_poly_segments_;
	std::vector<Segment_2> all_street_poly_segments_;

	/** @brief Grid used for neighbour detection
	  *
	  * If we need the neighbours of one point, we check 
	  * the corresponding square. We can thus compute the neighbours
	  * by a simple lookup operation \c (O(1)) */
    std::vector<Segment_2> street_segment_grid[100][100];
	/// Grid used for intersection point. See street_segment_grid for further explanantion
	std::vector<Point_2> intersection_grid[100][100];
	/// Grid used for river segments. See street_segment_grid for further explanantion
	std::vector<Segment_2> river_segment_grid[100][100];

	/** @brief Gets grid cell depending on x and y coordinate
	  * 
	  * Returns the indizes of the grid cell in which x and
	  * y belong. Cann be used as index to the points grid.
	  *
	  * @param CGAL::MP_Float x x-coordinate
	  * @param CGAL::MP_Float y y-coordinate
	  *
	  * @return Point_2 The cell the coordinates are in
	  */
    Point_2 getGridCell(CGAL::MP_Float x, CGAL::MP_Float y);

    /// Insert new street segment into grid-system
	void insertStreetSegment(Segment_2 new_segment);

	/// Insert new intersection into grid-system
	void insertIntersection(Point_2 intersection);

	/// Insert new river segment into grid-system
	void insertRiverSegment(Segment_2 new_segment);

	/** @brief Checks if segment intersects with another one
	  *
	  * Checks if the segment which starts at x/y and ends at x_new/y_new
	  * intersects with any other segment within a hard-coded range.
  	  *
	  * If the segment intercepts with one other segments, the closest
	  * interception point is returned
 	  *
	  * @param CGAL::MP_Float x start X of new segment
	  * @param CGAL::MP_Float y start Y of new segment
	  * @param CGAL::MP_Float x_new end X of new segment
	  * @param CGAL::MP_Float y_new end Y of new segment
	  * @param unsigned old_angle angle (Intersection merging)
	  * @param int x_off Grid Range X
	  * @param int y_off Grid Range Y
	  * 
	  * @return Point_2 the next closest interception_point, P(0,0) if no intersection was found
	  */
	Point_2 segmentIntersected(CGAL::MP_Float x, CGAL::MP_Float y, 
		CGAL::MP_Float x_new, CGAL::MP_Float y_new, unsigned old_angle, int x_off, int y_off);

	/** @brief Gets the nearest intersection
	  *
	  * Calculates the nearest intersection point (if found), by using
	  * the grid system from the EntityContainer class.
  	  *
	  * If no intersection could be detected, the method returns P(0,0)
	  *
	  * @param CGAL::MP_Float x Coordinate of POI
	  * @param CGAL::MP_Float y Coordinate of POI
	  * @param int x_off Grid Range X
	  * @param int y_off Grid Range Y
	  *
	  * @return Point_2 the next closest interception_point, P(0,0) if no intersection was found
	  */
	Point_2 getNearestIntersection(CGAL::MP_Float x, CGAL::MP_Float y);

	/** @brief Checks if an existin river would be intersected by inserting the
  	  *        point specified by the paramters
	  *
	  * @param CGAL::MP_Float x start X of new segment
	  * @param CGAL::MP_Float y start Y of new segment
	  * @param CGAL::MP_Float x_new end X of new segment
	  * @param CGAL::MP_Float y_new  end Y of new segment
	  * @param int x_off, int y_off Grid offset
 	  *
	  * @returen Point_2 the next closest interception_point, P(0,0) if no intersection was found
	  */
	std::vector<Point_2> riverIntersected(CGAL::MP_Float x, CGAL::MP_Float y,
		CGAL::MP_Float x_new, CGAL::MP_Float y_new, int x_off, int y_off);

private:

	static EntityContainer* instance_;
	EntityContainer();
};

#endif