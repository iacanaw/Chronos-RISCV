/**	\file	matex.cpp
 *	\brief	Main source file of the application.
 */

#include "matex.h"




/**	\brief	Main function of the application.
 *
 */
int main(int argc, char *argv[])
{
	// Setup timing measurements
	timespec time_resolution;
	clock_getres(CLOCK_MONOTONIC, &time_resolution);
	//cout << endl << "Time measurement resolution: " << time_resolution.tv_sec << "seg. and " << time_resolution.tv_nsec << "nseg." << endl;
	double SECONDS_TO_EXEC_TIME_UNIT = 1000000000.0 / time_resolution.tv_nsec;
	double NANOSECONDS_TO_EXEC_TIME_UNIT = time_resolution.tv_nsec;

	timespec startComputation, endComputation, startProgram, endProgram;
	clock_gettime(CLOCK_MONOTONIC, &startProgram);


	ConfigurationParameters configurationParameters;
	Floorplan floorplan;
	RCmodel rcModel;

	bool doTransient = false;
	bool doMaximumTemperatures = true;




	// Read the configuration from the command line
	configurationParameters.parseCommandLine(argc, argv);

	// If a configuration file is passed in the command line, read the rest of the configuration from there. Parameters passed through the command line have priority over the configuration file
	configurationParameters.readConfigurationFile();

	// Verify the configuration
	if(configurationParameters.verify() == false){
		cout << "Error: The configuration has some invalid parameters." << endl;
		exit(1);
	}

	// If an output file (with option "-o") was passed as a command line argument, then the application will compute the complete transient output for the power trace file. Thus, we mark the corresponding flag.
	if(configurationParameters.fileNameBlockTransientOutputTemperatures.size() > 0)
		doTransient = true;

//	/* if package model is used, run package model */
//	if (((idx = get_str_index(table, size, "package_model_used")) >= 0) && !(table[idx].value==0)) {
//		if (thermal_config.package_model_used) {
//			avg_sink_temp = thermal_config.ambient + SMALL_FOR_CONVEC;
//			natural = package_model(&thermal_config, table, size, avg_sink_temp);
//			if (thermal_config.r_convec<R_CONVEC_LOW || thermal_config.r_convec>R_CONVEC_HIGH)
//				printf("Warning: Heatsink convection resistance is not realistic, double-check your package settings...\n");
//		}
//	}

	// If a configuration dump file was passed as a command line argument, the configuration is dumped into the file
	if(configurationParameters.fileNameDumpConfiguration.size() > 0)
		configurationParameters.dumpToFile(configurationParameters.fileNameDumpConfiguration);






	/* Number of iterations */
	const int newtonsIterations = configurationParameters.iterations;


	/* Declare and compute the necessary information of the RC thermal network */
	double** Binv;
	double** C;
	double** Cexp;
	double* Gamb;
	double Tamb = configurationParameters.Tamb;
	int numberUnits;
	int numberNodesAmbient;
	int numberThermalNodes;


	/* Declare and compute the eigenvalues and eigenvectors of matrix C */
	double* eigenValues;
	double** eigenVectors;
	double** eigenVectorsInv;

	/* output matrices files */
	ofstream outputCexp;
	outputCexp.open("../cexp.txt");
	ofstream outputBinv;
	outputBinv.open("../binv.txt");




	// If there was an input file with the eigenvalues and eigenvectors, then we read it. In such a case, the floorplan is ignored and the data is directly read from the file.
	// To speed up computation, this is a binary file.
	if(configurationParameters.fileNameEigenvaluesEigenvectorsIn.size() > 0){
		if(readEigenvaluesEigenvectorsFromFile(configurationParameters.fileNameEigenvaluesEigenvectorsIn, numberUnits, numberNodesAmbient, numberThermalNodes, floorplan, Binv, Gamb, eigenValues, eigenVectors, eigenVectorsInv) == false){
			cout << "Error: There was an error reading the eigenvalues and eigenvectors information file." << endl;
			exit(1);
		}
	}
	else{
		// Read the floorplan from the floorplan file
		if(floorplan.readFloorplan(configurationParameters.fileNameFloorplan, true) == false){
			cout << "Error: There are no functional units in the floorplan." << endl;
			exit(1);
		}
		numberUnits = floorplan.getNumberFunctionalUnits();
		numberThermalNodes = floorplan.getNumberThermalNodes();


		// Load the RC thermal network from the floorplan information
		if(rcModel.load(configurationParameters, floorplan, doTransient || doMaximumTemperatures) == false){
			cout << "Error: The RC thermal network could not be loaded." << endl;
			exit(1);
		}
		numberNodesAmbient = rcModel.numberNodesAmbient;

		Binv = new double*[numberThermalNodes];
		C = new double*[numberThermalNodes];
		Cexp = new double*[numberThermalNodes];
		for(int i = 0; i < numberThermalNodes; i++){
			Binv[i] = new double[numberThermalNodes];
			C[i] = new double[numberThermalNodes];
			Cexp[i] = new double[numberThermalNodes];
		}
		for(int row = 0; row < numberThermalNodes; row++){
			for(int column = 0; column < numberThermalNodes; column++){
				Binv[row][column] = rcModel.Binv(row,column);
				C[row][column] = rcModel.C(row,column);
				outputBinv << Binv[row][column] << " "; //alzemiro << hexfloat 
			}
			outputBinv << endl;
		}
		outputBinv.close();

		Gamb = new double[numberNodesAmbient];
		for(int row = 0; row < numberNodesAmbient; row++){
			Gamb[row] = rcModel.Gamb(row);
		}

		#if VERBOSE > 2
		rcModel.print();
		#endif





		/* Compute the eigenvalues and eigenvectors of matrix C in the RC thermal network.	*/
		if(doTransient || doMaximumTemperatures){
			EigenSolver< Matrix<double, Dynamic, Dynamic> > es(rcModel.C);
			Matrix<double, Dynamic, 1> eigenValuesAux = es.eigenvalues().real();
			Matrix<double, Dynamic, Dynamic> eigenVectorsAux = es.eigenvectors().real();
			Matrix<double, Dynamic, Dynamic> eigenVectorsInvAux = eigenVectorsAux.inverse();


			// Create eigenvalues and eigenvectors
			eigenValues = new double[eigenValuesAux.rows()];
			for(int i = 0; i < eigenValuesAux.rows(); i++){
				eigenValues[i] = eigenValuesAux(i);
			}

			// Create eigenvalues and eigenvectors
			eigenVectors = new double*[eigenVectorsAux.rows()];
			for(int i = 0; i < eigenVectorsAux.rows(); i++){
				eigenVectors[i] = new double[eigenVectorsAux.cols()];
				for(int j = 0; j < eigenVectorsAux.cols(); j++){
					eigenVectors[i][j] = eigenVectorsAux(i,j);
				}
			}

			eigenVectorsInv = new double*[eigenVectorsAux.rows()];
			for(int i = 0; i < eigenVectorsAux.rows(); i++){
				eigenVectorsInv[i] = new double[eigenVectorsAux.cols()];
				for(int j = 0; j < eigenVectorsAux.cols(); j++){
					eigenVectorsInv[i][j] = eigenVectorsInvAux(i,j);
				}
			}


			// Check that no eigenvalue is complex. If so, then the computation needs to be slightly changed. However, this should never happen for a valid RC thermal network.
			for (int n = 0; n < es.eigenvalues().size(); n++){
				if(abs(remainder(arg(es.eigenvalues()(n)),M_PI)) > 1e-12)
					cout << "Eigen value (" << n+1 << ") is complex: " << es.eigenvalues()(n) << endl;
			}
		}
	}



	// If an output file for the eigenvalues and eigenvectors was specified, we save the information in the corresponding file
	if(configurationParameters.fileNameEigenvaluesEigenvectorsOut.size() > 0){
		if(saveEigenvaluesEigenvectorsToFile(configurationParameters.fileNameEigenvaluesEigenvectorsOut, rcModel, floorplan, eigenValues, eigenVectors, eigenVectorsInv) == false){
			cout << "Error: There was an error saving the eigenvalues and eigenvectors information file." << endl;
			exit(1);
		}
	}





	/* Declare H matrices for the transient and maximum temperature computations	*/
	double *multiplierH = new double[numberThermalNodes];
	double **HfunctionWithoutLambda = new double*[numberThermalNodes];
	double **Hfunction = new double*[numberThermalNodes];
	double **HfunctionSquaredLambda = new double*[numberThermalNodes];
	for(int i = 0; i < numberThermalNodes; i++){
		HfunctionWithoutLambda[i] = new double[numberThermalNodes];
		Hfunction[i] = new double[numberThermalNodes];
		HfunctionSquaredLambda[i] = new double[numberThermalNodes];
	}



	/* Temperature and power vectors */
	double* Tinit = new double[numberThermalNodes];
	double* Tsteady = new double[numberThermalNodes];
	double* TempTraceEnd = new double[numberThermalNodes];
	double* Ttrans = new double[numberThermalNodes];
	//double* TempTraceStart = new double[numberThermalNodes];
	double *Tdifference = new double[numberThermalNodes];
	double *exponentials = new double[numberThermalNodes];
	double* Power = new double[numberThermalNodes];
	double *previousPower = new double[numberThermalNodes];
	for(int i = 0; i < numberThermalNodes; i++){
		Power[i] = -1;
	}



	/* Set up initial instantaneous temperatures
	 * Note:	If the first line in the power trace file is with time less than 0, then the initial temperature is computed as the steady-state temperature for such power values,
	 *			overriding the initial temperature file or initial temperature value in the configuration. If in the power trace file there are several lines with time less than 0,
	 *			the initial temperature is computed for the last line before the time is greater than or equal to zero.
	 */
	if(doTransient || doMaximumTemperatures){
		/* Check if there is a file with the initial temperatures */
		if(configurationParameters.fileNameInitialTemperatures.size() > 0){
			readTemperaturesInitFile(Tinit, configurationParameters, floorplan);
		}
		else{
			/* If there was no initial temperature file, then use the initial temperature parameter	*/
			if(configurationParameters.initTemp >= 0){
				for(int i = 0; i < numberThermalNodes; i++){
					Tinit[i] = configurationParameters.initTemp;
				}
			}
			else{
				cout << "Error: The initial temperature cannot be a negative value." << endl;
				exit(1);
			}
		}
	}


	/* Open output file in which to store the transient temperatures	*/
	ofstream outputTransientTemperatures;
	bool saveOutputTransientTemperatures = false;
	if(doTransient){
		if(configurationParameters.fileNameBlockTransientOutputTemperatures.size() > 0){
			outputTransientTemperatures.open (configurationParameters.fileNameBlockTransientOutputTemperatures.c_str());
			if (outputTransientTemperatures.is_open()){
				if (outputTransientTemperatures.good()){
					saveOutputTransientTemperatures = true;
				}
				else{
					cout << "Error: Output transient temperature file: \"" << configurationParameters.fileNameBlockTransientOutputTemperatures << "\" could not be open for writing." << endl;
					outputTransientTemperatures.close();
					exit(1);
				}
			}
			else{
				cout << "Error: Output transient temperature file: \"" << configurationParameters.fileNameBlockTransientOutputTemperatures << "\" could not be open for writing." << endl;
				exit(1);
			}
		}
	}


	/* Open output file in which to store the maximum temperatures	*/
	ofstream outputMaximumTemperatures;
	bool saveOutputMaximumTemperatures = false;
	if(doMaximumTemperatures){
		if(configurationParameters.fileNameMaximumOutputTemperatures.size() > 0){
			outputMaximumTemperatures.open (configurationParameters.fileNameMaximumOutputTemperatures.c_str());
			if (outputMaximumTemperatures.is_open()){
				if (outputMaximumTemperatures.good()){
					saveOutputMaximumTemperatures = true;
				}
				else{
					cout << "Error: Output maximum temperatures file: \"" << configurationParameters.fileNameMaximumOutputTemperatures << "\" could not be open for writing." << endl;
					outputMaximumTemperatures.close();
					exit(1);
				}
			}
			else{
				cout << "Error: Output maximum temperatures file: \"" << configurationParameters.fileNameMaximumOutputTemperatures << "\" could not be open for writing." << endl;
				exit(1);
			}
		}
	}

	if(saveOutputMaximumTemperatures == false){
		if(saveOutputTransientTemperatures == true)
			doMaximumTemperatures = false;
	}




	


	/* Open input power trace file */
	ifstream inputPowerTraceFile(configurationParameters.fileNamePowerTrace.c_str());
	if ( inputPowerTraceFile.is_open() ){
		if(inputPowerTraceFile.good()){
			// Read the first line of the trace file to get the column indexes of the functional units, with respect to the floorplan
			string lineNames;
			getline(inputPowerTraceFile, lineNames);

			// If the line was successfully read
			if(inputPowerTraceFile.good()){
				vector< int > unitIndexInFloorplan;

				stringstream lineStreamNames(lineNames);
				lineStreamNames.exceptions(stringstream::goodbit);

				// Read all unit names
				while(lineStreamNames.good()){
					string auxUnitName;
					lineStreamNames >> auxUnitName;

					// If the unit name was successfully read
					if((lineStreamNames.rdstate() == stringstream::goodbit) || (lineStreamNames.rdstate() == stringstream::eofbit)){
						if(auxUnitName == "time"){
							bool timeColumnPresent = false;
							for(unsigned int i = 0; i < unitIndexInFloorplan.size(); i++)
								if(unitIndexInFloorplan[i] == -1)
									timeColumnPresent = true;

							if(timeColumnPresent == false)
								unitIndexInFloorplan.push_back(-1);
							else{
								cout << "Error: The input power trace file has more than one \"time\" column." << endl;
								inputPowerTraceFile.close();
								if(saveOutputTransientTemperatures)
									outputTransientTemperatures.close();
								if(saveOutputMaximumTemperatures)
									outputMaximumTemperatures.close();
								exit(1);
							}
						}
						else{
							int indexUnit = -1;
							for(int i = 0; i < numberUnits; i++){
								if(auxUnitName == floorplan.units[i].name){
									indexUnit = i;
									break;
								}
							}

							// If the unit read from the power trace was found in the floorplan
							if(indexUnit >= 0){
								unitIndexInFloorplan.push_back(indexUnit);
							}
							else{
								cout << "Error: The input power trace file does not match the floorplan." << endl;
								inputPowerTraceFile.close();
								if(saveOutputTransientTemperatures)
									outputTransientTemperatures.close();
								if(saveOutputMaximumTemperatures)
									outputMaximumTemperatures.close();
								exit(1);
							}
						}
					}
				}


				// Check that the input power trace file is valid
				if(unitIndexInFloorplan.size() != numberUnits + 1){
					cout << "Error: The input power trace file is invalid or it does not match the floorplan." << endl;
					inputPowerTraceFile.close();
					if(saveOutputTransientTemperatures)
						outputTransientTemperatures.close();
					if(saveOutputMaximumTemperatures)
						outputMaximumTemperatures.close();
					exit(1);
				}


				// Write the first line of the output transient temperature file
				if(saveOutputTransientTemperatures){
					for(unsigned int i = 0; i < unitIndexInFloorplan.size(); i++){
						if(unitIndexInFloorplan[i] < 0){
							outputTransientTemperatures << "time";
						}
						else if(unitIndexInFloorplan[i] < numberUnits){
							outputTransientTemperatures << floorplan.units[unitIndexInFloorplan[i]].name;
						}

						if(i == (unitIndexInFloorplan.size() - 1))
							outputTransientTemperatures << endl;
						else
							outputTransientTemperatures << "\t";
					}
				}


				// Write the first line of the output transient temperature file
				if(saveOutputMaximumTemperatures){
					outputMaximumTemperatures << "time\tMaxTemp[Celsius]\tUnitName" << endl;
				}





				// Read all power traces until the end of the file
				unsigned long int lineNumber = 1;
				double timePoint;
				double previousTimePoint = -1;
				double lastMaxTimePoint = -1;
				bool preivousPointEqualPower = false;


				clock_gettime(CLOCK_MONOTONIC, &startComputation);

				while(inputPowerTraceFile.good()){
					// Read one line from the file
					string linePowers;
					getline(inputPowerTraceFile, linePowers);

					// If the line was successfully read
					if(inputPowerTraceFile.good()){

						timePoint = -1;
						for(int i = 0; i < numberThermalNodes; i++){
							previousPower[i] = Power[i];
						}

						stringstream lineStreamPowers(linePowers);
						lineStreamPowers.exceptions(stringstream::goodbit);

						// Read the power information of each functional unit
						unsigned int columnNumber = 0;
						while(lineStreamPowers.good()){
							double auxValue;
							lineStreamPowers >> auxValue;

							// If the power value was successfully read
							if((lineStreamPowers.rdstate() == stringstream::goodbit) || (lineStreamPowers.rdstate() == stringstream::eofbit)){
								if(columnNumber < unitIndexInFloorplan.size()){
									if(unitIndexInFloorplan[columnNumber] < 0){
										timePoint = auxValue;
									}
									else if(unitIndexInFloorplan[columnNumber] < numberUnits){
										if(auxValue < 0){
											cout << "Error: The input power trace file is invalid. Line number " << lineNumber << " has a negative power value " << auxValue << "." << endl;
											inputPowerTraceFile.close();
											if(saveOutputTransientTemperatures)
												outputTransientTemperatures.close();
											if(saveOutputMaximumTemperatures)
												outputMaximumTemperatures.close();
											exit(1);
										}
										Power[unitIndexInFloorplan[columnNumber]] = auxValue;
									}
								}
							}
							columnNumber++;
						}

						// Check if the number of columns in the line is valid
						if(columnNumber != numberUnits + 1){
							cout << "Error: The input power trace file is invalid. Line number " << lineNumber << " has " << columnNumber << " columns." << endl;
							cout << "Every line in this power trace file should have " << numberUnits + 1 << " columns. One column is the \"time\" column, and the rest are columns of each functional unit." << endl;
							inputPowerTraceFile.close();
							if(saveOutputTransientTemperatures)
								outputTransientTemperatures.close();
							if(saveOutputMaximumTemperatures)
								outputMaximumTemperatures.close();
							exit(1);
						}



						// At this point we have successfully read a line from the input power trace file. Now we compute the temperature according to the power values read.
						if(timePoint < 0){
							if(previousTimePoint < 0){
								/* Check if the first line (or lines) in the power trace file starts with time < 0. If so, the initial temperature is computed
								 * as the steady state for such power values. If there are several lines with time < 0, the initial temperature is computed for the last line with time < 0
								 * before the time becomes >= 0
								 */
								computeSteadyStateTemperatures(Tinit, numberUnits, numberNodesAmbient, numberThermalNodes, Tamb, Power, Binv, Gamb);
							}
						}
						else{
							preivousPointEqualPower = true;
							for(int i = 0; i < numberThermalNodes; i++){
								if(previousPower[i] != Power[i]){
									preivousPointEqualPower = false;
									break;
								}
							}

							if(preivousPointEqualPower == false){
								// Up to the first line read with time >= 0, we consider that the temperature is the initial temperature. Hence, if this is the first line we do not compute the
								// new transient temperatures yet, because we do not know until what point in time the temperatures should be computed.
								if(previousTimePoint < 0){
									// If we are saving all the transient temperatures in a file, this was the first line read, and the timePoint is larger than zero, then we fill
									// the temperatures until this timePoint with the initial temperature
									if(saveOutputTransientTemperatures && (timePoint > 0)){
										for(double t = 0; t < timePoint; t = t + configurationParameters.sampligInterval){
											for(unsigned int i = 0; i < unitIndexInFloorplan.size(); i++){
												if(unitIndexInFloorplan[i] < 0){
													outputTransientTemperatures << t;
													cout << t;
												}
												else if(unitIndexInFloorplan[i] < numberUnits){
													outputTransientTemperatures << Tinit[unitIndexInFloorplan[i]] - Tkelvin;
													//cout << Tinit[unitIndexInFloorplan[i]] - Tkelvin;
												}

												if(i == (unitIndexInFloorplan.size() - 1)){
													outputTransientTemperatures << endl;
													//cout << endl;
												}
												else{
													outputTransientTemperatures << "\t";
													//cout << "\t";
												}
											}
										}
									}
								}
								else{
									// If the transient state should be computed for all time intervals
									if(saveOutputTransientTemperatures || doMaximumTemperatures){

										/* Compute the function H */
										for(int j = 0; j < numberThermalNodes; j++){
											Tdifference[j] = Tinit[j] - Tsteady[j];
										}

										//double** Cexp;
										for(int k = 0; k < numberThermalNodes; k++){
											for(int j = 0; j < numberThermalNodes; j++){
												double multiplierAux = 0;
												for(int i = 0; i < numberThermalNodes; i++){
													multiplierAux += eigenVectors[k][i] * pow((double)M_E, eigenValues[i] * 0.001) * eigenVectorsInv[i][j];
												}
												Cexp[k][j] = multiplierAux;
												outputCexp << Cexp[k][j] << " "; // << hexfloat
											}
											outputCexp << endl;
										}
										outputCexp.close();

										for(int i = 0; i < numberThermalNodes; i++){
											double multiplierAux = 0;
											for(int j = 0; j < numberThermalNodes; j++){
												multiplierAux += Tdifference[j] * eigenVectorsInv[i][j];
											}
											multiplierH[i] = multiplierAux;
										}
										//
										for(int k = 0; k < numberThermalNodes; k++){
											for(int i = 0; i < numberThermalNodes; i++){
												double multiplierAux = multiplierH[i] * eigenVectors[k][i];
												HfunctionWithoutLambda[k][i] = multiplierAux;
												multiplierAux *= eigenValues[i];
												Hfunction[k][i] = multiplierAux;
												HfunctionSquaredLambda[k][i] = multiplierAux * eigenValues[i];
											}
										}



										/* Compute all temperatures at the beginning of this trace.		*/
										/*{
											for(int k = 0; k < numberThermalNodes; k++){
												double sumExponentials = 0;
												for(int i = 0; i < numberThermalNodes; i++){
													sumExponentials += HfunctionWithoutLambda[k][i];
												}

												TempTraceStart[k] = Tsteady[k] + sumExponentials;
											}
										}*/


										/* Compute all temperatures at the end of this trace. This is to later update Tinit		*/
										double timeTraceEnd = timePoint - previousTimePoint;
										{
											//cout << timeTraceEnd << endl;
											for(int i = 0; i < numberThermalNodes; i++){
												exponentials[i] = pow((double)M_E, eigenValues[i] * timeTraceEnd);
											}

											for(int k = 0; k < numberThermalNodes; k++){
												double sumExponentials = 0;
												for(int i = 0; i < numberThermalNodes; i++){
													sumExponentials += exponentials[i] * HfunctionWithoutLambda[k][i];
												}

												TempTraceEnd[k] = Tsteady[k] + sumExponentials;
												//if (k < 9) cout << "sumExponentials real " << k << ": " << sumExponentials << endl;
												//if (k < 9) cout << "TempTraceEnd real " << k << ": " << TempTraceEnd[k] - Tkelvin << endl;
												//if (k == 4) cout << "Tdifference[" << k << "]: " << Tdifference[k] << endl;
											}
											//cout << "===================================================================" << endl;

											/*for(int k = 0; k < numberThermalNodes; k++){
												double sumExponentials = 0;
												for(int j = 0; j < numberThermalNodes; j++){
													sumExponentials += Cexp[k][j] * Tdifference[j];
												}
												TempTraceEnd[k] = Tsteady[k] + sumExponentials;
												//if (k < 9) cout << "Tsteady cexp " << k << ": " << Tsteady[k] << endl;
												//if (k < 9) cout << "TempTraceEnd cexp " << k << ": " << TempTraceEnd[k] << endl;
												if (k < 9) cout << "sumExponentials cexp " << k << ": " << sumExponentials << endl;
											}*/
										}
										//printTemperatures(floorplan, TempTraceEnd); //alzemiro


										/* If required, we generate a trace file with all transient temperatures according to the step size t_step */
										if(saveOutputTransientTemperatures){
											//for(double t = 0; t < timeTraceEnd; t = t + configurationParameters.sampligInterval){
												double t = 0;
												for(int i = 0; i < numberThermalNodes; i++){
													exponentials[i] = pow((double)M_E, eigenValues[i] * t);
												}

												for(unsigned int k = 0; k < unitIndexInFloorplan.size(); k++){
													if(unitIndexInFloorplan[k] < 0){
														outputTransientTemperatures << t + previousTimePoint;
														cout << t + previousTimePoint << " " << t << " " << previousTimePoint << endl;
													}
													else if(unitIndexInFloorplan[k] < numberUnits){
														double sumExponentials = 0;
														for(int i = 0; i < numberThermalNodes; i++){
															sumExponentials += exponentials[i] * HfunctionWithoutLambda[unitIndexInFloorplan[k]][i];
															//Cexp[k][i] = exponentials[i] * HfunctionWithoutLambda[unitIndexInFloorplan[k]][i] /Tdifference[i];
															//sumExponentials += Cexp[k][i] * Tdifference[i];
															//if (k == 4) cout << i << " " << exponentials[i] << " " << HfunctionWithoutLambda[unitIndexInFloorplan[k]][i] << " " << sumExponentials << endl;
															outputCexp << Cexp[k][i] << " "; //alzemiro
														}
														outputCexp << endl;

														if (k <= 36) cout << k << "-sumExponentials: " << sumExponentials << " Tdifference: " << endl;
														outputTransientTemperatures << Tsteady[unitIndexInFloorplan[k]] + sumExponentials - Tkelvin;
														//if (k == 4) cout << Tsteady[unitIndexInFloorplan[k]] + sumExponentials - Tkelvin;
														//if (k == 4) cout << " " << Tsteady[unitIndexInFloorplan[k]];
														//if (k == 4) cout << " " << sumExponentials;
													}

													if(k == (unitIndexInFloorplan.size() - 1)){
														outputTransientTemperatures << endl;
														//cout << endl;
													}
													else{
														outputTransientTemperatures << "\t";
														//cout << "\t";
													}
												}
												outputCexp.close();
											//}
										}


										/* Compute the peak temperatures on each core for every power change */
										if(doMaximumTemperatures){
											double maxTemperature = 0;
											double tMaxTemperature = 0;
											unsigned int unitMaxTemperature = 0;
											cout << "newtonsIterations: " << newtonsIterations << endl;
											for(unsigned int k = 0; k < unitIndexInFloorplan.size(); k++){

												if((unitIndexInFloorplan[k] >= 0) && (unitIndexInFloorplan[k] < numberUnits)){

													double t_max = 0;
													for(int iteration = 0; iteration < newtonsIterations; iteration++){
														double numerator = 0;
														double denominator = 0;
														for(int i = 0; i < numberThermalNodes; i++){
															double e_t = pow((double)M_E, eigenValues[i] * t_max);
															numerator += Hfunction[unitIndexInFloorplan[k]][i] * e_t;
															denominator += HfunctionSquaredLambda[unitIndexInFloorplan[k]][i] * e_t;
														}
														t_max = t_max - numerator/denominator;
													}


													double T;
													if((t_max <= 0) || (t_max >= timeTraceEnd)){
														//if(TempTraceStart[unitIndexInFloorplan[k]] > TempTraceEnd[unitIndexInFloorplan[k]]){
														if(Tinit[unitIndexInFloorplan[k]] > TempTraceEnd[unitIndexInFloorplan[k]]){
															t_max = 0;
															//T = TempTraceStart[unitIndexInFloorplan[k]];
															T = Tinit[unitIndexInFloorplan[k]];
														}
														else{
															t_max = timeTraceEnd;
															T = TempTraceEnd[unitIndexInFloorplan[k]];
														}
													}
													else{
														T = 0;
														for(int i = 0; i < numberThermalNodes; i++){
															T += HfunctionWithoutLambda[unitIndexInFloorplan[k]][i] * pow((double)M_E, eigenValues[i] * t_max);
														}
														T += Tsteady[unitIndexInFloorplan[k]];


														//if(TempTraceStart[unitIndexInFloorplan[k]] > T){
														//	t_max = 0;
														//	T = TempTraceStart[unitIndexInFloorplan[k]];
														//}
														if(Tinit[unitIndexInFloorplan[k]] > T){
															t_max = 0;
															T = Tinit[unitIndexInFloorplan[k]];
														}

														if(TempTraceEnd[unitIndexInFloorplan[k]] > T){
															t_max = timeTraceEnd;
															T = TempTraceEnd[unitIndexInFloorplan[k]];
														}
													}


													if(maxTemperature < T){
														maxTemperature = T;
														tMaxTemperature = t_max;
														unitMaxTemperature = unitIndexInFloorplan[k];
													}
												}
											}

											// Check that we do not print a repeated max temperature
											if(lastMaxTimePoint != (tMaxTemperature + previousTimePoint)){
												cout << "Maximum Temperature = " << maxTemperature - Tkelvin << " °C, at time = " << tMaxTemperature + previousTimePoint << " sec.";
												if(unitMaxTemperature < floorplan.getNumberFunctionalUnits())
													cout << ", in unit \"" << floorplan.units[unitMaxTemperature].name << "\".";
												cout << endl;
												if(saveOutputMaximumTemperatures){
													outputMaximumTemperatures << tMaxTemperature + previousTimePoint << "\t" << maxTemperature - Tkelvin;
													if(unitMaxTemperature < floorplan.getNumberFunctionalUnits())
														outputMaximumTemperatures << "\t" << floorplan.units[unitMaxTemperature].name;
													outputMaximumTemperatures << endl;
												}
												lastMaxTimePoint = tMaxTemperature + previousTimePoint;
											}
										}

										// The temperature at the end of this trace is the Tinit for the next trace
										for(int i = 0; i < numberThermalNodes; i++){
											Tinit[i] = TempTraceEnd[i];
										}
									}
								}

								// The current time point is the previous time point for the next trace
								previousTimePoint = timePoint;

								// Compute the steady state temperature after the change in power
								computeSteadyStateTemperatures(Tsteady, numberUnits, numberNodesAmbient, numberThermalNodes, Tamb, Power, Binv, Gamb);
							}
						}
					}

					lineNumber++;
				}


				/* Compute the peak temperatures on each core for the last power change */
				if((previousTimePoint >= 0) && (doMaximumTemperatures || (preivousPointEqualPower && saveOutputTransientTemperatures))){
					/* Compute the function H */
					for(int j = 0; j < numberThermalNodes; j++){
						Tdifference[j] = Tinit[j] - Tsteady[j];
					}
					//Tdifference[0] = -100;
					for(int i = 0; i < numberThermalNodes; i++){
						double multiplierAux = 0;
						for(int j = 0; j < numberThermalNodes; j++){
							multiplierAux += Tdifference[j] * eigenVectorsInv[i][j];
						}
						multiplierH[i] = multiplierAux;
					}
					//double** Cexp;
					for(int k = 0; k < numberThermalNodes; k++){
						for(int j = 0; j < numberThermalNodes; j++){
							double multiplierAux = 0;
							for(int i = 0; i < numberThermalNodes; i++){
								multiplierAux += eigenVectors[k][i] * eigenVectorsInv[i][j] * pow((double)M_E, eigenValues[i] * 0.001);
							}
							Cexp[k][j] = multiplierAux;
							outputCexp << Cexp[k][j] << " "; //alzemiro
						}
						outputCexp << endl;
					}
					outputCexp.close();
					for(int k = 0; k < numberThermalNodes; k++){
						for(int i = 0; i < numberThermalNodes; i++){
							double multiplierAux = multiplierH[i] * eigenVectors[k][i];
							HfunctionWithoutLambda[k][i] = multiplierAux;
							multiplierAux *= eigenValues[i];
							Hfunction[k][i] = multiplierAux;
							HfunctionSquaredLambda[k][i] = multiplierAux * eigenValues[i];
						}
					}

					/* Compute all temperatures at the beginning of this trace.		*/
					/*for(int k = 0; k < numberThermalNodes; k++){
						double sumExponentials = 0;
						for(int i = 0; i < numberThermalNodes; i++){
							sumExponentials += HfunctionWithoutLambda[k][i];
						}

						TempTraceStart[k] = Tsteady[k] + sumExponentials;
					}*/


					double timeTraceEnd = timePoint - previousTimePoint;
					{
						for(int i = 0; i < numberThermalNodes; i++){
							exponentials[i] = pow((double)M_E, eigenValues[i] * timeTraceEnd);
							//cout << "eigenValues[" << i << "]: " << eigenValues[i] << endl;
							//cout << "exponentials[" << i << "]: " << exponentials[i] << endl;
						}

						for(int k = 0; k < numberThermalNodes; k++){
							double sumExponentials = 0;
							for(int i = 0; i < numberThermalNodes; i++){
								sumExponentials += exponentials[i] * HfunctionWithoutLambda[k][i];
								//cout << "exponentials[" << i << "]: " << exponentials[i] << endl;
								//cout << "HfunctionWithoutLambda[" << k << "][" << i << "]: " << HfunctionWithoutLambda[k][i] << endl;
								//cout << "sumExponentials[" << i << "]: " << sumExponentials << endl;
							}
							TempTraceEnd[k] = Tsteady[k] + sumExponentials;
							//cout << "sumExponentials cexp " << k << ": " << sumExponentials << endl;
							//if (k < 36) cout << "TempTraceEnd[" << k << "]: " << TempTraceEnd[k] - Tkelvin << endl;
						}
						//cout << "============================================" << endl;
						/*for(int k = 0; k < numberThermalNodes; k++){
							double sumExponentials = 0;
							for(int j = 0; j < numberThermalNodes; j++){
								sumExponentials += Cexp[k][j] * Tdifference[j];
							}
							TempTraceEnd[k] = Tsteady[k] + sumExponentials;
							if (k < 36) cout << "Tsteady cexp " << k << ": " << Tsteady[k] - Tkelvin << endl;
							if (k < 36) cout << "TempTraceEnd cexp " << k << ": " << TempTraceEnd[k] - Tkelvin << endl;
							if (k < 36) cout << "sumExponentials cexp " << k << ": " << sumExponentials << endl;
						}*/
					}
					//printTemperatures(floorplan, TempTraceEnd); //alzemiro


					/* If required, we generate a trace file with all transient temperatures according to the step size t_step */
					if(preivousPointEqualPower && saveOutputTransientTemperatures){
						for(double t = 0; t <= timeTraceEnd; t = t + configurationParameters.sampligInterval){

							for(int i = 0; i < numberThermalNodes; i++){
								exponentials[i] = pow((double)M_E, eigenValues[i] * t);
							}

							for(unsigned int k = 0; k < unitIndexInFloorplan.size(); k++){ //unitIndexInFloorplan.size()
								if(unitIndexInFloorplan[k] < 0){
									outputTransientTemperatures << t + previousTimePoint;
								}
								else if(unitIndexInFloorplan[k] < numberUnits){
									double sumExponentials = 0;
									for(int i = 0; i < numberThermalNodes; i++){
										sumExponentials += exponentials[i] * HfunctionWithoutLambda[unitIndexInFloorplan[k]][i];
									}
									if (k <= 36) cout << k << "-sumExponentials: " << sumExponentials << endl;
									//cout << "sumExponentials[" << unitIndexInFloorplan[k] << "]: " << sumExponentials << endl;
									//cout << "Tsteady[" << unitIndexInFloorplan[k] << "]: " << Tsteady[unitIndexInFloorplan[k]] << endl;
									//cout << "TempTraceEnd[" << unitIndexInFloorplan[k] << "]: " << TempTraceEnd[unitIndexInFloorplan[k]] << endl;
									outputTransientTemperatures << Tsteady[unitIndexInFloorplan[k]] + sumExponentials - Tkelvin;
									//cout << "outputTransientTemperatures[" << unitIndexInFloorplan[k] << "]: " << Tsteady[unitIndexInFloorplan[k]] + sumExponentials << endl;
								}

								if(k == (unitIndexInFloorplan.size() - 1)){
									outputTransientTemperatures << endl;
								}
								else{
									outputTransientTemperatures << "\t";
								}
							}
						}
					}





					if(doMaximumTemperatures){
						double maxTemperature = 0;
						double tMaxTemperature = 0;
						unsigned int unitMaxTemperature = 0;
						for(unsigned int k = 0; k < unitIndexInFloorplan.size(); k++){

							if((unitIndexInFloorplan[k] >= 0) && (unitIndexInFloorplan[k] < numberUnits)){

								double t_max = 0;
								for(int iteration = 0; iteration < newtonsIterations; iteration++){
									double numerator = 0;
									double denominator = 0;
									for(int i = 0; i < numberThermalNodes; i++){
										double e_t = pow((double)M_E, eigenValues[i] * t_max);
										numerator += Hfunction[unitIndexInFloorplan[k]][i] * e_t;
										denominator += HfunctionSquaredLambda[unitIndexInFloorplan[k]][i] * e_t;
									}
									t_max = t_max - numerator/denominator;
								}



								/*double T = 0;
								if(t_max <= 0){
									//if(maxTemperature < TempTraceStart[unitIndexInFloorplan[k]]){
									//	maxTemperature = TempTraceStart[unitIndexInFloorplan[k]];
									//	tMaxTemperature = 0;
									//	unitMaxTemperature = unitIndexInFloorplan[k];
									//}
									if(maxTemperature < Tinit[unitIndexInFloorplan[k]]){
										maxTemperature = Tinit[unitIndexInFloorplan[k]];
										tMaxTemperature = 0;
										unitMaxTemperature = unitIndexInFloorplan[k];
									}
								}
								else{
									for(int i = 0; i < numberThermalNodes; i++){
										T += HfunctionWithoutLambda[unitIndexInFloorplan[k]][i] * pow((double)M_E, eigenValues[i] * t_max);
									}
									T += Tsteady[unitIndexInFloorplan[k]];

									//if(T < TempTraceStart[unitIndexInFloorplan[k]]){
									//	if(maxTemperature < TempTraceStart[unitIndexInFloorplan[k]]){
									//		maxTemperature = TempTraceStart[unitIndexInFloorplan[k]];
									//		tMaxTemperature = 0;
									//		unitMaxTemperature = unitIndexInFloorplan[k];
									//	}
									//}
									if(T < Tinit[unitIndexInFloorplan[k]]){
										if(maxTemperature < Tinit[unitIndexInFloorplan[k]]){
											maxTemperature = Tinit[unitIndexInFloorplan[k]];
											tMaxTemperature = 0;
											unitMaxTemperature = unitIndexInFloorplan[k];
										}
									}
									else{
										if(maxTemperature < T){
											maxTemperature = T;
											tMaxTemperature = t_max;
											unitMaxTemperature = unitIndexInFloorplan[k];
										}
									}
								}*/
								double T;
								if((t_max <= 0) || (t_max >= timeTraceEnd)){
									//if(TempTraceStart[unitIndexInFloorplan[k]] > TempTraceEnd[unitIndexInFloorplan[k]]){
									if(Tinit[unitIndexInFloorplan[k]] > TempTraceEnd[unitIndexInFloorplan[k]]){
										t_max = 0;
										//T = TempTraceStart[unitIndexInFloorplan[k]];
										T = Tinit[unitIndexInFloorplan[k]];
									}
									else{
										t_max = timeTraceEnd;
										T = TempTraceEnd[unitIndexInFloorplan[k]];
									}
								}
								else{
									T = 0;
									for(int i = 0; i < numberThermalNodes; i++){
										T += HfunctionWithoutLambda[unitIndexInFloorplan[k]][i] * pow((double)M_E, eigenValues[i] * t_max);
									}
									T += Tsteady[unitIndexInFloorplan[k]];


									//if(TempTraceStart[unitIndexInFloorplan[k]] > T){
									//	t_max = 0;
									//	T = TempTraceStart[unitIndexInFloorplan[k]];
									//}
									if(Tinit[unitIndexInFloorplan[k]] > T){
										t_max = 0;
										T = Tinit[unitIndexInFloorplan[k]];
									}

									if(TempTraceEnd[unitIndexInFloorplan[k]] > T){
										t_max = timeTraceEnd;
										T = TempTraceEnd[unitIndexInFloorplan[k]];
									}
								}


								if(maxTemperature < T){
									maxTemperature = T;
									tMaxTemperature = t_max;
									unitMaxTemperature = unitIndexInFloorplan[k];
								}
							}
						}
						if(lastMaxTimePoint != (tMaxTemperature + previousTimePoint)){
							cout << "Maximum Temperature = " << maxTemperature - Tkelvin << " °C, at time = " << tMaxTemperature + previousTimePoint << " sec.";
							if(unitMaxTemperature < floorplan.getNumberFunctionalUnits())
								cout << ", in unit \"" << floorplan.units[unitMaxTemperature].name << "\".";
							cout << endl;
							if(saveOutputMaximumTemperatures){
								outputMaximumTemperatures << tMaxTemperature + previousTimePoint << "\t" << maxTemperature - Tkelvin;
								if(unitMaxTemperature < floorplan.getNumberFunctionalUnits())
									outputMaximumTemperatures << "\t" << floorplan.units[unitMaxTemperature].name;
								outputMaximumTemperatures << endl;
							}
						}
					}
				}
			}
			else{
				cout << "Error: There was an error while reading the input power trace file." << endl;
				if(saveOutputTransientTemperatures)
					outputTransientTemperatures.close();
				if(saveOutputMaximumTemperatures)
					outputMaximumTemperatures.close();
				exit(1);
			}
		}
		else{
			cout << "Error: The input power trace file could not be open for reading." << endl;
			if(saveOutputTransientTemperatures)
				outputTransientTemperatures.close();
			if(saveOutputMaximumTemperatures)
				outputMaximumTemperatures.close();
			exit(1);
		}
	}
	else{
		cout << "Error: The input power trace file could not be open for reading." << endl;
		if(saveOutputTransientTemperatures)
			outputTransientTemperatures.close();
		if(saveOutputMaximumTemperatures)
			outputMaximumTemperatures.close();
		exit(1);
	}

	clock_gettime(CLOCK_MONOTONIC, &endComputation);



	// Close the output transient temperatures file
	if(saveOutputTransientTemperatures)
		outputTransientTemperatures.close();

	// Close the output maximum temperatures file
	if(saveOutputMaximumTemperatures)
		outputMaximumTemperatures.close();






	//printTemperatures(floorplan, Tsteady);


	/* Dump steady state temperatures onto file if needed	*/
	if(configurationParameters.fileNameBlockSteadyOutputTemperatures.size() > 0)
		dumpTemperaturesToFile(configurationParameters.fileNameBlockSteadyOutputTemperatures, floorplan, Tsteady);








	/* Delete all matrices and vectors that are dynamically allocated */
	for(int i = 0; i < numberThermalNodes; i++){
		delete[] Binv[i];
	}
	delete[] Binv;
	delete[] Gamb;


	for(int i = 0; i < numberThermalNodes; i++){
		delete[] HfunctionWithoutLambda[i];
		delete[] Hfunction[i];
		delete[] HfunctionSquaredLambda[i];
	}
	delete[] HfunctionWithoutLambda;
	delete[] Hfunction;
	delete[] HfunctionSquaredLambda;
	delete[] multiplierH;


	delete[] eigenValues;
	for(int i = 0; i < numberThermalNodes; i++){
		delete[] eigenVectors[i];
	}
	delete[] eigenVectors;


	delete[] Tinit;
	delete[] Tsteady;
	delete[] TempTraceEnd;
	//delete[] TempTraceStart;
	delete[] Tdifference;
	delete[] exponentials;
	delete[] Power;
	delete[] previousPower;








	clock_gettime(CLOCK_MONOTONIC, &endProgram);
	double totalExecutionTime = (((double)(endProgram.tv_sec - startProgram.tv_sec))*SECONDS_TO_EXEC_TIME_UNIT + ((double)(endProgram.tv_nsec - startProgram.tv_nsec))/NANOSECONDS_TO_EXEC_TIME_UNIT);
	double computationExecutionTime = (((double)(endComputation.tv_sec - startComputation.tv_sec))*SECONDS_TO_EXEC_TIME_UNIT + ((double)(endComputation.tv_nsec - startComputation.tv_nsec))/NANOSECONDS_TO_EXEC_TIME_UNIT);

/*
	cout << "#Total Execution Time [ms]: " << totalExecutionTime/(SECONDS_TO_EXEC_TIME_UNIT/1000) << endl;
	cout << "#Comp. Execution Time [ms]: " << computationExecutionTime/(SECONDS_TO_EXEC_TIME_UNIT/1000) << endl;
	cout << M_E << endl;
*/
	exit(0);
}











