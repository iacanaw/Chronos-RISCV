/**	\file	floorplan.cpp
 *	\brief	Source file for floorplan class.
 */

#include "floorplan.h"




bool FunctionalUnit::load(const string &inputLine)
{
	try{
		string unitName;
		double dValue;
		stringstream streamValue(inputLine);
		streamValue.exceptions(stringstream::goodbit);

		streamValue >> unitName;

		if(unitName.size() > 0){
			if(unitName[0] != '#'){
				name = unitName;

				streamValue >> width;
				streamValue >> height;
				streamValue >> left;
				streamValue >> bottom;

				// If the stream has no error, then the unit was properly read
				if(streamValue.rdstate() == stringstream::eofbit)
					return true;
				else
					return false;
			}
		}

		return false;
	}
	catch(...){
		cout << "Error: Floorplan is invalid. Error found while reading line \"" << inputLine << "\"." << endl;
		return false;
	}
}



double FunctionalUnit::getArea(void)
{
	return width * height;
}



bool FunctionalUnit::isValid(void)
{
	if((width * height) > 0)
		return true;
	else
		return false;
}



FunctionalUnit::FunctionalUnit()
{
	name = "";
	width = 0;
	height = 0;
	left = 0;
	bottom = 0;
}









bool WireDensityRaw::load(const string &inputLine)
{
	try{
		string unitName;
		double dValue;
		stringstream streamValue(inputLine);
		streamValue.exceptions(stringstream::goodbit);

		streamValue >> unitName;

		if(unitName.size() > 0){
			if(unitName[0] != '#'){
				nameFrom = unitName;

				streamValue >> unitName;
				nameTo = unitName;

				streamValue >> dValue;
				density = dValue;

				// If the stream has no error, then the unit was properly read
				if(streamValue.rdstate() == stringstream::eofbit)
					return true;
				else
					return false;
			}
		}

		return false;
	}
	catch(...){
		cout << "Error: Floorplan is invalid. Error found while reading line \"" << inputLine << "\"." << endl;
		return false;
	}
}





WireDensityRaw::WireDensityRaw()
{
	nameFrom = "";
	nameTo = "";
	density = 0;
}









bool Floorplan::readFloorplan(const string &floorplanFileName, bool const &readConnectivity)
{
	units.clear();


	vector< WireDensityRaw > wireDensitiesRaw;

	if(floorplanFileName.size() > 0){
		ifstream inputFloorplanFile(floorplanFileName.c_str());
		if ( inputFloorplanFile.is_open() ){
			if(inputFloorplanFile.good()){
				// Read until the end of the file
				while(inputFloorplanFile.good()){
					// Read one line from the file
					string line;
					getline(inputFloorplanFile, line);

					// If the line was successfully read
					if(inputFloorplanFile.good()){
						// If the line is not a comment
						if(line.size() > 0){
							if(line[0] != '#'){
								FunctionalUnit auxFunctionalUnit;
								if(auxFunctionalUnit.load(line)){
									units.push_back(auxFunctionalUnit);
								}
								else{
									WireDensityRaw auxWireDensity;
									if(auxWireDensity.load(line)){
										if(readConnectivity)
											wireDensitiesRaw.push_back(auxWireDensity);
									}
									else{
										cout << "Error: Floorplan is invalid. Error found while reading line \"" << line << "\"." << endl;
										exit(1);
									}
								}
							}
						}
					}
				}
				// The floorplan file is read, so close it
				inputFloorplanFile.close();



				// Once the file was complete read, we fill the matrix with the wire densities
				// If there was no wire density information in the floorplan, we take the default density of 1.
				if((readConnectivity == true) && (wireDensitiesRaw.size() > 0)){
					// We start with a zero wire density among every block
					wireDensities = Matrix<double, Dynamic, Dynamic>::Zero(units.size(), units.size());
					for(unsigned int i = 0; i < wireDensitiesRaw.size(); i++){
						int indexFrom = getBlockIndex(wireDensitiesRaw[i].nameFrom);
						int indexTo = getBlockIndex(wireDensitiesRaw[i].nameTo);

						if((indexFrom >= 0) && ((indexTo >= 0))){
							if(indexFrom == indexTo){
								cout << "Error: Floorplan is invalid. Block \"" << wireDensitiesRaw[i].nameFrom << "\" is connected to itself." << endl;
								exit(1);
							}

							// For the floorplan to be valid, the wire density from A->B has to be the same as for B->A. Therefore, the floorplan only needs wire densities from A->B to be specified.
							// If wire densities from B->A are also specified, they must match the previous values, or the floorplan is considered as invalid.
							if((wireDensities(indexFrom, indexTo) == 0) && (wireDensities(indexTo, indexFrom) == 0)){
								wireDensities(indexFrom, indexTo) = wireDensitiesRaw[i].density;
								wireDensities(indexTo, indexFrom) = wireDensitiesRaw[i].density;
							}
							else if((wireDensities(indexFrom, indexTo) != wireDensitiesRaw[i].density) || (wireDensities(indexTo, indexFrom) != wireDensitiesRaw[i].density)){
								cout << "Error: Floorplan is invalid. Connectivity information from block \"" << wireDensitiesRaw[i].nameFrom << "\" to block \"" << wireDensitiesRaw[i].nameTo << "\" is different than from block \"" << wireDensitiesRaw[i].nameTo << "\" to block \"" << wireDensitiesRaw[i].nameFrom << "\"." << endl;
								exit(1);
							}
						}
						else{
							if(indexFrom < 0)
								cout << "Error: Floorplan is invalid. Block \"" << wireDensitiesRaw[i].nameFrom << "\" is not a valid block and it cannot be connected to block \"" << wireDensitiesRaw[i].nameTo << "\"." << endl;
							if(indexTo < 0)
								cout << "Error: Floorplan is invalid. Block \"" << wireDensitiesRaw[i].nameTo << "\" is not a valid block and it cannot be connected to block \"" << wireDensitiesRaw[i].nameFrom << "\"." << endl;
							exit(1);
						}
					}
				}
				else{
					wireDensities = Matrix<double, Dynamic, Dynamic>::Constant(units.size(), units.size(), 1.0);
				}

				// Finally, we translate the floorplan to the origin (0,0), if it was not already the case
				translate(0,0);


				if(units.size() > 0)
					return true;
				else
					return false;
			}
			else{
				cout << "Error: Floorplan file could not be open for reading." << endl;
				inputFloorplanFile.close();
				exit(1);
			}
		}
		else{
			cout << "Error: Floorplan file could not be open for reading." << endl;
			exit(1);
		}
	}
	else{
		cout << "Error: Required floorplan file missing." << endl;
		exit(1);
	}
}








