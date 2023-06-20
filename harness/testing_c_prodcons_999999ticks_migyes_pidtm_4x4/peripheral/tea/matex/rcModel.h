/**	\file	rcModel.h
 *	\brief	Header file for RC thermal network class.
 */
#ifndef RCMODEL_H_
#define RCMODEL_H_

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
#include <float.h>

#include "floorplan.h"
#include "configurationParameters.h"

using namespace std;
using namespace Eigen;







/**	\brief	Model specific constant: fitting factor to match floworks (due to lumping). Changed from 1/2 to 1/3 due to the difference from traditional Elmore Delay scenario.
 */
#define C_FACTOR	0.333


/*	Typical thermal properties for secondary path layers	*/
#define RHO_METAL	0.0025
#define RHO_DIELECTRIC	1.0
#define RHO_C4	0.8
#define RHO_UNDERFILL 0.03
#define	RHO_SUB	0.5
#define RHO_SOLDER	0.06
#define RHO_PCB	0.333
#define K_METAL	(1.0/RHO_METAL)
#define K_DIELECTRIC (1.0/RHO_DIELECTRIC)
#define K_C4	(1.0/RHO_C4)
#define K_UNDERFILL (1.0/RHO_UNDERFILL)
#define K_SUB	(1.0/RHO_SUB)
#define K_SOLDER	(1.0/RHO_SOLDER)
#define K_PCB	(1.0/RHO_PCB)
//FIXME! the following values need to be found
#define SPEC_HEAT_METAL	3.55e6					/**< \brief	Specfic heat of silicon in [Joule/(meters^3 * Kelvin].					*/
#define SPEC_HEAT_DIELECTRIC	2.2e6
#define SPEC_HEAT_C4	1.65e6
#define SPEC_HEAT_UNDERFILL	2.65e6
#define SPEC_HEAT_SUB	1.6e6					/**< \brief	Specific heat of copper in [Joule/(meters^3 * Kelvin].					*/
#define SPEC_HEAT_SOLDER	2.1e6				/**< \brief	Specific heat of the interface material in [Joule/(meters^3 * Kelvin].	*/
#define SPEC_HEAT_PCB	1.32e6




/* grid model differs from the block model in its use of
 * a mesh of cells whose resolution is configurable. unlike
 * the block model, it can also model a stacked 3-D chip,
 * with each layer having a different floorplan. information
 * about the floorplan and the properties of each layer is
 * input in the form of a layer configuration file.
 */

/*  */
#define LCF_NPARAMS			7	/**< \brief	Layer configuration file constant: Number of parameters per layer.	*/
#define LCF_SNO				0	/**< \brief	Layer configuration file constant: Serial number.					*/
#define LCF_LATERAL			1	/**< \brief	Layer configuration file constant: Has lateral heat flow?			*/
#define LCF_POWER			2	/**< \brief	Layer configuration file constant: Dissipates power?				*/
#define LCF_SP				3	/**< \brief	Layer configuration file constant: Specific heat capacity.			*/
#define LCF_RHO				4	/**< \brief	Layer configuration file constant: Resistivity.						*/
#define LCF_THICK			5	/**< \brief	Layer configuration file constant: Thickness.						*/
#define LCF_FLP				6	/**< \brief	Layer configuration file constant: Floorplan file.					*/



#define DEFAULT_CHIP_LAYERS	2	/**< \brief	Default number of chip layers (excluding spreader and sink). Used when LCF file is not specified.	*/
#define LAYER_SI			0
#define LAYER_INT			1

#define SEC_CHIP_LAYERS	2		/**< \brief	Layers of secondary path with same area as die.		*/
#define LAYER_METAL 0
#define LAYER_C4	1

#define DEFAULT_PACK_LAYERS	2	/**< \brief	Default number of package layers.					*/
#define LAYER_SP			0
#define LAYER_SINK			1

#define SEC_PACK_LAYERS	3		/**< \brief	Additional package layers from secondary path.		*/
#define LAYER_SUB	0
#define LAYER_SOLDER	1
#define LAYER_PCB	2





/**	\brief	Class that stores the package parameters.
 *
 *	Class that stores the package parameters.
 */
