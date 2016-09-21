# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.


project( RoadGen )

cmake_minimum_required(VERSION 2.8.10)

find_package(CGAL QUIET COMPONENTS Core )

if ( CGAL_FOUND )

  include( ${CGAL_USE_FILE} )

  include( CGAL_CreateSingleSourceCGALProgram )

  include_directories (BEFORE "../../include")

  create_single_source_cgal_program( "main.cpp" "IInterpretable.h" "PostScriptIntWriter.h" 
  "PostScriptIntWriter.cpp" "LGenerator.h" "LGenerator.cpp" "CGALDefs.h" "NonTerminalSymbol.h" "NonTerminalSymbol.cpp" "NTStart.h" "NTStart.cpp" "NTHighway.h" "NTHighway.cpp"
  "NTHighwaySegment.h" "NTHighwaySegment.cpp" "EntityContainer.h" "EntityContainer.cpp"
  "NTStreet.h" "NTStreet.cpp" "Clipper.h" "Clipper.cpp" "JSONGenerator.h" "JSONGenerator.cpp"
  "TreeGenerator.h" "TreeGenerator.cpp" "LightingGenerator.h" "LightingGenerator.cpp")

else()
  
    message(STATUS "This program requires the CGAL library, and will not be compiled.")
  
endif()

