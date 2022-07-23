/**	\file	floorplan.h
 *	\brief	Header file for floorplan class.
 */
#ifndef FLOORPLAN_H_
#define FLOORPLAN_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;


/**	\brief	Number of functional block layers.	*/

#define NUMBER_BLOCK_LAYERS		4
/**	\brief	Silicon layer index.		*/
#define SILICON		0
/**	\brief	Interface layer index (silicon is always layer 0).		*/
#define IFACE	1
/**	\brief	Heat spreader layer index.	*/
#define HSP	2
/**	\brief	Heat sink layer index.		*/
#define HSINK 3
/**	\brief	Ambient nodes layer index.		*/
#define AMBIENT_NODES 4

/**	\brief	Number of extra nodes due to the model: 4 spreader nodes, 4 heat sink nodes under the spreader (center),
 *			4 peripheral heat sink nodes (north, south, east and west) and a separate node for the ambient.
 */
#define EXTRA_NODES				12
#define	SP_W		0						/**< \brief	Spreader nodes.		*/
#define	SP_E		1						/**< \brief	Spreader nodes.		*/
#define	SP_N		2						/**< \brief	Spreader nodes.		*/
#define	SP_S		3						/**< \brief	Spreader nodes.		*/
#define SINK_C_W	4						/**< \brief	Central sink nodes (directly under the spreader).		*/
#define SINK_C_E	5						/**< \brief	Central sink nodes (directly under the spreader).		*/
#define SINK_C_N	6						/**< \brief	Central sink nodes (directly under the spreader).		*/
#define SINK_C_S	7						/**< \brief	Central sink nodes (directly under the spreader).		*/
#define	SINK_W		8						/**< \brief	Peripheral sink nodes.		*/
#define	SINK_E		9						/**< \brief	Peripheral sink nodes.		*/
#define	SINK_N		10						/**< \brief	Peripheral sink nodes.		*/
#define	SINK_S		11						/**< \brief	Peripheral sink nodes.		*/

/**	\brief	Secondary extra nodes.		*/
#define EXTRA_SECONDARY_NODES		16
#define	SUB_W		12						/**< \brief	Package substrate nodes.		*/
#define	SUB_E		13						/**< \brief	Package substrate nodes.		*/
#define	SUB_N		14						/**< \brief	Package substrate nodes.		*/
#define	SUB_S		15						/**< \brief	Package substrate nodes.		*/
#define	SOLDER_W		16					/**< \brief	Solder ball nodes.		*/
#define	SOLDER_E		17					/**< \brief	Solder ball nodes.		*/
#define	SOLDER_N		18					/**< \brief	Solder ball nodes.		*/
#define	SOLDER_S		19					/**< \brief	Solder ball nodes.		*/
#define PCB_C_W	20							/**< \brief	Central PCB nodes (directly under the solder balls).		*/
#define PCB_C_E	21							/**< \brief	Central PCB nodes (directly under the solder balls).		*/
#define PCB_C_N	22							/**< \brief	Central PCB nodes (directly under the solder balls).		*/
#define PCB_C_S	23							/**< \brief	Central PCB nodes (directly under the solder balls).		*/
#define	PCB_W		24						/**< \brief	Peripheral PCB nodes.		*/
#define	PCB_E		25						/**< \brief	Peripheral PCB nodes.		*/
#define	PCB_N		26						/**< \brief	Peripheral PCB nodes.		*/
#define	PCB_S		27						/**< \brief	Peripheral PCB nodes.		*/


/**	\brief	Comparison margin to decide if two values are equal.		*/
#define DELTA_EQUAL			1.0e-6







/**	\brief	Class for a functional unit.
 *
 *	Class for a functional unit.
 */
class FunctionalUnit{
public:
	string name;							/**< \brief	Name of the functional unit.	*/
	double width;							/**< \brief	Width of the functional unit [meters].	*/
	double height;							/**< \brief	Height of the functional unit [meters].	*/
	double left;							/**< \brief	Left (x) coordinate of the functional unit [meters].	*/
	double bottom;							/**< \brief	Bottom (y) coordinate of the functional unit [meters].	*/


	/**	\brief	Method that parses the line and loads the functional unit from the information in the line, if the line is valid
	 *
	 *	\param[in]	inputLine		String with the line read from the file with the information of a functional unit.
	 *	\return						True if the line was successfully parsed.
	 */
	bool load(const string &inputLine);