class Package{
public:
	double r_sp1_x;								/**< \brief	Lateral resistance: Peripheral spreader nodes.		*/
	double r_sp1_y;								/**< \brief	Lateral resistance: Peripheral spreader nodes.		*/
	double r_hs1_x;								/**< \brief	Lateral resistance: Sink's inner periphery.			*/
	double r_hs1_y;								/**< \brief	Lateral resistance: Sink's inner periphery.			*/
	double r_hs2_x;								/**< \brief	Lateral resistance: Sink's inner periphery.			*/
	double r_hs2_y;								/**< \brief	Lateral resistance: Sink's inner periphery.			*/
	double r_hs;								/**< \brief	Lateral resistance: Sink's outer periphery.			*/

	double r_sp_per_x;							/**< \brief	Vertical resistance: Peripheral spreader nodes.		*/
	double r_sp_per_y;							/**< \brief	Vertical resistance: Peripheral spreader nodes.		*/
	double r_hs_c_per_x;						/**< \brief	Vertical resistance: Sink's inner periphery.		*/
	double r_hs_c_per_y;						/**< \brief	Vertical resistance: Sink's inner periphery.		*/
	double r_hs_per;							/**< \brief	Vertical resistance: Sink's outer periphery.		*/

	double c_sp_per_x;							/**< \brief	Vertical capacitance: Peripheral spreader nodes.	*/
	double c_sp_per_y;							/**< \brief	Vertical capacitance: Peripheral spreader nodes.	*/
	double c_hs_c_per_x;						/**< \brief	Vertical capacitance: Sink's inner periphery.		*/
	double c_hs_c_per_y;						/**< \brief	Vertical capacitance: Sink's inner periphery.		*/
	double c_hs_per;							/**< \brief	Vertical capacitance: Sink's outer periphery.		*/

	double r_amb_c_per_x;						/**< \brief	Vertical resistance to ambient: Sink's inner periphery.		*/
	double c_amb_c_per_x;						/**< \brief	Vertical capacitance to ambient: Sink's inner periphery.	*/
	double r_amb_c_per_y;						/**< \brief	Vertical resistance to ambient: Sink's inner periphery.		*/
	double c_amb_c_per_y;						/**< \brief	Vertical capacitance to ambient: Sink's inner periphery.	*/
	double r_amb_per;							/**< \brief	Vertical resistance to ambient: Sink's outer periphery.		*/
	double c_amb_per;							/**< \brief	Vertical capacitance to ambient: Sink's outer periphery.	*/

	double r_sub1_x;							/**< \brief	Secondary path lateral resistance: Peripheral package substrate nodes.		*/
	double r_sub1_y;							/**< \brief	Secondary path lateral resistance: Peripheral package substrate nodes.		*/
	double r_solder1_x;							/**< \brief	Secondary path lateral resistance: Peripheral solder ball nodes.			*/
	double r_solder1_y;							/**< \brief	Secondary path lateral resistance: Peripheral solder ball nodes.			*/
	double r_pcb1_x;							/**< \brief	Secondary path lateral resistance: PCB's inner periphery.					*/
	double r_pcb1_y;							/**< \brief	Secondary path lateral resistance: PCB's inner periphery.					*/
	double r_pcb2_x;							/**< \brief	Secondary path lateral resistance: PCB's inner periphery.					*/
	double r_pcb2_y;							/**< \brief	Secondary path lateral resistance: PCB's inner periphery.					*/
	double r_pcb;								/**< \brief	Secondary path lateral resistance: PCB's outer periphery.					*/


	double r_sub_per_x;							/**< \brief	Secondary path vertical resistance: Peripheral package substrate nodes.		*/
	double r_sub_per_y;							/**< \brief	Secondary path vertical resistance: Peripheral package substrate nodes.		*/
	double r_solder_per_x;						/**< \brief	Secondary path vertical resistance: Peripheral solder ball nodes.			*/
	double r_solder_per_y;						/**< \brief	Secondary path vertical resistance: Peripheral solder ball nodes.			*/
	double r_pcb_c_per_x;						/**< \brief	Secondary path vertical resistance: PCB's inner periphery.					*/
	double r_pcb_c_per_y;						/**< \brief	Secondary path vertical resistance: PCB's inner periphery.					*/
	double r_pcb_per;							/**< \brief	Secondary path vertical resistance: PCB's outer periphery.					*/

