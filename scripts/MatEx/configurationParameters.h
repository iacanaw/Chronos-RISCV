/**	\file	configurationParameters.h
 *	\brief	Header file for general configuration parameters.
 */
#ifndef CONFIGURATIONPARAMETERS_H_
#define CONFIGURATIONPARAMETERS_H_

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

using namespace std;



const double Tkelvin = 273.15;




/**	\brief	Class that stores the configuration parameters.
 *
 *	Class that stores the configuration parameters from the configuration files or the command line. The command line has priority.
 */
class ConfigurationParameters{
public:

	/**	\brief	Structre with the definition pair of strings that forms a configuration parameter.	*/
	struct Parameter {
		string name;										/**< \brief Name of the configuration parameter.	*/
		string value;										/**< \brief Value of the configuration parameter.	*/
	};


	/**	\brief	Vector containing all the configuration parameters.	*/
	vector< Parameter > parameters;



	/**	\brief	Method that parses the command line arguments.
	 *
	 *	\param[in]	argc	Number of command line arguments.
	 *	\param[in]	argv	Strings with the command line arguments.
	 */
	void parseCommandLine(int argc, char *argv[]);



	/**	\brief	Method that reads the configuration from the configuration file.
	 *
	 *	Method that reads the configuration from the configuration file. The configuration file must be passed as a command line argument.
	 */
	void readConfigurationFile(void);



	/**	\brief	Method that verifies that the configuration parameters are valid.
	 *
	 */
	bool verify(void);



	/**	\brief	Method that prints the configuration parameters.
	 */
	void print(void);



	/**	\brief	Method that saves the configuration parameters into a file.
	 */
	void dumpToFile(const string &fileName);




	/**	\brief	Enumeration with the accepted types of models.	*/
	typedef enum {
		BLOCK=1,
		GRID=2
	} ModelTypes;

	/**	\brief	Enumeration with the accepted grid map modes.	*/
	typedef enum {
		AVERAGE=1,
		MIN=2,
		MAX=3,
		CENTER=4
	} GridMapModes;

	/**	\brief	Enumeration with the accepted leakage modes.	*/
	typedef enum {
		USER_LEAKAGE=1,
		HOTSPOT_LEAKAGE=2
	} LeakageModes;



