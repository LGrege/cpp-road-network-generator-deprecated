#pragma once

#ifndef __TREE_GENERATOR__
#define __TEEE_GENERATOR__

#include "CGALDefs.h"

using namespace std;

#define MIN_TREE_DISTANCE 10

/**
  * @class TreeGenerator
  *
  * @brief Generates the trees (Placement in park)
  *
  * Used for placing trees based on a given park. The trees can be palced
  * using different methods (e.g. random placement).
  *
  * The parks are trinagulated and within the triangles a random
  * point is chosen as position for the trees.
  *
  * @note Currently trees are placed randomly inside the parks only
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
class TreeGenerator {
  public:

	  /** @brief Generate the trees inside the park specified by the parameter
	    * 
	    * Method returns a vector of points, containind the positions for the
	    * trees inside a park (Specified by the given parameters).
	    *
	    * @parm std::vector<Point_2> park, Coordinates of the parks vertices
		*
	    * @return std::vector<Point_2> positions of the trees inside the park
	    */
	  static std::vector<Point_2> generateTrees(vector<Point_2> park);

	  /** @brief Checks if a tree is nearby another one
	    *
	    * Method is used to determine if a tree is nearby. If so, we do not wan't to
	    * create a new tree directly next to it.
	    *
	    * @parm Point_2 tree new tree
	    * @parm vector<Point_2> all_trees all trees already created
	    *
	    * @return bool true if not trees within MIN_TREE_DISTANCE range, false otherwise
	    */
	  static bool treeNearby(Point_2 tree, vector<Point_2> all_trees);

  private:

};


#endif