	double c_sub_per_x;							/**< \brief	Secondary path vertical capacitances: Peripheral package substrate nodes.	*/
	double c_sub_per_y;							/**< \brief	Secondary path vertical capacitances: Peripheral package substrate nodes.	*/
	double c_solder_per_x;						/**< \brief	Secondary path vertical capacitances: Peripheral solder ball nodes.			*/
	double c_solder_per_y;						/**< \brief	Secondary path vertical capacitances: Peripheral solder ball nodes.			*/
	double c_pcb_c_per_x;						/**< \brief	Secondary path vertical capacitances: PCB's inner periphery.				*/
	double c_pcb_c_per_y;						/**< \brief	Secondary path vertical capacitances: PCB's inner periphery.				*/
	double c_pcb_per;							/**< \brief	Secondary path vertical capacitances: PCB's outer periphery.				*/

	double r_amb_sec_c_per_x;					/**< \brief	Vertical resistance to ambient at PCB: PCB's inner periphery.		*/
	double c_amb_sec_c_per_x;					/**< \brief	Vertical capacitances to ambient at PCB: PCB's inner periphery.		*/
	double r_amb_sec_c_per_y;					/**< \brief	Vertical resistance to ambient at PCB: PCB's inner periphery.		*/
	double c_amb_sec_c_per_y;					/**< \brief	Vertical capacitances to ambient at PCB: PCB's inner periphery.		*/
	double r_amb_sec_per;						/**< \brief	Vertical resistance to ambient at PCB: PCB's outer periphery.		*/
	double c_amb_sec_per;						/**< \brief	Vertical capacitances to ambient at PCB: PCB's outer periphery.		*/


	/**	\brief	Method that loads all the R variables according to the configuration and floorplan size.
	 *
	 *	\param[in]	configuration		Configuration parameters.
	 *	\param[in]	floorplanWidth		Width of the floorplan.
	 *	\param[in]	floorplanHeight		Height of the floorplan.
	 */
	void loadR(const ConfigurationParameters &configuration, const double &floorplanWidth, const double &floorplanHeight);


	/**	\brief	Method that loads all the C variables according to the configuration and floorplan size.
	 *
	 *	\param[in]	configuration		Configuration parameters.
	 *	\param[in]	floorplanWidth		Width of the floorplan.
	 *	\param[in]	floorplanHeight		Height of the floorplan.
	 */
	void loadC(const ConfigurationParameters &configuration, const double &floorplanWidth, const double &floorplanHeight);





private:
	/**	\brief	Compute the thermal resistance.
	 *
	 */
	inline double getThermalResistance(const double &conductivity, const double &thickness, const double &area)
	{
		return thickness / (conductivity * area);
	}


	/**	\brief	Compute the thermal capacitance.
	 *
	 */
	inline double getThermalCapacitance(const double &sp_heat, const double &thickness, const double &area)
	{
		/* Include lumped vs. distributed correction	*/
		return C_FACTOR * sp_heat * thickness * area;
	}
};






/**	\brief	Class that stores a list of blocks mapped to a grid cell.
 *
 *	Class that stores a list of blocks mapped to a grid cell.
 */
class BlockList{
public:

	/**	\brief	Block to grid mapping data structure.
	 */
	struct BlockListElement{
		int blockIndex;			/**< \brief	Index of the mapped block.		*/
		double occupancy;		/**< \brief	Ratio of this block's area within the grid cell to the total area of the grid cell.	*/
	};


	vector <BlockListElement> blocks;	/**< \brief	List of blocks mapped to a grid cell.	*/


	inline void append(const int &newBlockIndex, const double &newOccupany)
	{
		BlockListElement newElement;
		newElement.blockIndex = newBlockIndex;
		newElement.occupancy = newOccupany;

		blocks.push_back(newElement);
	}
};



/**	\brief	Class that stores a the start and end indices of grid cells in a block (both in the x and y directions).
 *
 *	Class that stores a the start and end indices of grid cells in a block (both in the x and y directions).
 */
class GridList{
public:

	int i1;					/**< \brief	Start index in the y direction.		*/
	int i2;					/**< \brief	End index in the y direction + 1.	*/
	int j1;					/**< \brief	Start index in the x direction.		*/
	int j2;					/**< \brief	End index in the x direction + 1.	*/
};



/**	\brief	Class that stores the layer information for the grid model.
 *
 *	Class that stores the layer information for the grid model.
 */
