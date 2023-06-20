/**	\file	matex.h
 *	\brief	Main header file of the application.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <complex>
#include <string>
#include <math.h>
//#include <Eigen/Dense>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>
//#include <Eigen/Eigenvalues>

#include "configurationParameters.h"
#include "floorplan.h"
#include "rcModel.h"

using namespace std;
using namespace Eigen;



/**	\brief	Function that computes the steady state temperature for a given power vector.
 *
 *	Function that computes the steady state temperature for a given power vector.
 *
 *	\param[out] Tsteady					Vector in which to store the steady-state temperatures for all nodes.
 *	\param[in]	numberUnits				Number of units in the floorplan. Only units have power values, and the power in the rest of the nodes is 0.
 *	\param[in]	numberThermalNodes		Number of thermal nodes in the RC thermal network.
 *	\param[in]	Tamb					Ambient temperature in Kelvin.
 *	\param[in]	Power					Vector with the power values of all thermal nodes in the RC thermal network.
 *	\param[in]	Binv					Inverse of matrix B.
 *	\param[in]	Gamb					Vector G.
 */
void computeSteadyStateTemperatures(double* &Tsteady, const int &numberUnits, const int &numberNodesAmbient, const int &numberThermalNodes, const double &Tamb, double* &Power, double** &Binv, double* &Gamb);



/**	\brief	Function that prints a temperature vector to screen.
 *
 *	Function that prints a temperature vector to screen, according to the corresponding floorplan.
 *
 *	\param[in]	floorplan		Floorplan information with the names of the functional units.
 *	\param[in]	temperatures	Temperatures of each thermal node.
 */
void printTemperatures(Floorplan &floorplan, double* &temperatures);



/**	\brief	Function that dumps a temperature vector to a file.
 *
 *	Function that dumps a temperature vector to a file, according to the corresponding floorplan.
 *
 *	\pagam[in]	fileName		Name of the file inwhich to dump the temperature vector.
 *	\param[in]	floorplan		Floorplan information with the names of the functional units.
 *	\param[in]	temperatures	Temperatures of each thermal node.
 */
void dumpTemperaturesToFile(const string &fileName, Floorplan &floorplan, double* &temperatures);


/**	\brief	Function that saves the eigenvalues and eigenvectors (and some other information) to a binary file for fast computation.
 *
 *	Function that saves the eigenvalues and eigenvectors to a binary file for fast computation.
 *
 *	\pagam[in]	fileName				Name of the file inwhich to dump the temperature vector.
 *	\param[in]	rcModel					RC thermal network.
 *	\param[in]	floorplan				Floorplan information with the names of the functional units.
 *	\param[in]	eigenValues				Vector with eigenvalues.
 *	\param[in]	eigenVectors			Matrix with eigenvectors.
 *	\param[in]	eigenVectorsInv			Inverse of matrix with eigenvectors.
 *	\return								true if the file was successfully saved.
 */
bool saveEigenvaluesEigenvectorsToFile(const string &fileName, const RCmodel &rcModel, Floorplan &floorplan, double* &eigenValues, double** &eigenVectors, double** &eigenVectorsInv);


/**	\brief	Function that reads the eigenvalues and eigenvectors (and some other information) from a binary file for fast computation.
 *
 *	Function that reads the eigenvalues and eigenvectors (and some other information) from a binary file for fast computation.
 *
 *	\pagam[in]	fileName				Name of the file inwhich to dump the temperature vector.
 *	\param[out]	floorplan				Floorplan information with the names of the functional units.
 *	\param[out]	eigenValues				Vector with eigenvalues.
 *	\param[out]	eigenVectors			Matrix with eigenvectors.
 *	\param[out]	eigenVectorsInv			Inverse of matrix with eigenvectors.
 *	\return								True if the file was successfully read and consistent.
 */
bool readEigenvaluesEigenvectorsFromFile(const string &fileName, int &numberUnits, int &numberNodesAmbient, int &numberThermalNodes, Floorplan &floorplan, double** &Binv, double* &Gamb, double* &eigenValues, double** &eigenVectors, double** &eigenVectorsInv);


/**	\brief	Function that prints a temperature vector to screen.
 *
 *	Function that prints a temperature vector to screen, according to the corresponding floorplan. If the function returns a vector, then the initial temperature file was valid.
 *
 *	\param[out]	Tinit						Temperature vector read from the file.
 *	\param[in]	configuration				Configuration parameters.
 *	\param[in]	floorplan					Floorplan information with the names of the functional units.
 */
void readTemperaturesInitFile(double* &Tinit, const ConfigurationParameters &configuration, Floorplan &floorplan);





