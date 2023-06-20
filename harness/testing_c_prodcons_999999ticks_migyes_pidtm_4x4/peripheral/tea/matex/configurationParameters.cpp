/**	\file	configurationParameters.cpp
 *	\brief	Source file for general configuration parameters.
 */

#include "configurationParameters.h"




ConfigurationParameters::ConfigurationParameters()
{
	/* Set defaults values for all configuration parameters	*/
	/*fileNameFloorplan = "";
	fileNamePowerTrace = "";
	fileNameBlockSteadyOutputTemperatures = "";
	fileNameBlockTransientOutputTemperatures = "";
	fileNameGridSteadyOutputTemperatures = "";
	fileNameGridTransientOutputTemperatures = "";
	fileNameMaximumOutputTemperatures = "";
	fileNameConfiguration = "";
	fileNameDumpConfiguration = "";
	fileNameInitialTemperatures = "";
	fileNamePackageConfig = "";
	fileNameGridLayer = "";*/

	/* Chip specs	*/
	tChip= 0.15e-3;
	kChip = 100.0;
	pChip = 1.75e6;
	thermalThreshold = 81.8 + Tkelvin;

	/* Heat sink specs	*/
	cConvec = 140.4;
	rConvec = 0.1;
	sSink = 60e-3;
	tSink = 6.9e-3;
	kSink = 400.0;
	pSink = 3.55e6;

	/* Heat spreader specs	*/
	sSpreader = 30e-3;
	tSpreader = 1e-3;
	kSpreader = 400.0;
	pSpreader = 3.55e6;

	/* Interface material specs	*/
	tInterface = 20e-6;
	kInterface = 4.0;
	pInterface = 4.0e6;

	/* Secondary heat transfer path */
	modelSecondary = false;
	rConvecSec = 1.0;
	cConvecSec = 140.4;						/**< \TODO Needs an updated value. */
	nMetal = 8;
	tMetal = 10.0e-6;
	tC4 = 0.0001;
	sC4 = 20.0e-6;
	nC4 = 400;
	sSub = 0.021;
	tSub = 0.001;
	sSolder = 0.021;
	tSolder = 0.00094;
	sPcb = 0.1;
	tPcb = 0.002;

	/* Others	*/
	Tamb = 45 + Tkelvin;
	initTemp = 60 + Tkelvin;
	sampligInterval = ((double)1)/((double)300000);
	baseProcFreq = 3e9;
	dtmUsed = false;

	leakageUsed = false;
	leakageMode = USER_LEAKAGE;

	packageModelUsed = false;

	modelType = BLOCK;

	blockOmitLateral = false;

	gridRows = 64;
	gridCols = 64;

	gridMapMode = CENTER;

	iterations = 20;
}




void ConfigurationParameters::parseCommandLine(int argc, char *argv[])
{
	// Iterate through the command line
	int i = 1;
	while(i < argc){
		// Depending on the position of the string, it should be a parameter name or a value. Since we start with i=1, it has to be a name.
		// A parameter name, it should start with '-'
		if(argv[i][0] == '-'){
			Parameter newParameter;
			newParameter.name = &(argv[i][1]);
			i++;

			// Check if the new parameter has a value, or if there is no value after the parameter name
			if(i < argc){
				newParameter.value = argv[i];
				i++;

				// If valid, add the configuration parameter to the list
				if(addNewParameter(newParameter, true) == false){
					exit(1);
				}
			}
			else{
				cout << "Error: Invalid command line. Parameter \"" << newParameter.name << "\" has no value. Please check usage." << endl;
				exit(1);
			}
		}
		else{
			cout << "Error: Invalid command line. Please check usage." << endl;
			exit(1);
		}
	}




	// The application needs at least a floorplan file (or a eigendata file) and a power trace file. If this was not passed in the command line, then the application cannot continue
	if((fileNameFloorplan.size() == 0) && (fileNameEigenvaluesEigenvectorsIn.size() == 0)){
		cout << "Error: Required floorplan or eigendata file missing." << endl;
		exit(1);
	}
	if(fileNamePowerTrace.size() == 0){
		cout << "Error: Required power trace file missing." << endl;
		exit(1);
	}
}