	string fileNameEigenvaluesEigenvectorsIn;				/**< \brief	Name of the input file with the eigenvalues and eigenvectors for the floorplan.	*/
	string fileNameEigenvaluesEigenvectorsOut;				/**< \brief	Name of the output file with the eigenvalues and eigenvectors for the floorplan.	*/
	string fileNameFloorplan;								/**< \brief	Name of the floorplan file.	*/
	string fileNamePowerTrace;								/**< \brief	Name of the power trace file.	*/
	string fileNameBlockSteadyOutputTemperatures;			/**< \brief	Name of the output file where the application stores the steady-state temperature results for the block model.	*/
	string fileNameBlockTransientOutputTemperatures;		/**< \brief	Name of the output file where the application stores the transient temperature results for the block model.	*/
	string fileNameGridSteadyOutputTemperatures;			/**< \brief Name of the output file where the application stores the steady-state temperature results for the grid model.	*/
	string fileNameGridTransientOutputTemperatures;			/**< \brief Name of the output file where the application stores the transient temperature results for the grid model.	*/
	string fileNameMaximumOutputTemperatures;				/**< \brief	Name of the output file where the application stores the peaks in temperature for every change in power in the power trace file.	*/
	string fileNameConfiguration;							/**< \brief	Name of the configuration file.	*/
	string fileNameDumpConfiguration;						/**< \brief	Name of the dump configuration file.	*/
	string fileNameInitialTemperatures;						/**< \brief Name of the file with the initial temperatures of the nodes. If no file is specified, the application uses initTemp.	*/
	string fileNamePackageConfig;							/**< \brief Name of the file with the package configuration.	*/
	string fileNameGridLayer;								/**< \brief Name of the layer file for the grid model.	*/
	ModelTypes modelType;									/**< \brief Model type: block or grid.	*/
	GridMapModes gridMapMode;								/**< \brief Grid to block mapping mode - (avg|min|max|center), i.e., a block's temperature is the avg, min or max of all the grid cells in it or equal to that of the grid cell in its center.	*/
	LeakageModes leakageMode;								/**< \brief Leakage mode (only valid when -leakage_used=1).	*/
	bool modelSecondary;									/**< \brief Option to model the secondary path.	*/
	bool dtmUsed;											/**< \brief Option to consider Dynamic Thermal Management (DTM).	*/
	bool leakageUsed;										/**< \brief Option to consider a temperature-leakage loop within the application.	*/
	bool packageModelUsed;									/**< \brief Option to use the detailed package model.	*/
	bool blockOmitLateral;									/**< \brief Option to omit lateral chip resistances.	*/
	int gridRows;											/**< \brief Number of rows to use in the grid model.	*/
	int gridCols;											/**< \brief Number of columns to use in the grid model.	*/
	double tChip;											/**< \brief Chip thickness in meters.	*/
	double kChip;											/**< \brief Silicon thermal conductivity [W/(meters * Kelvin)].	*/
	double pChip;											/**< \brief Silicon specific heat [Joule/(meters^3 * Kelvin)].	*/
	double thermalThreshold;								/**< \brief Temperature threshold for DTM [Kelvin].	*/
	double cConvec;											/**< \brief Convection capacitance [Joule/Kelvin].	*/
	double rConvec;											/**< \brief Convection resistance [Kelvin/Watt].	*/
	double sSink;											/**< \brief Heatsink side [meters].	*/
	double tSink;											/**< \brief Heatsink thickness [meters].	*/
	double kSink;											/**< \brief Heatsink thermal conductivity [Watts/(meters * Kelvin].	*/
	double pSink;											/**< \brief Heatsink specific heat [Joule/(meters^3 * Kelvin].	*/
	double sSpreader;										/**< \brief Heat spreader side [meters].	*/
	double tSpreader;										/**< \brief Heat spreader thickness [meters].	*/
	double kSpreader;										/**< \brief Heat spreader thermal conductivity [Watts/(meters * Kelvin].	*/
	double pSpreader;										/**< \brief Heat spreader specific heat [Joule/(meters^3 * Kelvin].	*/
	double tInterface;										/**< \brief Interface material thickness [meters].	*/
	double kInterface;										/**< \brief Interface material thermal conductivity [Watts/(meters * Kelvin].	*/
	double pInterface;										/**< \brief Interface material specific heat [Joule/(meters^3 * Kelvin].	*/
	double rConvecSec;										/**< \brief Convection resistance at the air/PCB interface [Kelvin/Watt].	*/
	double cConvecSec;										/**< \brief Convection capacitance at the air/PCB interface [Jolue/Kelvin].	*/
	double nMetal;											/**< \brief Number of on-chip metal layers.	*/
	double tMetal;											/**< \brief One metal layer thickness [meters].	*/
	double tC4;												/**< \brief C4/underfill thickness [meters].	*/
	double sC4;												/**< \brief Side size of EACH C4 pad.	*/
	double nC4;												/**< \brief Number of C4 pads.	*/
	double sSub;											/**< \brief Package substrate side [meters].	*/
	double tSub;											/**< \brief Package substrate thickness [meters].	*/
	double sSolder;											/**< \brief Solder ball side [meters].	*/
	double tSolder;											/**< \brief Solder ball thickness [meters].	*/
	double sPcb;											/**< \brief PCB side [meters].	*/
	double tPcb;											/**< \brief PCB thickness [meters].	*/
	double Tamb;											/**< \brief Ambient temperature [Kelvin].	*/
	double initTemp;										/**< \brief Initial temperature [Kelvin], when not read from a file.	*/
	double sampligInterval;									/**< \brief Sampling interval for transient computation [seconds].	*/
	double baseProcFreq;									/**< \brief Base processor frequency [Hz].	*/
	int iterations;											/**< \brief Number of iterations used in Newton's method to compute the peaks in temperature.	*/



	/**	\brief	Constructor. Initializes all variables with the default values.	*/
	ConfigurationParameters();

private:
	/**	\brief	Method that checks whether the value of the new parameter is valid.
	 *
	 *	\param[in]	newParameter		Name and value of the new parameter.
	 *	\param[in]	newHasPriority		When the parameter name is repeated and variable is true, the new value replaces the old one. When this variable is false, the old value is preserved.
	 *
	 *	\return							True if the name is valid, false otherwise.
	 */
	bool addNewParameter(const Parameter &newParameter, const bool &newHasPriority);



	/**	\brief	Return true if the string containing the file name is "(null)", "null", "(Null)", "Null", "(NULL)", or "NULL"	*/
	inline bool nullFileName(const string &fileName) { return ((fileName == "(null)") || (fileName == "null") || (fileName == "(Null)") || (fileName == "Null") || (fileName == "(NULL)") || (fileName == "NULL")); }
};













#endif /* CONFIGURATIONPARAMETERS_H_ */