	/**	\brief	Method that returns the area of the functional unit.
	 *
	 *	\return						Area of the functional unit.
	 */
	double getArea(void);


	/**	\brief	Method that returns whether the functional unit is valid or not.
	 *
	 *	\return						True if the area of the functional unit is larger than zero.
	 */
	bool isValid(void);



	/**	\brief	Constructor. Initializes the unit with area zero.
	 *
	 */
	FunctionalUnit();
};









/**	\brief	Class for the wire density between functional units.
 *
 *	Class for the wire density between functional units.
 */
class WireDensityRaw{
public:
	string nameFrom;						/**< \brief	Name of one of the functional units.	*/
	string nameTo;							/**< \brief	Name of one of the functional units.	*/
	double density;							/**< \brief	Wire density between functional unit.	*/


	/**	\brief	Method that parses the line and loads the wire density unit from the information in the line, if the line is valid
	 *
	 *	\param[in]	inputLine		String with the line read from the file with the information of the wire density between functional units.
	 *	\return						True if the line was successfully parsed.
	 */
	bool load(const string &inputLine);



	/**	\brief	Constructor. Initializes the unit with wire density zero.
	 *
	 */
	WireDensityRaw();
};












/**	\brief	Class that stores the floorplan.
 *
 *	Class that stores the floorplan.
 */
class Floorplan{
public:
	/**	\brief	Vector with the functional units.
	 */
	vector< FunctionalUnit > units;


	/**	\brief	Matrix with the wire densities between functional units.
	 */
	Matrix<double, Dynamic, Dynamic> wireDensities;


	/**	\brief	Method that reads the functional units from a file and loads this information into vector units.
	 *
	 *	\return						True if the floorplan was correctly read.
	 */
	bool readFloorplan(const string &floorplanFileName, bool const &readConnectivity);



	/**	\brief	Get the total width of the floorplan.
	 *
	 *	\return		Total width of the floorplan.
	 */
	double getWidth(void);



	/**	\brief	Get the total height of the floorplan.
	 *
	 *	\return		Total height of the floorplan.
	 */
	double getHeight(void);




	/**	\brief	Method that prints the floorplan information.
	 */
	void print(void);



	/**	\brief	Method that return the index of the block.
	 *
	 *	\param[in]	blockName			Name of the block.
	 *	\return							Index of the block. -1 if the block does not exist.
	 */
	int getBlockIndex(const string &blockName);



	/**	\brief	Method that returns the shared length between two functional units.
	 *
	 *	\return						Shared length between unitA and unitB.
	 */
	double getSharedLength(const int &unitA, const int &unitB);



	/**	\brief	Method that returns true if two units are horizontally adjacent.
	 *
	 *	\return						True if unitA is horizontally adjacent to unitB.
	 */
	bool isHorizontalAdjacent(const int &unitA, const int &unitB);



	/**	\brief	Method that returns true if two units are horizontally adjacent.
	 *
	 *	\return						True if unitA is horizontally adjacent to unitB.
	 */
	bool isVerticallyAdjacent(const int &unitA, const int &unitB);



	/**	\brief	Method that returns the number of functional units in the floorplan.
	 *
	 */
	inline unsigned int getNumberFunctionalUnits(void) { return units.size(); }



	/**	\brief	Method that returns the number of thermal nodes resulting from the floorplan.
	 *
	 */
	inline unsigned int getNumberThermalNodes(void) { return NUMBER_BLOCK_LAYERS*units.size() + EXTRA_NODES; }



private:
	/**	\brief	Method that translates the left-most and bottom-most coordinates of the entire floorplan.
	 *
	 *	\param[in]	newLeftFloorplan	New left-most coordinate.
	 *	\param[in]	newBottomFloorplan	New bottom-most coordinate.
	 */
	void translate(const double &newLeftFloorplan, const double &newBottomFloorplan);

	/**	\brief	Compares two values under a certain margin of error.
	 *
	 */
	inline bool equalTo(const double &x, const double &y)
	{
		return (fabs(x-y) < DELTA_EQUAL);
	}

	/**	\brief	Compares two values and return the minimum value.
	 *
	 */
	inline double minimum(const double &x, const double &y)
	{
		return ((x < y)? x:y);
	}

	/**	\brief	Compares two values and return the maximum value.
	 *
	 */
	inline double maximum(const double &x, const double &y)
	{
		return ((x > y)? x:y);
	}
};













#endif /* FLOORPLAN_H_ */
