#include <iostream>
#include "LGenerator.h"
#include "EntityContainer.h"
#include "JSONGenerator.h"
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "EntityContainer.h"


/*! @mainpage Road generation based on L-Systems
*
* @section intro_sec Introduction
*
* The road generator was created as part of a bachelor thesis at the TU-Graz.
* Its main goal was to create a visually appealing road system, which will later
* be used in combinations with shape grammars (procedural modeling) to generate a complete
* city from scratch.
*
* The system has to be able to generate a visually appealing city map, containing rivers as well as
* green-spaces and parking lots next to the lots for the city-blocks itself. The generated result
* is configurable by the user in an easy way, so that a lot of different street patterns
* can be computed without greater foreknowledge. This is done by providing the input files
* to the programm at creation (See "all_parm.txt" as well as "all_points.txt")
*
* To create unique looking maps with an easily understandable underlying model, L-Systems
* were used. The L-System used a linked-list of non-terminal symbols, which were imple-
* mented using different classes for each symbol. Instances of the classes will replace them-
* selves in the list, until every instance has terminated.  Non-terminal symbols are:
*
*	- NTStart
*	- NTHighway
*	- NTHighwaySegment
*	- NTStreet
*       
* Before creating any of the roads, rivers are created. The amount of rivers used in the map
* can be configured by the user. Rivers grow from one (random) side of the map to the other.
* After doing so, the algorithm first creates a set of highways, which serve as anchor points for
* the generation of smaller roads. During creation, two set of rules are used in order to create
* visually appealing networks, which can be split in global and local rules.
*
* @subsection Global
*
* The global rules only apply to the creation of highway segments. These major traffic aortae
* are used as branching point for smaller street segments. In order to gain some visual control
* over the network generation, the user is able to place n-start points, from which highway
* segments will start growing towards m-target points. Each start point is represented by one
* member of the NTHighway class, which will create m-HighwaySegments, growing towards
* every of the goal points (segments terminate at collision or if the goal is reached). Every time
* a highway segment replaces itself with a new segment, a direct segment towards the goal
* is calculated (fixed length). After doing so a random offset is added, in order to make the
* creation look less linear.
*
* @image html HighwayGen.png
*
* Another characteristic of the highways is, that they are able to create bridges spanning rivers.
* Before a highway segment grows towards the goal, it is first checked if any river would inter-
* fere with the new segment and if so where. The algorithm then tries to get the other riverbank
* by extruding the conflict segment. If the two points could be acquired, a bridge can be con-
* structed between them. The old segment is only produced up to the first conflict point, while
* on the other side of the river a new highway-segment is created and inserted into the list.
*
* After the last highway has finished (either by reaching the goal or by intersecting another highway segment), 
* branching of the streets starts. Not waiting for the highways to terminate
* might otherwise lead to streets getting in the way of new highways, which would stop them
* mid-creation.
*
* @subsection Local 
*
* Every time a highway segment grows outwards, it has a (user selected) possibility to create
* a new street branch (branching-probability). Streets grow towards random locations, only
* keeping the angle relative to the previous street segment in mind (do not want too steep
* angles, should look natural). If the new segment is calculated, a check for intersections is
* necessary (when found, terminate the segment at the collision).
*
* @subsection Lots
* 
* The road network keeps growing until every element in the list has terminated. The created
* segments are stored in corresponding containers. Until now only segments were created, but
* as the lot creation algorithm needs polygons to work with, they had to be calculated by using
* the given segments as well as @b CPSGs @b 2D-Arrangement functionality. 
*
* @image html StreetGenFin1.png
* 
* By doing so polygons
* were created without any margin between them for the streets. This problem was solved by
* using the @b CLIPPER-Library to shrink the given polygons evenly.
*
* <br>
* @section install_sec Installation
*
* @subsection windows Windows
*
*	- Download source from https://github.com/LGrege/RoadNetworkGenerator
*   - Install CMAKE from https://cmake.org/download/
*	- Follow the instructions under http://www.cgal.org/download/windows.html to install CGAL
*	- Use CMAKE to generate the Visual Studio project <br>
*     @image html Setup1.png
*	- Open the project in visual studio and build it using the release settings <br>
*     @image html Setup2.png
*   - After building is done, a main.exe file is created under the Release folder
*
* @section usage_sec Usage
*
* The previously created executable expects two files as input, which have to be placed in the same
* directory as the file. These files are a config file as well as a input file (both .txt).
*
* @subsection config_file Config
*
* The name of the config file has to be "all_parms.txt". Example config file:
*
* \code{.txt}
* DAY-1
* BP-87
* LP-33
* RC-1
* MS-8000
* DIM-600x800
* \endcode
* 
* The input values are:
*	- DAY -> Sets a flag in the output files later used for building creation (not relevant for road creation)
*   - BP -> Branch Probability, defines probability that street segment creates new branch. If higher, road network gets finer (lesser big lots)
*   - LP -> Lighting Probability, based on this parameter the amount of lights placed on the map can be manipulated
*   - RC -> River Count, Number of rivers to be created
*   - MS -> Max Segment, Maximum number of segments after which creation terminates. If dimension is increased, this value has to be increased also.
*   - DIM -> Dimension of the Map
*
* Inputs that are not set are set by default with the values shown in the above configuration.
* 
*
* @subsection input_file Input
*
* The name of the input file has to be "all_points.txt" and has to look as follows:
*
* \code{.txt}
* L-87-124
* R-312-115
* L-639-99
* R-45-435
* L-393-359
* R-704-357
* \endcode
*
* The "L"-Points define start points from which the road highways start to grow towards the "R"-Points (End points).
* An arbitrary amount of start and end points can be created, shaping the way the road network will look.
* 
* @section generated_output Output
*
* The resulting lots as well as rivers are exported to a generated folder, containing subfolders for the corresponding
* entities. Depending on the type of object exported, they are either JSON or .txt files (later needed for another project).
* 
* Also two postscript files are created in the root directory, displaying the road network itself ("out.ps") as well as the
* generated lot-polygons of the city blocks ("poly.ps")
*
* <br>
* @section ack_sec Acknowledgments
*
*	- Clipper library was used for shrinking the polygons http://www.angusj.com/delphi/clipper.php
*	- CGAL was used throughout the whole project http://www.cgal.org/
*
*/

