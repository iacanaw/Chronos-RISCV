/**	\file	rcModel.cpp
 *	\brief	Source file for RC thermal network class.
 */

#include "rcModel.h"









void Package::loadR(const ConfigurationParameters &configuration, const double &floorplanWidth, const double &floorplanHeight)
{
	/* Lateral resistances of spreader and sink */
	r_sp1_x = getThermalResistance(configuration.kSpreader, (configuration.sSpreader - floorplanWidth)/4.0, (configuration.sSpreader + 3*floorplanHeight)/4.0 * configuration.tSpreader);
	r_sp1_y = getThermalResistance(configuration.kSpreader, (configuration.sSpreader - floorplanHeight)/4.0, (configuration.sSpreader + 3*floorplanWidth)/4.0 * configuration.tSpreader);
	r_hs1_x = getThermalResistance(configuration.kSink, (configuration.sSpreader - floorplanWidth)/4.0, (configuration.sSpreader + 3*floorplanHeight)/4.0 * configuration.tSink);
	r_hs1_y = getThermalResistance(configuration.kSink, (configuration.sSpreader - floorplanHeight)/4.0, (configuration.sSpreader + 3*floorplanWidth)/4.0 * configuration.tSink);
	r_hs2_x = getThermalResistance(configuration.kSink, (configuration.sSpreader - floorplanWidth)/4.0, (3*configuration.sSpreader + floorplanHeight)/4.0 * configuration.tSink);
	r_hs2_y = getThermalResistance(configuration.kSink, (configuration.sSpreader - floorplanHeight)/4.0, (3*configuration.sSpreader + floorplanWidth)/4.0 * configuration.tSink);
	r_hs = getThermalResistance(configuration.kSink, (configuration.sSink - configuration.sSpreader)/4.0, (configuration.sSink+3*configuration.sSpreader)/4.0 * configuration.tSink);

	/* Vertical resistances of spreader and sink */
	r_sp_per_x = getThermalResistance(configuration.kSpreader, configuration.tSpreader, (configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	r_sp_per_y = getThermalResistance(configuration.kSpreader, configuration.tSpreader, (configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	r_hs_c_per_x = getThermalResistance(configuration.kSink, configuration.tSink, (configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	r_hs_c_per_y = getThermalResistance(configuration.kSink, configuration.tSink, (configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	r_hs_per = getThermalResistance(configuration.kSink, configuration.tSink, (configuration.sSink*configuration.sSink - configuration.sSpreader*configuration.sSpreader) / 4.0);

	/* Vertical resistances to ambient (divide r_convec proportional to area) */
	r_amb_c_per_x = configuration.rConvec * (configuration.sSink * configuration.sSink) / ((configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	r_amb_c_per_y = configuration.rConvec * (configuration.sSink * configuration.sSink) / ((configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	r_amb_per = configuration.rConvec * (configuration.sSink * configuration.sSink) / ((configuration.sSink*configuration.sSink - configuration.sSpreader*configuration.sSpreader) / 4.0);

	/* Lateral resistances of package substrate, solder and PCB */
	r_sub1_x = getThermalResistance(K_SUB, (configuration.sSub - floorplanWidth)/4.0, (configuration.sSub + 3*floorplanHeight)/4.0 * configuration.tSub);
	r_sub1_y = getThermalResistance(K_SUB, (configuration.sSub - floorplanHeight)/4.0, (configuration.sSub + 3*floorplanWidth)/4.0 * configuration.tSub);
	r_solder1_x = getThermalResistance(K_SOLDER, (configuration.sSolder - floorplanWidth)/4.0, (configuration.sSolder + 3*floorplanHeight)/4.0 * configuration.tSolder);
	r_solder1_y = getThermalResistance(K_SOLDER, (configuration.sSolder - floorplanHeight)/4.0, (configuration.sSolder + 3*floorplanWidth)/4.0 * configuration.tSolder);
	r_pcb1_x = getThermalResistance(K_PCB, (configuration.sSolder - floorplanWidth)/4.0, (configuration.sSolder + 3*floorplanHeight)/4.0 * configuration.tPcb);
	r_pcb1_y = getThermalResistance(K_PCB, (configuration.sSolder - floorplanHeight)/4.0, (configuration.sSolder + 3*floorplanWidth)/4.0 * configuration.tPcb);
	r_pcb2_x = getThermalResistance(K_PCB, (configuration.sSolder - floorplanWidth)/4.0, (3*configuration.sSolder + floorplanHeight)/4.0 * configuration.tPcb);
	r_pcb2_y = getThermalResistance(K_PCB, (configuration.sSolder - floorplanHeight)/4.0, (3*configuration.sSolder + floorplanWidth)/4.0 * configuration.tPcb);
	r_pcb = getThermalResistance(K_PCB, (configuration.sPcb - configuration.sSolder)/4.0, (configuration.sPcb + 3*configuration.sSolder)/4.0 * configuration.tPcb);

	/* Vertical resistances of package substrate, solder balls and PCB */
	r_sub_per_x = getThermalResistance(K_SUB, configuration.tSub, (configuration.sSub + floorplanHeight) * (configuration.sSub - floorplanWidth) / 4.0);
	r_sub_per_y = getThermalResistance(K_SUB, configuration.tSub, (configuration.sSub + floorplanWidth) * (configuration.sSub - floorplanHeight) / 4.0);
	r_solder_per_x = getThermalResistance(K_SOLDER, configuration.tSolder, (configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	r_solder_per_y = getThermalResistance(K_SOLDER, configuration.tSolder, (configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	r_pcb_c_per_x = getThermalResistance(K_PCB, configuration.tPcb, (configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	r_pcb_c_per_y = getThermalResistance(K_PCB, configuration.tPcb, (configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	r_pcb_per = getThermalResistance(K_PCB, configuration.tPcb, (configuration.sPcb*configuration.sPcb - configuration.sSolder*configuration.sSolder) / 4.0);

	/* Vertical resistances to ambient at PCB (divide r_convec_sec proportional to area) */
	r_amb_sec_c_per_x = configuration.rConvecSec * (configuration.sPcb * configuration.sPcb) / ((configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	r_amb_sec_c_per_y = configuration.rConvecSec * (configuration.sPcb * configuration.sPcb) / ((configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	r_amb_sec_per = configuration.rConvecSec * (configuration.sPcb * configuration.sPcb) / ((configuration.sPcb*configuration.sPcb - configuration.sSolder*configuration.sSolder) / 4.0);
}




void Package::loadC(const ConfigurationParameters &configuration, const double &floorplanWidth, const double &floorplanHeight)
{
	/* Vertical C's of spreader and sink */
	c_sp_per_x = getThermalCapacitance(configuration.pSpreader, configuration.tSpreader, (configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	c_sp_per_y = getThermalCapacitance(configuration.pSpreader, configuration.tSpreader, (configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	c_hs_c_per_x = getThermalCapacitance(configuration.pSink, configuration.tSink, (configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	c_hs_c_per_y = getThermalCapacitance(configuration.pSink, configuration.tSink, (configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	c_hs_per = getThermalCapacitance(configuration.pSink, configuration.tSink, (configuration.sSink*configuration.sSink - configuration.sSpreader*configuration.sSpreader) / 4.0);

	/* Vertical C's to ambient (divide configuration.cConvec proportional to area) */
	c_amb_c_per_x = C_FACTOR * configuration.cConvec / (configuration.sSink * configuration.sSink) * ((configuration.sSpreader + floorplanHeight) * (configuration.sSpreader - floorplanWidth) / 4.0);
	c_amb_c_per_y = C_FACTOR * configuration.cConvec / (configuration.sSink * configuration.sSink) * ((configuration.sSpreader + floorplanWidth) * (configuration.sSpreader - floorplanHeight) / 4.0);
	c_amb_per = C_FACTOR * configuration.cConvec / (configuration.sSink * configuration.sSink) * ((configuration.sSink*configuration.sSink - configuration.sSpreader*configuration.sSpreader) / 4.0);

	/* Vertical C's of package substrate, solder balls, and PCB */
	c_sub_per_x = getThermalCapacitance(SPEC_HEAT_SUB, configuration.tSub, (configuration.sSub + floorplanHeight) * (configuration.sSub - floorplanWidth) / 4.0);
	c_sub_per_y = getThermalCapacitance(SPEC_HEAT_SUB, configuration.tSub, (configuration.sSub + floorplanWidth) * (configuration.sSub - floorplanHeight) / 4.0);
	c_solder_per_x = getThermalCapacitance(SPEC_HEAT_SOLDER, configuration.tSolder, (configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	c_solder_per_y = getThermalCapacitance(SPEC_HEAT_SOLDER, configuration.tSolder, (configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	c_pcb_c_per_x = getThermalCapacitance(SPEC_HEAT_PCB, configuration.tPcb, (configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	c_pcb_c_per_y = getThermalCapacitance(SPEC_HEAT_PCB, configuration.tPcb, (configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	c_pcb_per = getThermalCapacitance(SPEC_HEAT_PCB, configuration.tPcb, (configuration.sPcb*configuration.sPcb - configuration.sSolder*configuration.sSolder) / 4.0);

	/* Vertical C's to ambient at PCB (divide configuration.cConvec_sec proportional to area) */
	c_amb_sec_c_per_x = C_FACTOR * configuration.cConvecSec / (configuration.sPcb * configuration.sPcb) * ((configuration.sSolder + floorplanHeight) * (configuration.sSolder - floorplanWidth) / 4.0);
	c_amb_sec_c_per_y = C_FACTOR * configuration.cConvecSec / (configuration.sPcb * configuration.sPcb) * ((configuration.sSolder + floorplanWidth) * (configuration.sSolder - floorplanHeight) / 4.0);
	c_amb_sec_per = C_FACTOR * configuration.cConvecSec / (configuration.sPcb * configuration.sPcb) * ((configuration.sPcb*configuration.sPcb - configuration.sSolder*configuration.sSolder) / 4.0);
}










RCmodel::RCmodel()
{
	numberUnits = 0;
	numberNodesAmbient = 0;
	numberThermalNodes = 0;
	numberGridLayers = 0;
}



bool RCmodel::load(const ConfigurationParameters &configuration, Floorplan &floorplan, const bool &doTransient)
{
	if(configuration.modelType == ConfigurationParameters::BLOCK){
		if(loadRblock(configuration, floorplan)){
			if(doTransient){
				if(loadCblock(configuration, floorplan)){
					return true;
				}
				else{
					return false;
				}
			}
			else{
				return true;
			}
		}
		else{
			return false;
		}
	}
	else if(configuration.modelType == ConfigurationParameters::GRID){
		if(loadRgrid(configuration, floorplan)){
			if(doTransient){
				if(loadCgrid(configuration, floorplan)){
					return true;
				}
				else{
					return false;
				}
			}
			else{
				return true;
			}
		}
		else{
			return false;
		}
	}

	return false;
}




bool RCmodel::loadRblock(const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	if(floorplan.getNumberFunctionalUnits() <= 0){
		return false;
	}

	numberUnits = floorplan.getNumberFunctionalUnits();
	numberNodesAmbient = numberUnits + EXTRA_NODES;
	numberThermalNodes = NUMBER_BLOCK_LAYERS*numberUnits + EXTRA_NODES;

	double floorplanWidth = floorplan.getWidth();
	double floorplanHeight = floorplan.getHeight();

	/*	Sanity check on floorplan sizes	*/
	if ( (floorplanWidth > configuration.sSink) ||
		 (floorplanHeight > configuration.sSink) ||
		 (floorplanWidth > configuration.sSpreader) ||
		 (floorplanHeight > configuration.sSpreader)){

		floorplan.print();
		cout << "Error: Inordinate floorplan size." << endl;
		exit(1);
	}

	Gx.resize(numberUnits);
	Gy.resize(numberUnits);
	GxInterface.resize(numberUnits);
	GyInterface.resize(numberUnits);
	GxSpreader.resize(numberUnits);
	GySpreader.resize(numberUnits);
	GxHeatSink.resize(numberUnits);
	GyHeatSink.resize(numberUnits);
	LenghtSharedEdge.resize(numberUnits,numberUnits);

	B.resize(numberThermalNodes,numberThermalNodes);
	C.resize(numberThermalNodes,numberThermalNodes);
	G = Matrix<double, Dynamic, 1>::Zero(numberThermalNodes);
	Gamb = Matrix<double, Dynamic, 1>::Zero(numberUnits + EXTRA_NODES);
	conductances = Matrix<double, Dynamic, Dynamic>::Zero(numberThermalNodes,numberThermalNodes);

	border.resize(numberUnits, 4);


	/* Gx's and Gy's of blocks	*/
	for (int i = 0; i < numberUnits; i++) {
		/* At the silicon layer	*/
		if (configuration.blockOmitLateral) {
			Gx(i) = 0;
			Gy(i) = 0;
		}
		else {
			Gx(i) = 1.0/getThermalResistance(configuration.kChip, floorplan.units[i].width / 2.0, floorplan.units[i].height * configuration.tChip);
			Gy(i) = 1.0/getThermalResistance(configuration.kChip, floorplan.units[i].height / 2.0, floorplan.units[i].width * configuration.tChip);
		}

		/* At the interface layer	*/
		GxInterface(i) = 1.0/getThermalResistance(configuration.kInterface, floorplan.units[i].width / 2.0, floorplan.units[i].height * configuration.tInterface);
		GyInterface(i) = 1.0/getThermalResistance(configuration.kInterface, floorplan.units[i].height / 2.0, floorplan.units[i].width * configuration.tInterface);

		/* At the spreader layer	*/
		GxSpreader(i) = 1.0/getThermalResistance(configuration.kSpreader, floorplan.units[i].width / 2.0, floorplan.units[i].height * configuration.tSpreader);
		GySpreader(i) = 1.0/getThermalResistance(configuration.kSpreader, floorplan.units[i].height / 2.0, floorplan.units[i].width * configuration.tSpreader);

		/* At the heat sink layer	*/
		GxHeatSink(i) = 1.0/getThermalResistance(configuration.kSink, floorplan.units[i].width / 2.0, floorplan.units[i].height * configuration.tSink);
		GyHeatSink(i) = 1.0/getThermalResistance(configuration.kSink, floorplan.units[i].height / 2.0, floorplan.units[i].width * configuration.tSink);
	}


	/* Shared lengths between blocks	*/
	for (int i = 0; i < numberUnits; i++){
		for (int j = i; j < numberUnits; j++){
			LenghtSharedEdge(i,j) = floorplan.getSharedLength(i,j);
			LenghtSharedEdge(j,i) = LenghtSharedEdge(i,j);
		}
	}


	/* Load the information of the package	*/
	package.loadR(configuration, floorplanWidth, floorplanHeight);







	double gn_sp = 0, gs_sp = 0, ge_sp = 0, gw_sp = 0;
	double gn_hs = 0, gs_hs = 0, ge_hs = 0, gw_hs = 0;




	/* short the R's from block centers to a particular chip edge	*/
	for (int i = 0; i < numberUnits; i++) {
		if (equalTo(floorplan.units[i].bottom + floorplan.units[i].height, floorplanHeight)) {
			gn_sp += GySpreader(i);
			gn_hs += GyHeatSink(i);
			border(i,2) = 1;	/* block is on northern border 	*/
		} else
			border(i,2) = 0;

		if (equalTo(floorplan.units[i].bottom, 0)) {
			gs_sp += GySpreader(i);
			gs_hs += GyHeatSink(i);
			border(i,3) = 1;	/* block is on southern border	*/
		} else
			border(i,3) = 0;

		if (equalTo(floorplan.units[i].left + floorplan.units[i].width, floorplanWidth)) {
			ge_sp += GxSpreader(i);
			ge_hs += GxHeatSink(i);
			border(i,1) = 1;	/* block is on eastern border	*/
		} else
			border(i,1) = 0;

		if (equalTo(floorplan.units[i].left, 0)) {
			gw_sp += GxSpreader(i);
			gw_hs += GxHeatSink(i);
			border(i,0) = 1;	/* block is on western border	*/
		} else
			border(i,0) = 0;
	}



	/* Overall Rs between nodes */
	for (int i = 0; i < numberUnits; i++) {
		double area = floorplan.units[i].getArea();
		/* Amongst functional units	in the various layers	*/
		for (int j = 0; j < numberUnits; j++) {
			double part = 0, part_int = 0, part_sp = 0, part_hs = 0;
			if (floorplan.isHorizontalAdjacent(i, j)) {
				part = Gx(i) / floorplan.units[i].height;
				part_int = GxInterface(i) / floorplan.units[i].height;
				part_sp = GxSpreader(i) / floorplan.units[i].height;
				part_hs = GxHeatSink(i) / floorplan.units[i].height;
			}
			else if (floorplan.isVerticallyAdjacent(i, j))  {
				part = Gy(i) / floorplan.units[i].width;
				part_int = GyInterface(i) / floorplan.units[i].width;
				part_sp = GySpreader(i) / floorplan.units[i].width;
				part_hs = GyHeatSink(i) / floorplan.units[i].width;
			}
			conductances(i, j) = part * LenghtSharedEdge(i, j);
			conductances(IFACE*numberUnits + i, IFACE*numberUnits + j) = part_int * LenghtSharedEdge(i,j);
			conductances(HSP*numberUnits + i, HSP*numberUnits + j) = part_sp * LenghtSharedEdge(i,j);
			conductances(HSINK*numberUnits + i, HSINK*numberUnits + j) = part_hs * LenghtSharedEdge(i,j);
		}
		/* The 2.0 factor in the following equations is explained during the calculation of the B matrix	*/
 		/* Vertical g's in the silicon layer	*/
		conductances(i, IFACE*numberUnits + i) = conductances(IFACE*numberUnits + i, i) = 2.0/getThermalResistance(configuration.kChip, configuration.tChip, area);
 		/* Vertical g's in the interface layer	*/
		conductances(IFACE*numberUnits + i, HSP*numberUnits + i) = conductances(HSP*numberUnits + i, IFACE*numberUnits + i) = 2.0/getThermalResistance(configuration.kInterface, configuration.tInterface, area);
		/* Vertical g's in the spreader layer	*/
		conductances(HSP*numberUnits + i, HSINK*numberUnits + i) = conductances(HSINK*numberUnits + i, HSP*numberUnits + i) = 2.0/getThermalResistance(configuration.kSpreader, configuration.tSpreader, area);
		/* Vertical g's in the heatsink core layer	*/
		/* Vertical R to ambient: divide r_convec proportional to area	*/
		double r_amb = configuration.rConvec * (configuration.sSink * configuration.sSink) / area;
		Gamb(i) = 1.0 / (getThermalResistance(configuration.kSink, configuration.tSink, area) + r_amb);

		/* lateral g's from block center (spreader layer) to peripheral (n,s,e,w) spreader nodes	*/
		conductances(HSP*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SP_N) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_N, HSP*numberUnits + i) = 2.0*border(i,2) / ((1.0/GySpreader(i)) + package.r_sp1_y*gn_sp/GySpreader(i));
		conductances(HSP*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SP_S) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_S, HSP*numberUnits + i) = 2.0*border(i,3) / ((1.0/GySpreader(i)) + package.r_sp1_y*gs_sp/GySpreader(i));
		conductances(HSP*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SP_E) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_E, HSP*numberUnits + i) = 2.0*border(i,1) / ((1.0/GxSpreader(i)) + package.r_sp1_x*ge_sp/GxSpreader(i));
		conductances(HSP*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SP_W) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_W, HSP*numberUnits + i) = 2.0*border(i,0) / ((1.0/GxSpreader(i)) + package.r_sp1_x*gw_sp/GxSpreader(i));

		/* lateral g's from block center (heatsink layer) to peripheral (n,s,e,w) heatsink nodes	*/
		conductances(HSINK*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N, HSINK*numberUnits + i) = 2.0*border(i,2) / ((1.0/GyHeatSink(i)) + package.r_hs1_y*gn_hs/GyHeatSink(i));
		conductances(HSINK*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S, HSINK*numberUnits + i) = 2.0*border(i,3) / ((1.0/GyHeatSink(i)) + package.r_hs1_y*gs_hs/GyHeatSink(i));
		conductances(HSINK*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E, HSINK*numberUnits + i) = 2.0*border(i,1) / ((1.0/GxHeatSink(i)) + package.r_hs1_x*ge_hs/GxHeatSink(i));
		conductances(HSINK*numberUnits + i, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W, HSINK*numberUnits + i) = 2.0*border(i,0) / ((1.0/GxHeatSink(i)) + package.r_hs1_x*gw_hs/GxHeatSink(i));
	}

	/* g's from peripheral(n,s,e,w) nodes	*/
	/* vertical g's between peripheral spreader nodes and center peripheral heatsink nodes */
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_N, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N, NUMBER_BLOCK_LAYERS*numberUnits + SP_N) = 2.0/package.r_sp_per_y;
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_S, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S, NUMBER_BLOCK_LAYERS*numberUnits + SP_S) = 2.0/package.r_sp_per_y;
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_E, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E, NUMBER_BLOCK_LAYERS*numberUnits + SP_E) = 2.0/package.r_sp_per_x;
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SP_W, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W, NUMBER_BLOCK_LAYERS*numberUnits + SP_W) = 2.0/package.r_sp_per_x;
	/* lateral g's between peripheral outer sink nodes and center peripheral sink nodes	*/
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N, NUMBER_BLOCK_LAYERS*numberUnits + SINK_N) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_N, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N) = 2.0/(package.r_hs + package.r_hs2_y);
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S, NUMBER_BLOCK_LAYERS*numberUnits + SINK_S) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_S, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S) = 2.0/(package.r_hs + package.r_hs2_y);
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E, NUMBER_BLOCK_LAYERS*numberUnits + SINK_E) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_E, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E) = 2.0/(package.r_hs + package.r_hs2_x);
	conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W, NUMBER_BLOCK_LAYERS*numberUnits + SINK_W) = conductances(NUMBER_BLOCK_LAYERS*numberUnits + SINK_W, NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W) = 2.0/(package.r_hs + package.r_hs2_x);
	/* vertical g's between inner peripheral sink nodes and ambient	*/
	Gamb(numberUnits + SINK_C_N) = Gamb(numberUnits + SINK_C_S) = 1.0 / (package.r_hs_c_per_y + package.r_amb_c_per_y);
	Gamb(numberUnits + SINK_C_E) = Gamb(numberUnits + SINK_C_W) = 1.0 / (package.r_hs_c_per_x + package.r_amb_c_per_x);
	/* vertical g's between outer peripheral sink nodes and ambient	*/
	Gamb(numberUnits + SINK_N) = Gamb(numberUnits + SINK_S) = Gamb(numberUnits + SINK_E) = Gamb(numberUnits + SINK_W) = 1.0 / (package.r_hs_per + package.r_amb_per);

	/* Calculate matrix B such that BT = POWER in steady state */
	/* Non-diagonal elements	*/
	for (int i = 0; i < numberThermalNodes; i++)
		for (int j = 0; j < i; j++)
			if ((conductances(i,j) == 0.0) || (conductances(j,i) == 0.0))
				B(i,j) = B(j,i) = 0.0;
			else
				/* Here is why the 2.0 factor comes when calculating conductances(,)	*/
				B(i,j) = B(j,i) = -1.0/((1.0/conductances(i,j))+(1.0/conductances(j,i)));
	/* Diagonal elements	*/
	for (int i = 0; i < numberThermalNodes; i++) {
		/* Functional blocks in the heat sink layer	*/
		if (i >= HSINK*numberUnits && i < NUMBER_BLOCK_LAYERS*numberUnits)
			B(i,i) = Gamb(i%numberUnits);
		/* Heat sink peripheral nodes	*/
		else if (i >= NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W)
			B(i,i) = Gamb(numberUnits + i - NUMBER_BLOCK_LAYERS*numberUnits);
		/* All other nodes that are not connected to the ambient	*/
		else
			B(i,i) = 0.0;
		/* Sum up the conductances	*/
		for(int j = 0; j < numberThermalNodes; j++)
			if (i != j)
				B(i,i) -= B(i,j);
	}

	/* Fill the inverse of matrix B	*/
	Binv = B.inverse();

	/* Fill the G vector from the Gamb vector	*/
	for (int n = 0; n < Gamb.rows(); n++){
		G(n + numberThermalNodes - Gamb.rows()) = Gamb(n);
	}


	/* vertical conductances to ambient	*/
	//model->t_vector = dvector(m);/* scratch pad	*/
	//model->p = ivector(m);		/* permutation vector for b's LUP decomposition	*/
	/* Compute the LUP decomposition of B and store it too	*/
	//Matrix<double, Dynamic, Dynamic> LU;
	//LU = B;

	/*
	 * B is a symmetric positive definite matrix. It is
	 * symmetric because if a node A is connected to B,
	 * then B is also connected to A with the same R value.
	 * It is positive definite because of the following
	 * informal argument from Professor Lieven Vandenberghe's
	 * lecture slides for the spring 2004-2005 EE 103 class
	 * at UCLA: http://www.ee.ucla.edu/~vandenbe/103/chol.pdf
	 * x^T*B*x = voltage^T * (B*x) = voltage^T * current
	 * = total power dissipated in the resistors > 0
	 * for x != 0.
	 */
	//lupdcmp(lu, NUMBER_BLOCK_LAYERS*numberUnits + EXTRA, p, 1);




	return true;
}









bool RCmodel::loadCblock(const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	double floorplanWidth = floorplan.getWidth();
	double floorplanHeight = floorplan.getHeight();

	/* Load the information of the package	*/
	package.loadC(configuration, floorplanWidth, floorplanHeight);

	A.resize(numberThermalNodes);
	Ainv.resize(numberThermalNodes);

	/* Functional block C's */
	for (int i = 0; i < numberUnits; i++) {
		double area = floorplan.units[i].getArea();
		/* C's from functional units to ground	*/
		A(i) = getThermalCapacitance(configuration.pChip, configuration.tChip, area);
		/* C's from interface portion of the functional units to ground	*/
		A(IFACE*numberUnits + i) = getThermalCapacitance(configuration.pInterface, configuration.tInterface, area);
		/* C's from spreader portion of the functional units to ground	*/
		A(HSP*numberUnits + i) = getThermalCapacitance(configuration.pSpreader, configuration.tSpreader, area);
		/* C's from heatsink portion of the functional units to ground	*/
		/* vertical C to ambient: divide configuration.cConvec proportional to area	*/
		double c_amb = C_FACTOR * configuration.cConvec / (configuration.sSink * configuration.sSink) * area;
		A(HSINK*numberUnits + i) = getThermalCapacitance(configuration.pSink, configuration.tSink, area) + c_amb;
	}

	/* C's from peripheral(n,s,e,w) nodes	*/
 	/* from peripheral spreader nodes to ground	*/
	A(NUMBER_BLOCK_LAYERS*numberUnits + SP_N) = A(NUMBER_BLOCK_LAYERS*numberUnits + SP_S) = package.c_sp_per_y;
	A(NUMBER_BLOCK_LAYERS*numberUnits + SP_E) = A(NUMBER_BLOCK_LAYERS*numberUnits + SP_W) = package.c_sp_per_x;
 	/* From center peripheral sink nodes to ground
	 * NOTE: this treatment of capacitances (and the corresponding treatment of resistances in loadRblock) as parallel (series)
	 * is only approximate and is done in order to avoid creating an extra layer of nodes.
	 */
	A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_N) = A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_S) = package.c_hs_c_per_y + package.c_amb_c_per_y;
	A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_E) = A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_C_W) = package.c_hs_c_per_x + package.c_amb_c_per_x;
	/* From outer peripheral sink nodes to ground	*/
	A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_N)= A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_S) = A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_E) = A(NUMBER_BLOCK_LAYERS*numberUnits + SINK_W) = package.c_hs_per + package.c_amb_per;

	/* Calculate A^-1 (for diagonal matrix A) such that A(dT) + BT = POWER */
	for (int i = 0; i < numberThermalNodes; i++)
		Ainv(i) = 1.0/A(i);

	/* We are always going to use the eqn dT = - A^-1 * B T + A^-1 * POWER. so, store  C = - A^-1 * B	*/
	C = Ainv.asDiagonal() * B;
	for(int i = 0; i < numberThermalNodes; i++)
		for(int j = 0; j < numberThermalNodes; j++)
			if(C(i,j) != 0)
				C(i,j) = - C(i,j);


	return true;
}





void RCmodel::populateDefaultGridLayers(const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	LayerInformation siliconLayer, interfaceMaterialLayer;

	/* Silicon layer	*/
	siliconLayer.has_lateral = true;
	siliconLayer.has_power = false;
	siliconLayer.k = configuration.kChip;
	siliconLayer.thickness = configuration.tChip;
	siliconLayer.sp = configuration.pChip;
	siliconLayer.layerFloorplan = floorplan;
	gridLayers.push_back(siliconLayer);


	/* Interface material layer	*/
	interfaceMaterialLayer.has_lateral = true;
	interfaceMaterialLayer.has_power = false;
	interfaceMaterialLayer.k = configuration.kInterface;
	interfaceMaterialLayer.thickness = configuration.tInterface;
	interfaceMaterialLayer.sp = configuration.pInterface;
	interfaceMaterialLayer.layerFloorplan = floorplan;
	gridLayers.push_back(interfaceMaterialLayer);

	if (configuration.modelSecondary == true){
		LayerInformation metalLayer, c4UnderfillLayer;

		/* Metal layer	*/
		metalLayer.has_lateral = true;
		metalLayer.has_power = false;
		metalLayer.k = K_METAL;
		metalLayer.k1 = K_DIELECTRIC;
		metalLayer.thickness = configuration.tMetal;
		metalLayer.sp = SPEC_HEAT_METAL;
		metalLayer.layerFloorplan = floorplan;
		gridLayers.push_back(metalLayer);


		/* C4/underfill layer*/
		c4UnderfillLayer.has_lateral = true;
		c4UnderfillLayer.has_power = false;
		c4UnderfillLayer.k = K_C4;
		c4UnderfillLayer.k1 = K_UNDERFILL;
		c4UnderfillLayer.thickness = configuration.tC4;
		c4UnderfillLayer.sp = SPEC_HEAT_C4;
		c4UnderfillLayer.sp1 = SPEC_HEAT_UNDERFILL;
		c4UnderfillLayer.layerFloorplan = floorplan;
		gridLayers.push_back(c4UnderfillLayer);
	}
}





void RCmodel::appendPackageLayers(const ConfigurationParameters &configuration)
{
	if(gridLayers.size() <= 0)
		return;

	LayerInformation spreaderLayer, heatsinkLayer;
	Floorplan lastFloorplan = gridLayers[gridLayers.size() - 1].layerFloorplan;

	/* Spreader layer	*/
	spreaderLayer.has_lateral = true;
	spreaderLayer.has_power = false;
	spreaderLayer.k = configuration.kSpreader;
	spreaderLayer.thickness = configuration.tSpreader;
	spreaderLayer.sp = configuration.pSpreader;
	spreaderLayer.layerFloorplan = lastFloorplan;
	gridLayers.push_back(spreaderLayer);


	/* Heatsink layer	*/
	heatsinkLayer.has_lateral = true;
	heatsinkLayer.has_power = false;
	heatsinkLayer.k = configuration.kSink;
	heatsinkLayer.thickness = configuration.tSink;
	heatsinkLayer.sp = configuration.pSink;
	heatsinkLayer.layerFloorplan = lastFloorplan;
	gridLayers.push_back(heatsinkLayer);


	if (configuration.modelSecondary) {
		LayerInformation packagaeSubstrateLayer, solderBallsLayer, pcbLayer;

		/* Package substrate layer	*/
		packagaeSubstrateLayer.has_lateral = true;
		packagaeSubstrateLayer.has_power = false;
		packagaeSubstrateLayer.k = K_SUB;
		packagaeSubstrateLayer.thickness = configuration.tSub;
		packagaeSubstrateLayer.sp = SPEC_HEAT_SUB;
		packagaeSubstrateLayer.layerFloorplan = lastFloorplan;
		gridLayers.push_back(packagaeSubstrateLayer);


		/* Solder balls layer	*/
		solderBallsLayer.has_lateral = true;
		solderBallsLayer.has_power = false;
		solderBallsLayer.k = K_SOLDER;
		solderBallsLayer.thickness = configuration.tSolder;
		solderBallsLayer.sp = SPEC_HEAT_SOLDER;
		solderBallsLayer.layerFloorplan = lastFloorplan;
		gridLayers.push_back(solderBallsLayer);


		/* PCB layer	*/
		pcbLayer.has_lateral = true;
		pcbLayer.has_power = false;
		pcbLayer.k = K_PCB;
		pcbLayer.thickness = configuration.tPcb;
		pcbLayer.sp = SPEC_HEAT_PCB;
		pcbLayer.layerFloorplan = lastFloorplan;
		gridLayers.push_back(pcbLayer);
	}
}





void RCmodel::parseLayerFile(const string &fileName)
{
	ifstream inputGridLayerFile;
	if(fileName.size() > 0){
		inputGridLayerFile.open(fileName.c_str());
		if(inputGridLayerFile.is_open()){
			if(inputGridLayerFile.good() == false){
				cout << "Error: Input grid layer file: \"" << fileName << "\" could not be open for reading." << endl;
				inputGridLayerFile.close();
				exit(1);
			}
		}
		else{
			cout << "Error: Input grid layer file: \"" << fileName << "\" could not be open for reading." << endl;
			exit(1);
		}
	}
	else{
		return;
	}


	int significantLinesInFile = 0;
	int field = 0;
	vector<int> layerOrder;
	LayerInformation auxLayer;
	double floorplanWidth = 0;
	double floorplanHeight = 0;


	// Read until the end of the file
	while(inputGridLayerFile.good()){
		// Read one line from the file
		string line;
		getline(inputGridLayerFile, line);

		// If the line was successfully read
		if(inputGridLayerFile.good()){
			// If the line is not a comment
			if(line.size() > 0){
				if(line[0] != '#'){
					switch (field){
						case LCF_SNO:
							int layerNumber;
								try{
									int iValue;
									stringstream streamValue(line);
									streamValue.exceptions(stringstream::goodbit);
									streamValue >> iValue;
									if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
										for(unsigned int i = 0; layerOrder.size(); i++){
											if(layerOrder[i] == iValue){
												cout << "Error in layer configuration file: The layer number must be unique values." << endl;
												inputGridLayerFile.close();
												exit(1);
											}
										}
										layerOrder.push_back(iValue);
									}
									else{
										cout << "Error in layer configuration file: The layer number should be an integer value." << endl;
										inputGridLayerFile.close();
										exit(1);
									}
								}
								catch(...){
									cout << "Error in layer configuration file: The layer number should be an integer value." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
								break;
						case LCF_LATERAL:
							if(line.size() == 1){
								if((line[0] == 'Y') || (line[0] == 'y')){
									auxLayer.has_lateral = true;
								}
								else if((line[0] == 'N') || (line[0] == 'n')){
									auxLayer.has_lateral = false;
								}
								else{
									cout << "Error in layer configuration file: Invalid layer heat flow indicator." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
							}
							else{
								cout << "Error in layer configuration file: Invalid layer heat flow indicator." << endl;
								inputGridLayerFile.close();
								exit(1);
							}
							break;
						case LCF_POWER:
							if(line.size() == 1){
								if((line[0] == 'Y') || (line[0] == 'y')){
									auxLayer.has_power = true;
								}
								else if((line[0] == 'N') || (line[0] == 'n')){
									auxLayer.has_power = false;
								}
								else{
									cout << "Error in layer configuration file: Invalid layer power dissipation indicator." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
							}
							else{
								cout << "Error in layer configuration file: Invalid layer power dissipation indicator." << endl;
								inputGridLayerFile.close();
								exit(1);
							}
							break;
						case LCF_SP:
							try{
								double dValue;
								stringstream streamValue(line);
								streamValue.exceptions(stringstream::goodbit);
								streamValue >> dValue;
								if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
									auxLayer.sp = dValue;
								}
								else{
									cout << "Error in layer configuration file: Invalid specific heat." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
							}
							catch(...){
								cout << "Error in layer configuration file: Invalid specific heat." << endl;
								inputGridLayerFile.close();
								exit(1);
							}
							break;
						case LCF_RHO:
							try{
								double dValue;
								stringstream streamValue(line);
								streamValue.exceptions(stringstream::goodbit);
								streamValue >> dValue;
								if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
									auxLayer.k = 1.0 / dValue;
								}
								else{
									cout << "Error in layer configuration file: Invalid resistivity." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
							}
							catch(...){
								cout << "Error in layer configuration file: Invalid resistivity." << endl;
								inputGridLayerFile.close();
								exit(1);
							}
							break;
						case LCF_THICK:
							try{
								double dValue;
								stringstream streamValue(line);
								streamValue.exceptions(stringstream::goodbit);
								streamValue >> dValue;
								if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
									auxLayer.thickness = dValue;
								}
								else{
									cout << "Error in layer configuration file: Invalid thickness." << endl;
									inputGridLayerFile.close();
									exit(1);
								}
							}
							catch(...){
								cout << "Error in layer configuration file: Invalid thickness." << endl;
								inputGridLayerFile.close();
								exit(1);
							}
							break;
						case LCF_FLP:
							if(auxLayer.layerFloorplan.readFloorplan(line, false) == false){
								cout << "Error in layer configuration file: There are no functional units in the floorplan." << endl;
								inputGridLayerFile.close();
								exit(1);
							}

							/* First layer	*/
							if (significantLinesInFile < LCF_NPARAMS) {
								floorplanWidth = auxLayer.layerFloorplan.getWidth();
								floorplanHeight = auxLayer.layerFloorplan.getHeight();
							}
							else if((equalTo(floorplanWidth, auxLayer.layerFloorplan.getWidth()) == false) || (equalTo(floorplanHeight, auxLayer.layerFloorplan.getHeight()) == false)){
								cout << "Error in layer configuration file: Width and height differ across layers." << endl;
								inputGridLayerFile.close();
								exit(1);
							}

							gridLayers.push_back(auxLayer);

							break;
						default:
							cout << "Error in layer configuration file: Invalid field number." << endl;
							inputGridLayerFile.close();
							exit(1);
							break;
					}
					field = (field + 1) % LCF_NPARAMS;
					significantLinesInFile++;
				}
			}
		}
	}
	inputGridLayerFile.close();


	if (significantLinesInFile % LCF_NPARAMS){
		cout << "Error: Wrong number of lines in layer file." << endl;
		exit(1);
	}
}




/* setup the block and grid mapping data structures	*/
void RCmodel::set_bgmap(const ConfigurationParameters &configuration, Floorplan &floorplan, LayerInformation &layer)
{
	/* i1, i2, j1 and j2 are indices of the boundary grid cells	*/
	int i1, i2, j1, j2;


	double floorplanWidth = floorplan.getWidth();
	double floorplanHeight = floorplan.getHeight();
	double cellWidth = floorplanWidth / configuration.gridCols;
	double cellHeight = floorplanHeight / configuration.gridRows;

	int numberUnitsFloorplan = layer.layerFloorplan.getNumberFunctionalUnits();


	/* Initialize	*/
	layer.b2gmap.resize(configuration.gridRows, configuration.gridCols);
	for(int i = 0; i < configuration.gridRows; i++){
		for(int j = 0; j < configuration.gridCols; j++){
			layer.b2gmap(i,j).blocks.clear();
		}
	}
	layer.g2bmap.resize(numberUnitsFloorplan);


	/* For each functional unit	*/
	for(unsigned int u = 0; u < numberUnitsFloorplan; u++) {
		/* Shortcuts for unit boundaries	*/
		double lu = layer.layerFloorplan.units[u].left;
		double ru = lu + layer.layerFloorplan.units[u].width;
		double bu = layer.layerFloorplan.units[u].bottom;
		double tu = bu + layer.layerFloorplan.units[u].height;

		/* top index (lesser row) = rows - ceil (top / cell height)	*/
		i1 = configuration.gridRows - tolerant_ceil(tu/cellHeight);
		/* bottom index (greater row) = rows - floor (bottom / cell height)	*/
		i2 = configuration.gridRows - tolerant_floor(bu/cellHeight);
		/* left index = floor (left / cell width)	*/
		j1 = tolerant_floor(lu/cellWidth);
		/* right index = ceil (right / cell width)	*/
		j2 = tolerant_ceil(ru/cellWidth);
		/* sanity check	*/
		if((i1 < 0) || (j1 < 0)){
			cout << "Error: Negative grid cell start index!" << endl;
			exit(1);
		}
		if((i2 > configuration.gridRows) || (j2 > configuration.gridCols)){
			cout << "Error: Grid cell end index out of bounds!" << endl;
			exit(1);
		}
		if((i1 >= i2) || (j1 >= j2)){
			cout << "Error: Invalid floorplan spec or grid resolution" << endl;
			exit(1);
		}

		/* Setup g2bmap	*/
		layer.g2bmap(u).i1 = i1;
		layer.g2bmap(u).i2 = i2;
		layer.g2bmap(u).j1 = j1;
		layer.g2bmap(u).j2 = j2;

		/* Setup b2gmap	*/
		/* For each grid cell in this unit	*/
		for(int i = i1; i < i2; i++){
			for(int j = j1; j < j2; j++){
				/* Grid cells fully overlapped by this unit	*/
				if ((i > i1) && (i < i2-1) && (j > j1) && (j < j2-1)) {

					if (layer.b2gmap(i,j).blocks.size() != 0){
						/* This should not occur since the grid cell is fully covered and hence, no other unit should be sharing it	*/
						cout << "Warning: Overlap of functional blocks?" << endl;
					}

					layer.b2gmap(i,j).append(u, 1.0);
				}
				/* Boundary grid cells partially overlapped by this unit	*/
				else {
					/* Shortcuts for cell boundaries	*/
					double lc = j * cellWidth, rc = (j+1) * cellWidth;
					double tc = floorplanHeight - i * cellHeight;
					double bc = floorplanHeight - (i+1) * cellHeight;

					/* Shortcuts for overlap width and height	*/
					double oh = (minimum(tu, tc) - maximum(bu, bc));
					double ow = (minimum(ru, rc) - maximum(lu, lc));
					double occupancy;

					/* Overlap tolerance	*/
					if (equalTo(oh/cellHeight, 0))
						oh = 0;
					else if (equalTo(oh/cellHeight, 1))
						oh = cellHeight;

					if (equalTo(ow/cellWidth, 0))
						ow = 0;
					else if (equalTo(ow/cellWidth, 1))
						ow = cellWidth;

					occupancy = (oh * ow) / (cellHeight * cellWidth);
					if (oh < 0 || ow < 0){
						cout << "Error: negative overlap!" << endl;
						exit(1);
					}


					layer.b2gmap(i,j).append(u, occupancy);
				}
			}
		}
	}
}





void RCmodel::populateGridLayers(const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	/* Read in values from the layer configuration file when specified	*/
	if(configuration.fileNameGridLayer.size() > 0){
		if (configuration.modelSecondary == true){
			cout << "Error: Modeling secondary heat transfer path not supported when layer configuration file specified..." << endl;
			exit(1);
		}
		else{
			parseLayerFile(configuration.fileNameGridLayer);
		}
		cout << "Warning: Layer configuration file specified. Overriding default floorplan with those in the layer configuration file..." << endl;
	}
	else{
		/* Populate default set of layers	*/
		numberUnits = floorplan.getNumberFunctionalUnits();
		populateDefaultGridLayers(configuration, floorplan);
	}


	/* Append the package layers	*/
	appendPackageLayers(configuration);
}








bool RCmodel::loadRgrid(const ConfigurationParameters &configuration, Floorplan &floorplan)
{
	if (configuration.gridRows & (configuration.gridRows-1) ||
		configuration.gridCols & (configuration.gridCols-1)){
		cout << "Error: The number of grid rows and grid columns should both be powers of two." << endl;
		exit(1);
	}

	/* Check if there is a layer configuration file specified	*/
	bool hasLayerConfigurationFile;
	if(configuration.fileNameGridLayer.size() > 0){
		hasLayerConfigurationFile = true;
	}
	else{
		hasLayerConfigurationFile = false;
		numberUnits = floorplan.getNumberFunctionalUnits();
	}



	/* Get layer information	*/
	populateGridLayers(configuration, floorplan);


	/* count the total no. of blocks */
	//model->total_n_blocks = 0;
	//for(unsigned int i = 0; i < gridLayer.size(); i++)
	//	model->total_n_blocks += gridLayer[i].flp->n_units;




	double floorplanWidth;
	double floorplanHeight;
	int inner_layers;


	if (configuration.modelSecondary)
		inner_layers = gridLayers.size() - DEFAULT_PACK_LAYERS - SEC_PACK_LAYERS;
	else
		inner_layers = gridLayers.size() - DEFAULT_PACK_LAYERS;



	/* Setup the block-grid maps; flp parameter is ignored */
	if (hasLayerConfigurationFile){
		if(inner_layers > 0){
			for(int i = 0; i < inner_layers; i++){
				set_bgmap(configuration, floorplan, gridLayers[i]);
			}
			for(int i = inner_layers; i < gridLayers.size(); i++){
				gridLayers[i].b2gmap = gridLayers[inner_layers - 1].b2gmap;
				gridLayers[i].g2bmap = gridLayers[inner_layers - 1].g2bmap;
			}
		}
	}
	/* Only the silicon layer has allocated space for the maps. All the rest just point to it. so it is sufficient to setup the block-grid map for the silicon layer alone.
	 * Further, for default layer configuration, the `flp' parameter should be the same as that of the silicon layer. Finally, the chip width and height information
	 * need to be populated for default layer configuration.
	 */
	else {
		floorplanWidth = floorplan.getWidth();
		floorplanHeight = floorplan.getHeight();
		if(gridLayers.size() > 0){
			set_bgmap(configuration, floorplan, gridLayers[0]);
			for(int i = 1; i < gridLayers.size(); i++){
				gridLayers[i].b2gmap = gridLayers[0].b2gmap;
				gridLayers[i].g2bmap = gridLayers[0].g2bmap;
			}
		}
	}


	/*	Sanity check on floorplan sizes	*/
	if ( (floorplanWidth > configuration.sSink) ||
		 (floorplanHeight > configuration.sSink) ||
		 (floorplanWidth > configuration.sSpreader) ||
		 (floorplanHeight > configuration.sSpreader)){

		floorplan.print();
		cout << "Error: Inordinate floorplan size." << endl;
		exit(1);
	}


	double cellWidth = floorplanWidth / configuration.gridCols;
	double cellHeight = floorplanHeight / configuration.gridRows;
	double cellArea = cellWidth * cellHeight;



	/* Load the information of the package	*/
	package.loadR(configuration, floorplanWidth, floorplanHeight);



	/* Layer specific resistances	*/
	for(unsigned int i = 0; i < gridLayers.size(); i++) {
		if (gridLayers[i].has_lateral) {
			gridLayers[i].rx =  getThermalResistance(gridLayers[i].k, cellWidth, cellHeight * gridLayers[i].thickness);
			gridLayers[i].ry =  getThermalResistance(gridLayers[i].k, cellHeight, cellWidth * gridLayers[i].thickness);
			if (configuration.modelSecondary && (i==inner_layers-SEC_CHIP_LAYERS+LAYER_C4)) {
				gridLayers[i].rx1 =  getThermalResistance(gridLayers[i].k1, cellWidth, cellHeight * gridLayers[i].thickness);
				gridLayers[i].ry1 =  getThermalResistance(gridLayers[i].k1, cellHeight, cellWidth * gridLayers[i].thickness);
			}
		}
		else {
			/* Positive infinity	*/
			gridLayers[i].rx = DBL_MAX;
			gridLayers[i].ry = DBL_MAX;
			if (configuration.modelSecondary && (i == inner_layers - SEC_CHIP_LAYERS + LAYER_C4)) {
				gridLayers[i].rx1 = DBL_MAX;
				gridLayers[i].ry1 = DBL_MAX;
			}
		}
		gridLayers[i].rz =  getThermalResistance(gridLayers[i].k, gridLayers[i].thickness, cellArea);
		if (configuration.modelSecondary && (i == inner_layers - SEC_CHIP_LAYERS + LAYER_C4)) {
			gridLayers[i].rz1 =  getThermalResistance(gridLayers[i].k1, gridLayers[i].thickness, cellArea);
		}

		if (configuration.modelSecondary == false) {
			/* Heatsink	is connected to ambient. divide r_convec proportional to cell area */
			if (i == gridLayers.size() - DEFAULT_PACK_LAYERS + LAYER_SINK)
				gridLayers[i].rz += configuration.rConvec * (configuration.sSink * configuration.sSink) / cellArea;
		}
		else {
			/* Heatsink	is connected to ambient. divide r_convec proportional to cell area */
			if (i == gridLayers.size() - DEFAULT_PACK_LAYERS - SEC_PACK_LAYERS + LAYER_SINK)
				gridLayers[i].rz += configuration.rConvec * (configuration.sSink * configuration.sSink) / cellArea;
		}
	}


	return true;
}


bool RCmodel::loadCgrid(const ConfigurationParameters &configuration, Floorplan &floorplan)
{


	return true;
}















void RCmodel::print(void)
{
	if((B.rows() == 0) || (B.cols() == 0)){
		cout << "Matrix B is empty." << endl;
	}
	else{
		cout << "Matrix B(" << B.rows() << "," << B.cols() << "):" << endl;
		cout << B << endl << endl;
	}

	if((Binv.rows() == 0) || (Binv.cols() == 0)){
		cout << "Matrix B^-1 is empty." << endl;
	}
	else{
		cout << "Matrix B^-1(" << Binv.rows() << "," << Binv.cols() << "):" << endl;
		cout << Binv << endl << endl;
	}

	if((A.rows() == 0) || (A.cols() == 0)){
		cout << "Matrix A is empty." << endl;
	}
	else{
		Matrix<double, Dynamic, Dynamic> auxMatrix = A.asDiagonal();
		cout << "Matrix A(" << auxMatrix.rows() << "," << auxMatrix.cols() << "):" << endl;
		cout << auxMatrix << endl << endl;
	}

	if((Ainv.rows() == 0) || (Ainv.cols() == 0)){
		cout << "Matrix A^-1 is empty." << endl;
	}
	else{
		Matrix<double, Dynamic, Dynamic> auxMatrix = Ainv.asDiagonal();
		cout << "Matrix A^-1(" << auxMatrix.rows() << "," << auxMatrix.cols() << "):" << endl;
		cout << auxMatrix << endl << endl;
	}

	if((C.rows() > 0) && (C.cols() > 0)){
		cout << "Matrix C(" << C.rows() << "," << C.cols() << "):" << endl;
		cout << C << endl << endl;
	}

	if((G.rows() == 0) || (G.cols() == 0)){
		cout << "Vector G is empty." << endl;
	}
	else{
		cout << "Vector G:" << G.rows() << "):" << endl;
		cout << G << endl << endl;
	}
}