void computeSteadyStateTemperatures(double* &Tsteady, const int &numberUnits, const int &numberNodesAmbient, const int &numberThermalNodes, const double &Tamb, double* &Power, double** &Binv, double* &Gamb)
{
	int offsetGamb = numberThermalNodes - numberNodesAmbient;

	//cout << "numberNodesAmbient: " << numberNodesAmbient << endl;
	//cout << "offsetGamb: " << offsetGamb << endl;
	//cout << "numberThermalNodes: " << numberThermalNodes << endl;


	for(int i = 0; i < numberThermalNodes; i++){ // numberThermalNodes

		double heatContributionPower = 0;
		for(int j = 0; j < numberUnits; j++){
			heatContributionPower += Binv[i][j]*Power[j];

			//cout << "Binv[" << i << "][" << j  << "]: " << Binv[i][j] << endl;
			//cout << "Power[" << j << "]: " << Power[j] << endl;
			//cout << "heatContributionPower: " << heatContributionPower << endl;
		}

		double heatContributionAmbient = 0;
		for(int j = 0; j < numberNodesAmbient; j++){
			heatContributionAmbient += Binv[i][j + offsetGamb]*Gamb[j];
			//cout << "Binv[" << i << "][" << j + offsetGamb  << "]: " << Binv[i][j] << endl;
			//cout << "Gamb[" << j << "]: " << Gamb[j] << endl;
			//cout << "heatContributionAmbient: " << heatContributionAmbient << endl;
		}
		//cout << "=============================================================" << endl;
		//cout << "heatContributionPower: " << heatContributionPower << endl;
		//cout << "heatContributionAmbient: " << heatContributionAmbient << endl;
		Tsteady[i] = heatContributionPower + Tamb*heatContributionAmbient;
		//cout << "Tsteady[" << i << "]: " << Tsteady[i] << endl;
	}
}
