void ConfigurationParameters::readConfigurationFile(void)
{
	if(fileNameConfiguration.size() > 0){
		ifstream inputConfigFile(fileNameConfiguration.c_str());
		if ( inputConfigFile.is_open() ){
			if(inputConfigFile.good()){
				// Read until the end of the file
				while(inputConfigFile.good()){
					// Read one line from the file
					string line;
					getline(inputConfigFile, line);

					// If the line was successfully read
					if(inputConfigFile.good()){
						stringstream lineStream(line);
						Parameter newParameter;
						lineStream >> newParameter.name;

						// If the line had a parameter
						if(newParameter.name.size() > 0){
							if(newParameter.name[0] == '-'){
								newParameter.name.erase(0,1);
								lineStream >> newParameter.value;

								// If valid, add the configuration parameter to the list
								if(addNewParameter(newParameter, false) == false){
									inputConfigFile.close();
									exit(1);
								}
							}
						}
					}
				}
			}
			else{
				cout << "Error: Configuration file could not be open for reading." << endl;
				inputConfigFile.close();
				exit(1);
			}
			inputConfigFile.close();
		}
		else{
			cout << "Error: Configuration file could not be open for reading." << endl;
			exit(1);
		}
	}
}




bool ConfigurationParameters::verify(void)
{
	if(modelType == GRID){
		if(gridRows < 1){
			cout << "Error: Using the grid model with an invalid number of rows." << endl;
			return false;
		}

		if(gridCols < 1){
			cout << "Error: Using the grid model with an invalid number of columns." << endl;
			return false;
		}
	}

	if(dtmUsed){
		if(Tamb > thermalThreshold){
			cout << "Error: When using DTM, the ambient temperature cannot exceed the threshold temperature." << endl;
			return false;
		}

		if(thermalThreshold < 0){
			cout << "Error: The thermal threshold cannot be a negative value." << endl;
			return false;
		}
	}

	if(Tamb < 0){
		cout << "Error: The ambient temperature cannot be a negative value." << endl;
		return false;
	}

	if(iterations <= 0){
		cout << "Error: The number of iterations for Newton's method has to be a positive integer." << endl;
		return false;
	}

	return true;
}






void ConfigurationParameters::print(void)
{
	cout << "#Configuration Parameters:" << endl;
	if(parameters.size() == 0){
		cout << "\tNo configuration parameters" << endl;
		return;
	}
	for(unsigned int i = 0; i < parameters.size(); i++){
		cout << "\t" << parameters[i].name << ":\t" << parameters[i].value << endl;
	}
}



void ConfigurationParameters::dumpToFile(const string &fileName)
{
	if(fileName.size() > 0){
		ofstream dumpFile;
		dumpFile.open (fileName.c_str());
		if (dumpFile.is_open()){
			for(unsigned int i = 0; i < parameters.size(); i++){
				if(	(parameters[i].name != "p") &&
					(parameters[i].name != "c") &&
					(parameters[i].name != "d") &&
					(parameters[i].name != "o") &&
					(parameters[i].name != "f")){
					dumpFile << "\t-" << parameters[i].name << "\t\t" << parameters[i].value << endl;
					if(dumpFile.good() == false){
						cout << "Error: There was an error writing to the configuration dump file: \"" << fileName << "\"." << endl;
						dumpFile.close();
						exit(1);
					}
				}
			}
			dumpFile.close();
		}
		else{
			cout << "Error: Configuration dump file: \"" << fileName << "\" could not be open for writing." << endl;
			exit(1);
		}
	}
}



