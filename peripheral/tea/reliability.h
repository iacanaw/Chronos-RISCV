/* Reliability Aware MicroProcessors (RAMP) Version 2.0
 * Developed by Jayanth Srinivasan (srinivsn@cs.uiuc.edu)*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#ifndef RELIABILITY_H_
#define RELIABILITY_H_ 

#include "../whnoc_dma/noc.h"

/* reliability.h : Defines all global variables used by RAMP */

/* The bibliography of references for data used in this file can be found at the
 * end of the file.*/

/* Total FITS distributed to base processor at reliability qualification.
 * We assume a processor MTTF of 30 years, which gives a total processor FIT
 * value of approximately 4000. At qualification, we divide the total FITS
 * equally across all failure mechanisms (giving 800 each), and this is further
 * divided among the individual structures in proportion to their area.*/
#define TOTAL_EM_FITS 800.0
#define TOTAL_SM_FITS 800.0
#define TOTAL_TDDB_FITS 800.0
#define TOTAL_TC_FITS 800.0
#define TOTAL_NBTI_FITS 800.0	

#define EM_Ea_div_k  (0.9/(8.62e-5)) 	/* EM activation energy [1]*/

#define SM_Ea_div_k  (0.9/(8.62e-5)) 	/* SM activation energy [1]*/

#define SM_T_base 500.0		/* Stress free temperature or metal deposition temperature (assuming sputtering) [1]*/

#define TC_exponent 2.35	/* Coffin-Manson exponent [1]*/

#define NBTI_beta   0.3		/* Beta in NBTI equation [2]*/

#define T_base  345.00	/* Qualification temperature. At this value, the total processor FIT value will be 4000. By changing this temperature, we can model processors with different reliability qualification costs (Higher implies more expensive).*/ 

#define ambient 318.15

#define TOTAL_STRUCTURES DIM_X*DIM_Y

struct unit_t{
	char name[10];
    float height;
    float width;
};

struct floorplan_structure{
    struct unit_t units[TOTAL_STRUCTURES];
};

struct UnitRel{
    int unit_ref_number;	/* Reference number of this structure*/

	// double old_temp;		/* Previous sampling temperature*/

	// double currently_positive;
	// double final_diff;
	// double old_set;
	// double delta_T;
    // long int delta_counter;
	// double TC_new_cycles;

	// double non_startup;

	float mean;
    //float stdev;
    //long long tc_counter;
    //long countdown;
    //int firsttime;
    //double frequency;
	//double nocycles;


	long int access_counter; 
	float fits;			 	/* Total fits for the structure*/

	//float TC_cycles_allowed;
	
	float EM_inst;		/* Instantaneous FIT values due to each faliure mechanism*/
	float SM_inst;
	float TDDB_inst;
	float TC_inst;
	float NBTI_inst;
	float ind_inst;

	float EM_base_fits;		/* Starting FIT values due to each faliure mechanism*/
	float SM_base_fits;
	float TDDB_base_fits;
	float TC_base_fits;
	float NBTI_base_fits;

	float EM_fits;		/* Total FIT values due to each faliure mechanism*/
	float SM_fits;
	float TDDB_fits;
	float TC_fits;
	float NBTI_fits;
	
	float unit_area;	 /* Area of structure */
	float total_struct_area;
};

struct UnitRel rel_unit[TOTAL_STRUCTURES]; /* Struct containing all reliability data*/
// const float T_base = 345.0;	/* Qualification temperature. At this value, the total processor FIT value will be 4000. By changing this temperature, we can model processors with different reliability qualification costs (Higher implies more expensive).*/ 
// const float ambient = 318.15;

void init(struct floorplan_structure *floorplan, int unit_number);
void fitinit(int unit_number);
void allmodels(float all_temp, float all_act, float all_voltage, float all_freq, int unit_number);
void EM(float temp, float act, float rel_voltage, float rel_freq, int un);
void SM(float temp, float act, float rel_voltage, float rel_freq, int un);
void TDDB(float temp, float act, float rel_voltage, float rel_freq, int un);
void TC(float temp, float act, float rel_voltage, float rel_freq, int un);
void NBTI(float temp, float act, float rel_voltage, float rel_freq, int un);

/* Bibliography
 *
 * [1] "Failure Mechanisms and Models for Semiconductor Devices," In JEDEC
 * Publication JEP122-A, 2002.
 *
 * [2] "A Model for Negative Bias Temperature Instability (NBTI) in Oxide and
 * High-K pFETS", Sufi Zafar et al,, In 2004 Symposia on VLSI Technology and
 * Circuits, June 2004.
 *
 * */
#endif /* RELIABILITY_H_ */