void printTemperatures(Floorplan &floorplan, double* &temperatures)
{
	int numberUnits = floorplan.getNumberFunctionalUnits();
	int numberThermalNodes = NUMBER_BLOCK_LAYERS*numberUnits + EXTRA_NODES;


	cout << "#Node\tTemperature[Kelvin]" << endl;
	/* On chip temperatures	*/
	for (int i = 0; i < numberUnits; i++)
		cout << floorplan.units[i].name << "\t" << temperatures[i] << endl;

	/* Interface temperatures	*/
	for (int i = 0; i < numberUnits; i++)
		cout << "iface_" << floorplan.units[i].name << "\t" << temperatures[IFACE*numberUnits + i] << endl;

	/* Spreader temperatures	*/
	for (int i = 0; i < numberUnits; i++)
		cout << "hsp_" << floorplan.units[i].name << "\t" << temperatures[HSP*numberUnits + i] << endl;

	/* Heatsink temperatures	*/
	for (int i = 0; i < numberUnits; i++)
		cout << "hsink_" << floorplan.units[i].name << "\t" << temperatures[HSINK*numberUnits + i] << endl;

	/* Internal node temperatures	*/
	for (int i = 0; i < EXTRA_NODES; i++)
		cout << "inode_" << i << "\t" << temperatures[i + NUMBER_BLOCK_LAYERS*numberUnits] << endl;
}




