#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace std;

#define SIZE 9
#define THERMAL_NODES 48
#define INT_CONVE 1000000
#define INT_CONVB 10000
#define INT_CONVG 100  // CONVE\CONVB
#define NUMBER_BLOCK_LAYERS 4
#define EXTRA_NODES 12
#define TAMB 31815	//kelvin
#define TKEV 27315

void computeSteadyStateTemperatures(unsigned int* &Tsteady, const int &numberUnits, const int &numberNodesAmbient, const int &numberThermalNodes, const unsigned int &Tamb, unsigned int* &Power, unsigned int** &Binv, unsigned int* &Gamb)
{
	int offsetGamb = numberThermalNodes - numberNodesAmbient;

	cout << "numberNodesAmbient: " << numberNodesAmbient << endl;
	cout << "offsetGamb: " << offsetGamb << endl;
	cout << "numberThermalNodes: " << numberThermalNodes << endl;

	for(int i = 0; i < numberThermalNodes; i++){ // numberThermalNodes

		unsigned int heatContributionPower = 0;
		for(int j = 0; j < numberUnits; j++){
			heatContributionPower += Binv[i][j]*Power[j];
			//cout << "Binv[" << i << "][" << j  << "]: " << Binv[i][j] << endl;
			//cout << "Power[" << j << "]: " << Power[j] << endl;
			//cout << "heatContributionPower: " << heatContributionPower << endl;
		}
		//if (i < 9) cout << "heatContributionPower " << i << " " << heatContributionPower/10000 << endl;

		unsigned int heatContributionAmbient = 0;
		for(int j = 0; j < numberNodesAmbient; j++){
			//heatContributionAmbient += Binv[i][j + offsetGamb]*Gamb[j];
			heatContributionAmbient = 1;
			//cout << "Binv[" << i << "][" << j + offsetGamb  << "]: " << Binv[i][j] << endl;
			//cout << "Gamb[" << j << "]: " << Gamb[j] << endl;
			//cout << "heatContributionAmbient: " << heatContributionAmbient << endl;
		}
		//cout << "heatContributionPower[" << i << "]: " << heatContributionPower << endl;
		//cout << "heatContributionAmbient: " << heatContributionAmbient << endl;
		Tsteady[i] = heatContributionPower/10000 + Tamb*heatContributionAmbient; //+
		unsigned int crivo = Tamb*heatContributionAmbient;
		//if (i < 9) cout << "Tsteady[" << i << "]: " << Tsteady[i] << endl;
	}
}