class LayerInformation{
public:

	bool has_lateral;					/**< \brief	Configuration parameter: Model lateral spreading of heat?		*/
	bool has_power;						/**< \brief	Configuration parameter: Dissipates power?						*/
	double k;							/**< \brief	Configuration parameter: Thermal conductivity.					*/
	double k1;							/**< \brief	Configuration parameter: Thermal conductivity of the other material in some layers, such as C4/underfill.	*/
	double thickness;					/**< \brief	Configuration parameter: Thickness.								*/
	double sp;							/**< \brief	Configuration parameter: Specific heat capacity.				*/
	double sp1;							/**< \brief	Configuration parameter: Specific heat of the other material in some layers, such as C4/underfill.			*/

	double rx;							/**< \brief	Extracted information: x resistor.		*/
	double ry;							/**< \brief	Extracted information: y resistor.		*/
	double rz;							/**< \brief	Extracted information: z resistor.		*/
	double rx1;							/**< \brief	Extracted information: Resistor of the other material in some layers, e.g. C4/underfill.				*/
	double ry1;							/**< \brief	Extracted information: Resistor of the other material in some layers, e.g. C4/underfill.				*/
	double rz1;							/**< \brief	Extracted information: Resistor of the other material in some layers, e.g. C4/underfill.				*/
	double c;							/**< \brief	Extracted information: Capacitance.		*/
	double c1;							/**< \brief	Extracted information: Capacitance.		*/

	Floorplan layerFloorplan;			/**< \brief	Floorplan of the layer.					*/

	Array<BlockList,Dynamic,Dynamic> b2gmap;		/**<\brief	Block-grid map: Every element in the array holds the information of which blocks are mapped to the corresponding grid cell.	*/

	Array<GridList,Dynamic,1> g2bmap;		/**<\brief	Grid-block map: Every element in the array holds the information of which blocks are mapped to the corresponding grid cell.	*/
};













/**	\brief	Class that stores the RC thermal network.
 *
 *	Class that stores the RC thermal network.
 */
class RCmodel{
public:
	/**	\brief	Matrix B.
	 */
	Matrix<double, Dynamic, Dynamic> B;


	/**	\brief	Inverse of matrix B.
	 */
	Matrix<double, Dynamic, Dynamic> Binv;

	/**	\brief	Vector A (because A is a diagonal matrix).
	 */
	Matrix<double, Dynamic, 1> A;


	/**	\brief	Inverse of vector A (because A is a diagonal matrix).
	 */
	Matrix<double, Dynamic, 1> Ainv;


	/**	\brief	Matrix C.
	 */
	Matrix<double, Dynamic, Dynamic> C;


	/**	\brief	Vector G.
	 */
	Matrix<double, Dynamic, 1> G;


	/**	\brief	Vector Gamb. Is a partial copy of vector G.
	 */
	Matrix<double, Dynamic, 1> Gamb;



	/**	\brief	Total number of functional units.
	 */
	int numberUnits;


	/**	\brief	Total number of thermal nodes in contact with the ambient temperature.
	 */
	int numberNodesAmbient;


	/**	\brief	Total number of thermal nodes.
	 */
	int numberThermalNodes;


	/**	\brief	Total number of grid layers.
	 */
	int numberGridLayers;




	/**	\brief	Method that loads the RC thermal network from the information in the floorplan.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\param[in]	doTransient		If true, load the entire RC thermal network. If false, matrix C is not needed so it is not loaded.
	 *	\return						True if the RC thermal network was correctly loaded.
	 */
	bool load(const ConfigurationParameters &configuration, Floorplan &floorplan, const bool &doTransient);




	/**	\brief	Method that prints the floorplan information.
	 */
	void print(void);




	/**	\brief	Constructor. Initializes the variables.
	 *
	 */
	RCmodel();




private:

	/**	\brief	Method that loads the RC thermal network for the steady-state computations from the information in the floorplan.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\return						True if the RC thermal network was correctly loaded.
	 */
	bool loadRblock(const ConfigurationParameters &configuration, Floorplan &floorplan);



	/**	\brief	Method that loads the RC thermal network for the transient computations from the information in the floorplan.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\return						True if the RC thermal network was correctly loaded.
	 */
	bool loadCblock(const ConfigurationParameters &configuration, Floorplan &floorplan);