void dumpTemperaturesToFile(const string &fileName, Floorplan &floorplan, double* &temperatures)
{
	if(fileName.size() > 0){
		ofstream dumpFile;
		dumpFile.open (fileName.c_str());
		if (dumpFile.is_open()){
			int numberUnits = floorplan.getNumberFunctionalUnits();
			int numberThermalNodes = NUMBER_BLOCK_LAYERS*numberUnits + EXTRA_NODES;


			dumpFile << "#Node\tTemperature[Kelvin]" << endl;
			/* On chip temperatures	*/
			for (int i = 0; i < numberUnits; i++)
				dumpFile << floorplan.units[i].name << "\t" << temperatures[i] << endl;

			/* Interface temperatures	*/
			for (int i = 0; i < numberUnits; i++)
				dumpFile << "iface_" << floorplan.units[i].name << "\t" << temperatures[IFACE*numberUnits + i] << endl;

			/* Spreader temperatures	*/
			for (int i = 0; i < numberUnits; i++)
				dumpFile << "hsp_" << floorplan.units[i].name << "\t" << temperatures[HSP*numberUnits + i] << endl;

			/* Heatsink temperatures	*/
			for (int i = 0; i < numberUnits; i++)
				dumpFile << "hsink_" << floorplan.units[i].name << "\t" << temperatures[HSINK*numberUnits + i] << endl;

			/* Internal node temperatures	*/
			for (int i = 0; i < EXTRA_NODES; i++)
				dumpFile << "inode_ " << i << "\t" << temperatures[i + NUMBER_BLOCK_LAYERS*numberUnits] << endl;


			if(dumpFile.good() == false){
				cout << "Error: There was an error writing to the temperature dump file: \"" << fileName << "\"." << endl;
				dumpFile.close();
				exit(1);
			}

			dumpFile.close();
		}
		else{
			cout << "Error: Temperature dump file: \"" << fileName << "\" could not be open for writing." << endl;
			exit(1);
		}
	}
}