int main(int argc, char *argv[]){
	int i;
	ifstream inputBinv(argv[1]);
	ifstream inputGamb(argv[2]);
	ifstream inputCexp(argv[3]);

	int numberUnits = SIZE;
	int numberThermalNodes = NUMBER_BLOCK_LAYERS*numberUnits + EXTRA_NODES;
	int numberNodesAmbient = 0;

	unsigned int* Power = new unsigned int[numberUnits];
	unsigned int Trace[3][SIZE] = {{0, 0, 0, 94, 110, 0, 39, 42, 31}, {0, 0, 0, 102, 123, 0, 41, 45, 30}, {0, 0, 0, 105, 124, 0, 42, 45, 30}};
	unsigned int Tamb = TAMB;

	double** Binv;
	double** Cexp;
	double* Gamb;
	unsigned int** Binv_int;
	unsigned int** Cexp_int;
	unsigned int* Gamb_int;

	unsigned int* Tsteady = new unsigned int[numberThermalNodes];
	int* Tdifference = new int[numberThermalNodes];
	int* TempTraceEnd = new int[numberThermalNodes];

	ofstream outputMatrices;
	outputMatrices.open("matrices.h");

	Binv = new double*[numberThermalNodes];
	Binv_int = new unsigned int*[numberThermalNodes];
	Cexp = new double*[numberThermalNodes];
	Cexp_int = new unsigned int*[numberThermalNodes];
	for(i = 0; i < numberThermalNodes; i++){
		Binv[i] = new double[numberThermalNodes];
		Binv_int[i] = new unsigned int[numberThermalNodes];
		Cexp[i] = new double[numberThermalNodes];
		Cexp_int[i] = new unsigned int[numberThermalNodes];
	}
	Gamb = new double[numberThermalNodes];
	Gamb_int = new unsigned int[numberThermalNodes];

	outputMatrices << "int Binv[" << THERMAL_NODES << "][" << SIZE << "] = { ";
	unsigned int lineNumber = 0;
	while(inputBinv.good()){
		string lineBinv;
		getline(inputBinv, lineBinv);
		stringstream lineStreamBinv(lineBinv);
		if (lineNumber < THERMAL_NODES) outputMatrices << "{";
		unsigned int columnNumber = 0;
		while(lineStreamBinv.good()){
			double auxValue;
			int auxInt;
			lineStreamBinv >> auxValue;
			if (lineNumber < THERMAL_NODES && columnNumber < THERMAL_NODES){
				Binv[lineNumber][columnNumber] = auxValue;
				auxInt = int(auxValue*INT_CONVB);
				Binv_int[lineNumber][columnNumber] = auxInt;
				if (lineNumber < THERMAL_NODES){
					if (columnNumber < SIZE-1)	outputMatrices << Binv_int[lineNumber][columnNumber] << ", ";
					else if (columnNumber < SIZE) outputMatrices << Binv_int[lineNumber][columnNumber];
				}
				columnNumber++;
			}
		}
		if (lineNumber < THERMAL_NODES-1) outputMatrices << "}," << endl;
		else if (lineNumber < THERMAL_NODES) outputMatrices << "}" << endl;
		lineNumber++;
	}
	outputMatrices << "};" << endl << endl;

	string lineGamb;
	getline(inputGamb, lineGamb);
	stringstream lineStreamGamb(lineGamb);

	lineNumber = 0;
	while(lineStreamGamb.good()){
		double auxValue;
		unsigned int auxInt;
		lineStreamGamb >> auxValue;

		if (lineNumber < THERMAL_NODES) {
			auxInt = int(auxValue*INT_CONVG);
			Gamb[lineNumber] = auxValue;
			Gamb_int[lineNumber] = auxInt;
			//cout << "Gamb[" << lineNumber << "]: " << auxValue << " -> " << Gamb_int[lineNumber] << endl;
		}
		lineNumber++;
	}
	numberNodesAmbient = lineNumber - 1;
	cout << "numberNodesAmbient " << numberNodesAmbient << endl;

	outputMatrices << "int Cexp[" << THERMAL_NODES-12 << "][" << THERMAL_NODES << "] = { ";
	lineNumber = 0;
	while(inputCexp.good()){
		string lineCexp;
		getline(inputCexp, lineCexp);
		stringstream lineStreamCexp(lineCexp);
		if (lineNumber < THERMAL_NODES-12) outputMatrices << "{";
		unsigned int columnNumber = 0;
		while(lineStreamCexp.good()){
			double auxValue;
			int auxInt;
			lineStreamCexp >> auxValue;
			if (lineNumber < THERMAL_NODES && columnNumber < THERMAL_NODES){
				Cexp[lineNumber][columnNumber] = auxValue;
				auxInt = int(auxValue*INT_CONVE);
				Cexp_int[lineNumber][columnNumber] = auxInt;
				//cout << "Cexp[" << lineNumber << "][" << columnNumber << "]: " << auxValue << " -> " << Cexp_int[lineNumber][columnNumber] << endl;
				if (lineNumber < THERMAL_NODES-12){
					if (columnNumber < THERMAL_NODES-1)	outputMatrices << Cexp_int[lineNumber][columnNumber] << ", ";
					else if (columnNumber < THERMAL_NODES) outputMatrices << Cexp_int[lineNumber][columnNumber];
				}
				columnNumber++;
			}
		}
		if (lineNumber < THERMAL_NODES-13) outputMatrices << "}," << endl;
		else if (lineNumber < THERMAL_NODES-12) outputMatrices << "}" << endl;
		lineNumber++;
	}
	outputMatrices << "};" << endl << endl;

	for (i = 0; i < numberThermalNodes; i++)
		TempTraceEnd[i] = Tamb;
	
	for (i = 0; i < 3; i++){

		for (int k = 0; k < numberUnits; k++){
			Power[k] = Trace[i][k];
		}

		computeSteadyStateTemperatures(Tsteady, numberUnits, numberNodesAmbient, numberThermalNodes, Tamb, Power, Binv_int, Gamb_int);

		for(int k = 0; k < numberThermalNodes; k++){
			Tdifference[k] = (int)(TempTraceEnd[k] - Tsteady[k]);
			if (k == 0) cout << "Tsteady[" << k << "] " << i+1 << ": " << Tsteady[k] << "\t";
			else if (k < 9) cout << Tsteady[k] << "\t";
		}
		cout << endl;

		for(int k = 0; k < numberThermalNodes-12; k++){
			int sumExponentials = 0;
			for(int j = 0; j < numberThermalNodes; j++){
				sumExponentials += Cexp_int[k][j] * (Tdifference[j]/INT_CONVG); //+
			}
			TempTraceEnd[k] = Tsteady[k] + sumExponentials/INT_CONVB; //-
			if (k < 9) cout << "sumExponentials[" << k << "]: " << sumExponentials << endl;
			//if (k == 0) cout << "TempTraceEnd[" << k << "] " << i+1 << ": " << TempTraceEnd[k] - TKEV << "\t";
			//else if (k < 9) cout << TempTraceEnd[k] - TKEV << "\t";
			//if (k < 16) cout << "Tdifference[" << k << "]: " << Tdifference[k] << endl;
		}
		cout << endl;
	}

	delete[] Cexp;
	delete[] Binv;
	delete[] Gamb;
	delete[] Gamb_int;

	inputBinv.close();
	inputGamb.close();
	inputCexp.close();
}