int Floorplan::getBlockIndex(const string &blockName)
{
	for(unsigned int i = 0; i < units.size(); i++){
		if(blockName == units[i].name)
			return (int)i;
	}

	return -1;
}







void Floorplan::translate(const double &newLeftFloorplan, const double &newBottomFloorplan)
{
	if(units.size() > 0){
		double leftMostCoordinate = units[0].left;
		double bottomMostCoordinate = units[0].bottom;

		for(unsigned int i = 1; i < units.size(); i++){
			if(leftMostCoordinate > units[i].left)
				leftMostCoordinate = units[i].left;
			if(bottomMostCoordinate > units[i].bottom)
				bottomMostCoordinate = units[i].bottom;
		}

		if(leftMostCoordinate != newLeftFloorplan){
			for(unsigned int i = 0; i < units.size(); i++){
				units[i].left +=  newLeftFloorplan - leftMostCoordinate;
			}
		}

		if(bottomMostCoordinate != newBottomFloorplan){
			for(unsigned int i = 0; i < units.size(); i++){
				units[i].bottom +=  newBottomFloorplan - bottomMostCoordinate;
			}
		}
	}
}





double Floorplan::getWidth(void)
{
	if(units.size() > 0){
		double leftMostCoordinate = units[0].left;
		double rightMostCoordinate = units[0].left + units[0].width;

		for(unsigned int i = 1; i < units.size(); i++){
			if(leftMostCoordinate > units[i].left)
				leftMostCoordinate = units[i].left;
			if(rightMostCoordinate < (units[i].left + units[i].width))
				rightMostCoordinate = units[i].left + units[i].width;
		}

		return rightMostCoordinate - leftMostCoordinate;
	}
	else
		return 0;
}



double Floorplan::getHeight(void)
{
	if(units.size() > 0){
		double bottomMostCoordinate = units[0].bottom;
		double topMostCoordinate = units[0].bottom + units[0].height;

		for(unsigned int i = 1; i < units.size(); i++){
			if(bottomMostCoordinate > units[i].bottom)
				bottomMostCoordinate = units[i].bottom;
			if(topMostCoordinate < (units[i].bottom + units[i].height))
				topMostCoordinate = units[i].bottom + units[i].height;
		}

		return topMostCoordinate - bottomMostCoordinate;
	}
	else
		return 0;
}