bool saveEigenvaluesEigenvectorsToFile(const string &fileName, const RCmodel &rcModel, Floorplan &floorplan, double* &eigenValues, double** &eigenVectors, double** &eigenVectorsInv)
{
	if(fileName.size() > 0){
		ofstream dumpFile;
		dumpFile.open (fileName.c_str(), ios::out | ios::binary);
		if (dumpFile.is_open()){

			dumpFile.write((const char*)&rcModel.numberUnits, sizeof(int));
			dumpFile.write((const char*)&rcModel.numberNodesAmbient, sizeof(int));
			dumpFile.write((const char*)&rcModel.numberThermalNodes, sizeof(int));

			for(int unit = 0; unit < floorplan.getNumberFunctionalUnits(); unit++){
				dumpFile << floorplan.units[unit].name << endl;
			}

			for(int row = 0; row < rcModel.Binv.rows(); row++){
				for(int column = 0; column < rcModel.Binv.cols(); column++){
					dumpFile.write((const char*)&rcModel.Binv(row,column), sizeof(double));
				}
			}

			for(int row = 0; row < rcModel.Gamb.rows(); row++){
				dumpFile.write((const char*)&rcModel.Gamb(row), sizeof(double));
			}

			for(int row = 0; row < rcModel.numberThermalNodes; row++){
				dumpFile.write((const char*)&eigenValues[row], sizeof(double));
			}

			for(int row = 0; row < rcModel.numberThermalNodes; row++){
				for(int column = 0; column < rcModel.numberThermalNodes; column++){
					dumpFile.write((const char*)&eigenVectors[row][column], sizeof(double));
				}
			}

			for(int row = 0; row < rcModel.numberThermalNodes; row++){
				for(int column = 0; column < rcModel.numberThermalNodes; column++){
					dumpFile.write((const char*)&eigenVectorsInv[row][column], sizeof(double));
				}
			}

			if(dumpFile.good() == false){
				cout << "Error: There was an error writing to the eigenvalues and eigenvectors dump file: \"" << fileName << "\"." << endl;
				dumpFile.close();
				exit(1);
			}

			dumpFile.close();

			return true;
		}
		else{
			cout << "Error: Eigenvalues and eigenvectors dump file: \"" << fileName << "\" could not be open for writing." << endl;
			exit(1);
		}
	}

	return false;
}