	/**	\brief	Method that loads the RC thermal network for the steady-state computations from the information in the floorplan.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\return						True if the RC thermal network was correctly loaded.
	 */
	bool loadRgrid(const ConfigurationParameters &configuration, Floorplan &floorplan);



	/**	\brief	Method that loads the RC thermal network for the transient computations from the information in the floorplan.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\return						True if the RC thermal network was correctly loaded.
	 */
	bool loadCgrid(const ConfigurationParameters &configuration, Floorplan &floorplan);



	/**	\brief	Method that populates the grid layer information either from the default floorplan or from the layer configuration file.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 */
	void populateGridLayers(const ConfigurationParameters &configuration, Floorplan &floorplan);

	/**	\brief	Method that populates the default grid layer information.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 */
	void populateDefaultGridLayers(const ConfigurationParameters &configuration, Floorplan &floorplan);

	/**	\brief	Method that appends the layer information of the package.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 */
	void appendPackageLayers(const ConfigurationParameters &configuration);


	/**	\brief	Method that parses the layer configuration file.
	 *
	 *	\param[in]	fileName			Name of the layer file to parse.
	 */
	void parseLayerFile(const string &fileName);

	/**	\brief	Method that sets the block and grid mapping data of the layer.
	 *
	 *	\param[in]	configuration	Configuration parameters.
	 *	\param[in]	floorplan		Floorplan information.
	 *	\param[inout]	layer			Layer.
	 */
	void set_bgmap(const ConfigurationParameters &configuration, Floorplan &floorplan, LayerInformation &layer);

	/**	\brief	Compute the thermal resistance.
	 *
	 */
	inline double getThermalResistance(const double &conductivity, const double &thickness, const double &area)
	{
		return thickness / (conductivity * area);
	}


	/**	\brief	Compute the thermal capacitance.
	 *
	 */
	inline double getThermalCapacitance(const double &sp_heat, const double &thickness, const double &area)
	{
		/* Include lumped vs. distributed correction	*/
		return C_FACTOR * sp_heat * thickness * area;
	}

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

	/**	\brief	Returns the ceiling function of the value, unless it is really close to an integer, case in which returns the integer.
	 *
	 */
	inline int tolerant_ceil(double val)
	{
		double nearest = floor(val+0.5);
		/* Numbers close to integers	*/
		if (equalTo(val, nearest))
			return ((int) nearest);
		/* All others	*/
		else
			return ((int) ceil(val));
	}

	/**	\brief	Returns the floor function of the value, unless it is really close to an integer, case in which returns the integer.
	 *
	 */
	inline int tolerant_floor(double val)
	{
		double nearest = floor(val+0.5);
		if (equalTo(val, nearest))
			return ((int) nearest);
		else
			return ((int) floor(val));
	}



	Matrix<double, Dynamic, 1> Gx;							/**< \brief	Lumped conductances in x direction.	*/
	Matrix<double, Dynamic, 1> Gy;							/**< \brief	Lumped conductances in x direction.	*/
	Matrix<double, Dynamic, 1> GxInterface;				/**< \brief	Lateral conductances in the interface layer.	*/
	Matrix<double, Dynamic, 1> GyInterface;				/**< \brief	Lateral conductances in the interface layer.	*/
	Matrix<double, Dynamic, 1> GxSpreader;					/**< \brief	Lateral conductances in the spreader layer.	*/
	Matrix<double, Dynamic, 1> GySpreader;					/**< \brief	Lateral conductances in the spreader layer.	*/
	Matrix<double, Dynamic, 1> GxHeatSink;					/**< \brief	Lateral conductances in the heat sink layer.	*/
	Matrix<double, Dynamic, 1> GyHeatSink;					/**< \brief	Lateral conductances in the heat sink layer.	*/
	Matrix<double, Dynamic, Dynamic> LenghtSharedEdge;		/**< \brief Length of shared edge between functional units.	*/
	Matrix<double, Dynamic, Dynamic> conductances;			/**< \brief	Matrix with conductances: conductances(i,j) = conductance between thermal nodes i and j */

	Matrix<int, Dynamic, Dynamic> border;

	Package package;											/**< \brief	Package parameters.						*/
	vector<LayerInformation> gridLayers;							/**< \brief	Vector with the layer information for the grid model.	*/
};













#endif /* RCMODEL_H_ */