double Floorplan::getSharedLength(const int &unitA, const int &unitB)
{
	if( (unitA == unitB) ||
		(unitA < 0) ||
		(unitB < 0) ||
		(unitA >= units.size()) ||
		(unitB >= units.size())){
		return 0;
	}

	double p11, p12, p21, p22;
	p11 = p12 = p21 = p22 = 0.0;


	if (isHorizontalAdjacent(unitA, unitB)) {
		p11 = units[unitA].bottom;
		p12 = p11 + units[unitA].height;
		p21 = units[unitB].bottom;
		p22 = p21 + units[unitB].height;
	}

	if (isVerticallyAdjacent(unitA, unitB)) {
		p11 = units[unitA].left;
		p12 = p11 + units[unitA].width;
		p21 = units[unitB].left;
		p22 = p21 + units[unitB].width;
	}

	return (minimum(p12, p22) - maximum(p11, p21));
}










bool Floorplan::isHorizontalAdjacent(const int &unitA, const int &unitB)
{
	if( (unitA == unitB) ||
		(unitA < 0) ||
		(unitB < 0) ||
		(unitA >= units.size()) ||
		(unitB >= units.size())){
		return false;
	}

	double x1, x2, x3, x4;
	double y1, y2, y3, y4;

	x1 = units[unitA].left;
	x2 = x1 + units[unitA].width;
	x3 = units[unitB].left;
	x4 = x3 + units[unitB].width;

	y1 = units[unitA].bottom;
	y2 = y1 + units[unitA].height;
	y3 = units[unitB].bottom;
	y4 = y3 + units[unitB].height;

	/* diagonally adjacent => not adjacent */
	if (equalTo(x2,x3) && equalTo(y2,y3))
		return false;
	if (equalTo(x1,x4) && equalTo(y1,y4))
		return false;
	if (equalTo(x2,x3) && equalTo(y1,y4))
		return false;
	if (equalTo(x1,x4) && equalTo(y2,y3))
		return false;

	if (equalTo(x1,x4) || equalTo(x2,x3))
		if ((y3 >= y1 && y3 <= y2) || (y4 >= y1 && y4 <= y2) ||
		    (y1 >= y3 && y1 <= y4) || (y2 >= y3 && y2 <= y4))
			return true;

	return false;
}




bool Floorplan::isVerticallyAdjacent(const int &unitA, const int &unitB)
{
	if( (unitA == unitB) ||
		(unitA < 0) ||
		(unitB < 0) ||
		(unitA >= units.size()) ||
		(unitB >= units.size())){
		return false;
	}

	double x1, x2, x3, x4;
	double y1, y2, y3, y4;

	x1 = units[unitA].left;
	x2 = x1 + units[unitA].width;
	x3 = units[unitB].left;
	x4 = x3 + units[unitB].width;

	y1 = units[unitA].bottom;
	y2 = y1 + units[unitA].height;
	y3 = units[unitB].bottom;
	y4 = y3 + units[unitB].height;

	/* diagonally adjacent => not adjacent */
	if (equalTo(x2,x3) && equalTo(y2,y3))
		return false;
	if (equalTo(x1,x4) && equalTo(y1,y4))
		return false;
	if (equalTo(x2,x3) && equalTo(y1,y4))
		return false;
	if (equalTo(x1,x4) && equalTo(y2,y3))
		return false;

	if (equalTo(y1,y4) || equalTo(y2,y3))
		if ((x3 >= x1 && x3 <= x2) || (x4 >= x1 && x4 <= x2) ||
		    (x1 >= x3 && x1 <= x4) || (x2 >= x3 && x2 <= x4))
			return true;

	return false;
}





void Floorplan::print(void)
{
	cout << "#Floorplan information:" << endl;
	if(units.size() == 0){
		cout << "\tNo functional units in the floorplan" << endl;
	}
	else{
		cout << "#Functional Units:" << endl;
		cout << "#Unit\tArea\tWidth\tHeight\tLeft\tBottom\tRight\tTop:" << endl;
		for(unsigned int i = 0; i < units.size(); i++){
			cout << "\t" << units[i].name << "\t\t" << units[i].width << "\t\t" << units[i].height << "\t\t" << units[i].left << "\t\t" << units[i].bottom << "\t\t" << units[i].left + units[i].width << "\t\t" << units[i].bottom + units[i].height << endl;
		}

		cout << "#Connectivity information:" << endl;
		for(unsigned int i = 0; i < wireDensities.rows(); i++){
			for(unsigned int j = 0; j < wireDensities.cols(); j++){
				cout << "\tFrom:" << i+1 << "\tTo:" << j+1 << "\tDensity:" << wireDensities(i,j) << endl;
			}
		}
	}
}