bool readEigenvaluesEigenvectorsFromFile(const string &fileName, int &numberUnits, int &numberNodesAmbient, int &numberThermalNodes, Floorplan &floorplan, double** &Binv, double* &Gamb, double* &eigenValues, double** &eigenVectors, double** &eigenVectorsInv)
{
	if(fileName.size() > 0){

		FunctionalUnit auxFunctionalUnit;

		ifstream inputEigenFile(fileName.c_str(), ios::in | ios::binary);
		inputEigenFile.exceptions(stringstream::goodbit);
		if ( inputEigenFile.is_open() ){
			if(inputEigenFile.good()){
				try{
					inputEigenFile.read((char*)&numberUnits, sizeof(int));
					if(numberUnits <= 0){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}

					inputEigenFile.read((char*)&numberNodesAmbient, sizeof(int));
					if(numberNodesAmbient <= 0){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}

					inputEigenFile.read((char*)&numberThermalNodes, sizeof(int));
					if(numberThermalNodes < numberUnits){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}

					for(int unit = 0; unit < numberUnits; unit++){
						//inputEigenFile >> auxFunctionalUnit.name;
						getline(inputEigenFile, auxFunctionalUnit.name);
						if(inputEigenFile.rdstate() != stringstream::goodbit){
							cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
							inputEigenFile.close();
							exit(1);
						}
						floorplan.units.push_back(auxFunctionalUnit);
					}

					if((numberUnits != floorplan.getNumberFunctionalUnits()) || (numberThermalNodes != floorplan.getNumberThermalNodes())){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}


					Binv = new double*[numberThermalNodes];
					for(int i = 0; i < numberThermalNodes; i++){
						Binv[i] = new double[numberThermalNodes];
					}
					for(int row = 0; row < numberThermalNodes; row++){
						for(int column = 0; column < numberThermalNodes; column++){
							inputEigenFile.read((char*)&Binv[row][column], sizeof(double));
						}
					}

					Gamb = new double[numberNodesAmbient];
					for(int row = 0; row < numberNodesAmbient; row++){
						inputEigenFile.read((char*)&Gamb[row], sizeof(double));
					}

					eigenValues = new double[numberThermalNodes];
					for(int row = 0; row < numberThermalNodes; row++){
						inputEigenFile.read((char*)&eigenValues[row], sizeof(double));
					}

					eigenVectors = new double*[numberThermalNodes];
					for(int row = 0; row < numberThermalNodes; row++){
						eigenVectors[row] = new double[numberThermalNodes];
						for(int column = 0; column < numberThermalNodes; column++){
							inputEigenFile.read((char*)&eigenVectors[row][column], sizeof(double));
						}
					}

					eigenVectorsInv = new double*[numberThermalNodes];
					for(int row = 0; row < numberThermalNodes; row++){
						eigenVectorsInv[row] = new double[numberThermalNodes];
						for(int column = 0; column < numberThermalNodes; column++){
							inputEigenFile.read((char*)&eigenVectorsInv[row][column], sizeof(double));
						}
					}


					// Verify that this and all previous values were successfully read
					if(inputEigenFile.rdstate() != stringstream::goodbit){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}


					// Read one more byte to verify that the file ends here
					char auxChar;
					inputEigenFile.read((char*)&auxChar, sizeof(char));
					if(inputEigenFile.rdstate() == stringstream::goodbit){
						cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
						inputEigenFile.close();
						exit(1);
					}
				}
				catch(...){
					cout << "Error: file with the eigenvalues and eigenvectors information is invalid." << endl;
					inputEigenFile.close();
					exit(1);
				}
			}
			else{
				cout << "Error: The file with the eigenvalues and eigenvectors information could not be open for reading." << endl;
				inputEigenFile.close();
				exit(1);
			}
		}
		else{
			cout << "Error: The file with the eigenvalues and eigenvectors information could not be open for reading." << endl;
			exit(1);
		}
	}

	return true;
}





