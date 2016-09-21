/**
 *
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
 *
*/

#pragma once

#ifndef __CGAL_DEFS__
#define __CGAL_DEFS__
#define _USE_MATH_DEFINES

#include <CGAL/Cartesian.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>
#include <CGAL/Point_set_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

/********************************************//**
* \brief needed CGAL-Typedefs
***********************************************/
typedef CGAL::Quotient<CGAL::MP_Float>      NT;
typedef CGAL::Cartesian<NT>                 Kernel;
typedef Kernel::Point_2                     Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>  Traits_2;
typedef Traits_2::Curve_2                   Segment_2;
typedef Kernel::Vector_2                    Vector_2;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Partition_traits_2<K>                         Traits;
typedef Traits::Polygon_2                                   Polygon_2;
typedef std::list<Polygon_2>                                Polygon_list;
typedef Polygon_2::Vertex_iterator VertexIterator;

/********************************************//**
* \brief Construction Definitions
*
* Values define behaviour during of street
* creation
***********************************************/
#define DISTANCE 12
#define RIVER_DISTANCE 10
#define MAX_RIVER_WIDTH 20
#define MAX_RIVER_ANGLE 7.0f
#define CLIPPING_FACTOR -1.5
#define MAX_GREEN_SPACE_SIZE 20
#define NUMBER_OF_PARKS 20
#define MIN_PARK_DISTANCE 80
#define MIN_LOT_AREA 40
#define MIN_PARKING_LOT_AREA 10
#define MAX_GENERATION_ITERS 1000
#define MIN_LIGHT_DISTANCE 4
#define HIGHWAY_SNAP_DISTANCE 30.0f

#endif