using namespace boost;

// Forward declaration
void readInPoints();
void readInParms();

int main()
{
	readInPoints();
	readInParms();

	// Create a new generator and hand over the 
	// high density points of the road network to 
	// be created by using the interpreter (L-Systems).d
	LGenerator main_gen;
	main_gen.generateRoadNetwork("out.ps");

	// Now write the JSON-File needed by GML.
	// Will also create polygons from the previously
	// created line segments
	JSONGenerator::writeJSONOut("Lots/generated.json");
	
	return 0;
}


//---------------------------------------------------------------------------
void readInPoints() {
	string curr_line;
	ifstream infile;
	infile.open("all_points.txt");
	while (!infile.eof())
	{
		getline(infile, curr_line);
		char_separator<char> sep("-");
		tokenizer< char_separator<char> > tokens(curr_line, sep);

		string cmd;
		double x;
		double y;

		int count = 0;
		BOOST_FOREACH(const string& t, tokens) {
			switch (count) {
			case 0:
				cmd = t;
				break;
			case 1:
				y = stod(t);
				break;
			case 2:
				x = stod(t);
				break;
			}
			count++;
		}

		if (cmd.compare("L") == 0)
			EntityContainer::getInstance()->start_points_.push_back(Point_2(x, y));
		if(cmd.compare("R") == 0)
			EntityContainer::getInstance()->high_dens_points_.push_back(Point_2(x, y));
	}
	infile.close();
}

//---------------------------------------------------------------------------
void readInParms() {
	string curr_line;
	ifstream infile;
	infile.open("all_parms.txt");
	while (!infile.eof())
	{
		getline(infile, curr_line);
		char_separator<char> sep("-");
		tokenizer< char_separator<char> > tokens(curr_line, sep);

		string parm_type;
		unsigned parm_value;
		string dimension = "";

		int count = 0;
		BOOST_FOREACH(const string& t, tokens) {
			switch (count) {
			case 0:
				parm_type = t;
				break;
			case 1:
				if (t.find("x") == string::npos)
					parm_value = stoi(t);
				else
					dimension = t;
				break;
			}
			count++;
		}

		if (parm_type.compare("DAY") == 0)
			EntityContainer::getInstance()->DAY_MODE = parm_value;

		if (parm_type.compare("BP") == 0)
			EntityContainer::getInstance()->BRANCH_PROB = parm_value;

		if (parm_type.compare("LP") == 0)
			EntityContainer::getInstance()->LIGHT_PROB = parm_value;

		if (parm_type.compare("RC") == 0)
			EntityContainer::getInstance()->NUMB_RIVERS = parm_value;

		if (parm_type.compare("MS") == 0) {
			EntityContainer::getInstance()->MAX_SEG_COUNT = parm_value;
			EntityContainer::getInstance()->MAX_SEG_COUNT_INIT = parm_value;
		}

		if (parm_type.compare("DIM") == 0)
		{
			EntityContainer::getInstance()->MAX_X = stoi(dimension.substr(0, dimension.find("x")));
			EntityContainer::getInstance()->MAX_Y = stoi(dimension.substr(dimension.find("x") + 1, dimension.length() - 1));
		}
	}
	infile.close();
}