bool ConfigurationParameters::addNewParameter(const Parameter &newParameter, const bool &newHasPriority)
{
	if(newParameter.value.size() > 0){
		// Check if the parameter is repeated.
		int repeatedIndex = -1;
		for(unsigned int existingParameter = 0; existingParameter < parameters.size(); existingParameter++){
			if(newParameter.name == parameters[existingParameter].name)
				repeatedIndex = existingParameter;
		}

		// If it is not repeated or the new one has priority, then we add the value to the configuration if it matches the type.
		if((repeatedIndex < 0) || newHasPriority){

			// Start with the configuration parameters that are names of files
			if(newParameter.name == "i"){
				if(nullFileName(newParameter.value))
					fileNameEigenvaluesEigenvectorsIn = "";
				else
					fileNameEigenvaluesEigenvectorsIn = newParameter.value;
			}
			else if(newParameter.name == "f"){
				if(nullFileName(newParameter.value))
					fileNameFloorplan = "";
				else
					fileNameFloorplan = newParameter.value;
			}
			else if(newParameter.name == "p"){
				if(nullFileName(newParameter.value))
					fileNamePowerTrace = "";
				else
					fileNamePowerTrace = newParameter.value;
			}
			else if(newParameter.name == "c"){
				if(nullFileName(newParameter.value))
					fileNameConfiguration = "";
				else
					fileNameConfiguration = newParameter.value;
			}
			else if(newParameter.name == "d"){
				if(nullFileName(newParameter.value))
					fileNameDumpConfiguration = "";
				else
					fileNameDumpConfiguration = newParameter.value;
			}
			else if(newParameter.name == "o"){
				if(nullFileName(newParameter.value))
					fileNameMaximumOutputTemperatures = "";
				else
					fileNameMaximumOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "steady_file_block"){
				if(nullFileName(newParameter.value))
					fileNameBlockSteadyOutputTemperatures = "";
				else
					fileNameBlockSteadyOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "steady_file_grid"){
				if(nullFileName(newParameter.value))
					fileNameGridSteadyOutputTemperatures = "";
				else
					fileNameGridSteadyOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "transient_file_block"){
				if(nullFileName(newParameter.value))
					fileNameBlockTransientOutputTemperatures = "";
				else
					fileNameBlockTransientOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "transient_file_grid"){
				if(nullFileName(newParameter.value))
					fileNameGridTransientOutputTemperatures = "";
				else
					fileNameGridTransientOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "eigen_out"){
				if(nullFileName(newParameter.value))
					fileNameEigenvaluesEigenvectorsOut = "";
				else
					fileNameEigenvaluesEigenvectorsOut = newParameter.value;
			}
			else if(newParameter.name == "init_file"){
				if(nullFileName(newParameter.value))
					fileNameInitialTemperatures = "";
				else
					fileNameInitialTemperatures = newParameter.value;
			}
			else if(newParameter.name == "package_config_file"){
				if(nullFileName(newParameter.value))
					fileNamePackageConfig = "";
				else
					fileNamePackageConfig = newParameter.value;
			}
			else if(newParameter.name == "grid_layer_file"){
				if(nullFileName(newParameter.value))
					fileNameGridLayer = "";
				else
					fileNameGridLayer = newParameter.value;
			}
			else if(newParameter.name == "grid_steady_file"){
				if(nullFileName(newParameter.value))
					fileNameGridSteadyOutputTemperatures = "";
				else
					fileNameGridSteadyOutputTemperatures = newParameter.value;
			}
			else if(newParameter.name == "grid_transient_file"){
				if(nullFileName(newParameter.value))
					fileNameGridTransientOutputTemperatures = "";
				else
					fileNameGridTransientOutputTemperatures = newParameter.value;
			}
			// Then we check the configuration parameters that are enumerations
			else if(newParameter.name == "model_type"){
				if(newParameter.value == "block"){
					modelType = BLOCK;
				}
				else if(newParameter.value == "grid"){
					modelType = GRID;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be 'block' or 'grid'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "grid_map_mode"){
				if((newParameter.value == "block") || (newParameter.value == "Block") || (newParameter.value == "BLOCK")){
					gridMapMode = AVERAGE;
				}
				else if((newParameter.value == "min") || (newParameter.value == "Min") || (newParameter.value == "MIN")){
					gridMapMode = MIN;
				}
				else if((newParameter.value == "max") || (newParameter.value == "Max") || (newParameter.value == "MAX")){
					gridMapMode = MAX;
				}
				else if((newParameter.value == "center") || (newParameter.value == "Center") || (newParameter.value == "CENTER")){
					gridMapMode = CENTER;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be 'avg', 'min', 'max', or 'center'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "leakage_mode"){
				if(newParameter.value == "0"){
					leakageMode = USER_LEAKAGE;
				}
				else if(newParameter.value == "1"){
					leakageMode = HOTSPOT_LEAKAGE;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			// Now we check for all the boolean variables
			else if(newParameter.name == "model_secondary"){
				if(newParameter.value == "0"){
					modelSecondary = false;
				}
				else if(newParameter.value == "1"){
					modelSecondary = true;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "dtm_used"){
				if(newParameter.value == "0"){
					dtmUsed = false;
				}
				else if(newParameter.value == "1"){
					dtmUsed = true;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "leakage_used"){
				if(newParameter.value == "0"){
					leakageUsed = false;
				}
				else if(newParameter.value == "1"){
					leakageUsed = true;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "package_model_used"){
				if(newParameter.value == "0"){
					packageModelUsed = false;
				}
				else if(newParameter.value == "1"){
					packageModelUsed = true;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			else if(newParameter.name == "block_omit_lateral"){
				if(newParameter.value == "0"){
					blockOmitLateral = false;
				}
				else if(newParameter.value == "1"){
					blockOmitLateral = true;
				}
				else{
					cout << "Error: Value of parameter \"" << newParameter.name << "\" should be '0' or '1'" << endl;
					return false;
				}
			}
			// Now check the variables of type int
			else if(newParameter.name == "grid_rows"){
				try{
					int iValue;
					stringstream streamValue(newParameter.value);
					streamValue.exceptions(stringstream::goodbit);
					streamValue >> iValue;
					if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
						gridRows = iValue;
					}
					else{
						cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
						return false;
					}
				}
				catch(...){
					cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
					return false;
				}
			}
			else if(newParameter.name == "grid_cols"){
				try{
					int iValue;
					stringstream streamValue(newParameter.value);
					streamValue.exceptions(stringstream::goodbit);
					streamValue >> iValue;
					if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
						gridCols = iValue;
					}
					else{
						cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
						return false;
					}
				}
				catch(...){
					cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
					return false;
				}
			}
			else if(newParameter.name == "iterations"){
				try{
					int iValue;
					stringstream streamValue(newParameter.value);
					streamValue.exceptions(stringstream::goodbit);
					streamValue >> iValue;
					if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
						iterations = iValue;
					}
					else{
						cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
						return false;
					}
				}
				catch(...){
					cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
					return false;
				}
			}
			// Finally, we check all the variables of type double
			else{
				try{
					double dValue;
					stringstream streamValue(newParameter.value);
					streamValue.exceptions(stringstream::goodbit);
					streamValue >> dValue;
					if(newParameter.name == "t_chip"){
						tChip = dValue;
					}
					else if(newParameter.name == "k_chip"){
						kChip = dValue;
					}
					else if(newParameter.name == "p_chip"){
						pChip = dValue;
					}
					else if(newParameter.name == "thermal_threshold"){
						thermalThreshold = dValue;
					}
					else if(newParameter.name == "c_convec"){
						cConvec = dValue;
					}
					else if(newParameter.name == "r_convec"){
						rConvec = dValue;
					}
					else if(newParameter.name == "s_sink"){
						sSink = dValue;
					}
					else if(newParameter.name == "t_sink"){
						tSink = dValue;
					}
					else if(newParameter.name == "k_sink"){
						kSink = dValue;
					}
					else if(newParameter.name == "p_sink"){
						pSink = dValue;
					}
					else if(newParameter.name == "s_spreader"){
						sSpreader = dValue;
					}
					else if(newParameter.name == "t_spreader"){
						tSpreader = dValue;
					}
					else if(newParameter.name == "k_spreader"){
						kSpreader = dValue;
					}
					else if(newParameter.name == "p_spreader"){
						pSpreader = dValue;
					}
					else if(newParameter.name == "t_interface"){
						tInterface = dValue;
					}
					else if(newParameter.name == "k_interface"){
						kInterface = dValue;
					}
					else if(newParameter.name == "p_interface"){
						pInterface = dValue;
					}
					else if(newParameter.name == "r_convec_sec"){
						rConvecSec = dValue;
					}
					else if(newParameter.name == "c_convec_sec"){
						cConvecSec = dValue;
					}
					else if(newParameter.name == "n_metal"){
						nMetal = dValue;
					}
					else if(newParameter.name == "t_metal"){
						tMetal = dValue;
					}
					else if(newParameter.name == "t_c4"){
						tC4 = dValue;
					}
					else if(newParameter.name == "s_c4"){
						sC4 = dValue;
					}
					else if(newParameter.name == "n_c4"){
						nC4 = dValue;
					}
					else if(newParameter.name == "s_sub"){
						sSub = dValue;
					}
					else if(newParameter.name == "t_sub"){
						tSub = dValue;
					}
					else if(newParameter.name == "s_solder"){
						sSolder = dValue;
					}
					else if(newParameter.name == "t_solder"){
						tSolder = dValue;
					}
					else if(newParameter.name == "s_pcb"){
						sPcb = dValue;
					}
					else if(newParameter.name == "t_pcb"){
						tPcb = dValue;
					}
					else if(newParameter.name == "ambient"){
						Tamb = dValue;
					}
					else if(newParameter.name == "init_temp"){
						initTemp = dValue;
					}
					else if(newParameter.name == "sampling_intvl"){
						sampligInterval = dValue;
					}
					else if(newParameter.name == "base_proc_freq"){
						baseProcFreq = dValue;
					}
					else{
						cout << "Error: \"" << newParameter.name << "\" is not a valid parameter. Please check usage." << endl;
						return false;
					}

					if((streamValue.rdstate() != stringstream::goodbit) && (streamValue.rdstate() != stringstream::eofbit)){
						cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
						return false;
					}
				}
				catch(...){
					cout << "Error: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
					return false;
				}
			}



			// If it is not repeated, then we add it to the list
			if(repeatedIndex < 0){
				parameters.push_back(newParameter);
			}
			// If it is repeated and the first value has priority, then we replace it
			else{
				parameters[repeatedIndex] = newParameter;
			}
		}

		return true;
	}
	else{
		return false;
	}
}