void readTemperaturesInitFile(double* &Tinit, const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	if(configuration.fileNameInitialTemperatures.size() > 0){
		int numberUnits = floorplan.getNumberFunctionalUnits();
		int numberThermalNodes = NUMBER_BLOCK_LAYERS*numberUnits + EXTRA_NODES;
		double maxTemperature = 0;

		// Initialize the result vector with -1. At the end, if the initial temperature file is correct, all temperatures must be positive values. If not, the temperature file was invalid.
		for(int i = 0; i < numberThermalNodes; i++){
			Tinit[i] = -1;
		}

		ifstream inputTemperatureFile(configuration.fileNameInitialTemperatures.c_str());
		if ( inputTemperatureFile.is_open() ){
			if(inputTemperatureFile.good()){
				// Read until the end of the file
				while(inputTemperatureFile.good()){
					// Read one line from the file
					string line;
					getline(inputTemperatureFile, line);

					// If the line was successfully read
					if(inputTemperatureFile.good()){
						stringstream lineStream(line);
						lineStream.exceptions(stringstream::goodbit);

						string newThermalNodeName;
						lineStream >> newThermalNodeName;

						// If the line had a parameter
						if((lineStream.rdstate() == stringstream::goodbit) && (newThermalNodeName.size() > 0)){
							// Ignore comments
							if(newThermalNodeName[0] != '#'){
								// At this point, the string newThermalNodeName should hold a valid thermal node name according to the floorplan information
								// First find the layer
								int indexLayer = SILICON;
								if (newThermalNodeName.substr(0, strlen("iface_")) == "iface_"){
									indexLayer = IFACE;
									newThermalNodeName.erase(0, strlen("iface_"));
								}
								else if (newThermalNodeName.substr(0, strlen("hsp_")) == "hsp_"){
									indexLayer = HSP;
									newThermalNodeName.erase(0, strlen("hsp_"));
								}
								else if (newThermalNodeName.substr(0, strlen("hsink_")) == "hsink_"){
									indexLayer = HSINK;
									newThermalNodeName.erase(0, strlen("hsink_"));
								}
								else if (newThermalNodeName.substr(0, strlen("inode_")) == "inode_"){
									indexLayer = AMBIENT_NODES;
									newThermalNodeName.erase(0, strlen("inode_"));
								}

								// Then find out the node in the layer
								int indexUnit = -1;
								if(indexLayer < AMBIENT_NODES){
									for(int i = 0; i < numberUnits; i++){
										if(newThermalNodeName == floorplan.units[i].name){
											indexUnit = i;
											break;
										}
									}
								}
								else{
									stringstream numberStream(newThermalNodeName);
									int iValue;
									numberStream >> iValue;
									if((numberStream.rdstate() == stringstream::goodbit) || (numberStream.rdstate() == stringstream::eofbit)){
										if((iValue >= 0) && (iValue < EXTRA_NODES)){
											indexUnit = iValue;
										}
									}
								}

								// If the unit is valid, only then we read the temperature and assign it to the node
								if(indexUnit >= 0){
									double newTemperature;

									if((lineStream.rdstate() == stringstream::goodbit) || (lineStream.rdstate() == stringstream::eofbit)){
										int indexTemperature = numberUnits*indexLayer + indexUnit;
										lineStream >> newTemperature;
										if((indexTemperature >= 0) && (indexTemperature < numberThermalNodes)){
											Tinit[indexTemperature] = newTemperature;
											if(maxTemperature < newTemperature)
												maxTemperature = newTemperature;
										}
										else{
											cout << "Error: The file with the initial temperature information does not match the floorplan information." << endl;
											exit(1);
										}
									}
									else{
										cout << "Error: The file with the initial temperatures is invalid." << endl;
										exit(1);
									}
								}
								else{
									cout << "Error: The file with the initial temperature information does not match the floorplan information." << endl;
									exit(1);
								}
							}
						}
					}
				}
			}
			else{
				cout << "Error: The file with the initial temperature information could not be open for reading." << endl;
				exit(1);
			}
		}
		else{
			cout << "Error: The file with the initial temperature information could not be open for reading." << endl;
			exit(1);
		}




		// If DTM is used, the initial temperatures are scaled down in case the maximum temperature exceeds the threshold temperature.
		// \TODO //FIXME! Is this a reasonable policy??
		if((configuration.dtmUsed == true) && (maxTemperature > configuration.thermalThreshold)){
			double factor = (configuration.thermalThreshold - configuration.Tamb) / (maxTemperature - configuration.Tamb);

			for(unsigned int i = 0; i < numberThermalNodes; i++){
				Tinit[i] = factor*(Tinit[i] - configuration.Tamb) + configuration.Tamb;
			}
		}

		// Finally, check that all the temperatures are valid
		for(unsigned int i = 0; i < numberThermalNodes; i++){
			if(Tinit[i] < 0){
				cout << "Error: The file with the initial temperatures is invalid." << endl;
				exit(1);
			}
		}
	}
}







