/* Reliability Aware MicroProcessors (RAMP) Version 2.0
 * Developed by Jayanth Srinivasan (srinivsn@cs.uiuc.edu)
 * */


/* reliability.cc: Main reliability file with models for each failure mechanism
 * */

/* The bibliography of references for data used in this file can be found at the
 * end of the file.*/

#include "reliability.h"

#include <iostream>
using namespace std;


/* Global reliability variables*/
double total_fits; 			/* Total processor FITs*/
extern double total_struct_area;		/* Total  processor area - Initialized in mainsim.cc*/
static double sampling_intvl = 1.0e-3;        /* 3.33 us sampling interval = 10K cycles at 3GHz*/
							/* Qualification temperature (intialized in reliability.h)*/

/* Object initialization  
 * Inputs : Pointer  to the structure, structure number
 * outputs : None*/

void UnitRel::init(floorplan_structure *floorplan,int unit_number)
{
	unit_t *rel_unit;			/* Each structure is a rel_unit object	*/
	unit_ref_number = unit_number;
	access_counter=0;			/* Cycle count*/
	
	rel_unit = &floorplan->units[unit_number];		 /* The floorplan file is used for each structure*/

	unit_area = (rel_unit->height)*(rel_unit->width); 		/* The area of each
	structure is calculated using the floorplan file*/
	
	
	
	total_struct_area += unit_area;		/* Keeping track of total area of the
	processor*/
	
	fits=0; 	   	/*Initialize each structure's FITs to 0*/

	ind_inst = 0;		/* Initialize each structure's instantaneous FIT value
	*/
	
	//printf("The area of structure %s is %0.12f \n",rel_unit->name,unit_area);
} /* UnitRel::init*/		


/* Initializing FITS per structure. This is done based on the area of each
 * structure. 
 * Inputs: Structure number
 * Outputs: none
 */

void UnitRel::fitinit(int unit_number)
{
	access_counter=0;

	/* Base FITS for structure is proportional to the area
	  occupied by the structure on the processor */
	
	EM_base_fits= TOTAL_EM_FITS*unit_area/total_struct_area;
	SM_base_fits= (TOTAL_SM_FITS*(unit_area)/total_struct_area);
	TDDB_base_fits= (TOTAL_TDDB_FITS*(unit_area)/total_struct_area);
	TC_base_fits= (TOTAL_TC_FITS*(unit_area)/total_struct_area);
	NBTI_base_fits= (TOTAL_NBTI_FITS*(unit_area)/total_struct_area);
	
	/* Initial FIT values are same as base FIT values*/
	
	EM_fits= TOTAL_EM_FITS*unit_area/total_struct_area;
	SM_fits= (TOTAL_SM_FITS*(unit_area)/total_struct_area);
	TDDB_fits= (TOTAL_TDDB_FITS*(unit_area)/total_struct_area);
	TC_fits= (TOTAL_TC_FITS*(unit_area)/total_struct_area);
	NBTI_fits= (TOTAL_NBTI_FITS*(unit_area)/total_struct_area);
} /* UnitRel::fitinit*/




/* RAMP parameters are calculated in this function for each structure
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 */

void UnitRel::allmodels(float all_temp,float all_act, float all_voltage, float all_freq)
{
	/* Call functions to calculate MTTF values by each failure mechanism*/
	EM(all_temp,all_act,all_voltage,all_freq);
	SM(all_temp,all_act,all_voltage,all_freq);
	TDDB(all_temp,all_act,all_voltage,all_freq);
	TC(all_temp,all_act,all_voltage,all_freq);
	NBTI(all_temp,all_act,all_voltage,all_freq);

	/* Instantaneous and time-average FIT values are updated by the above calls
	 fits per structure - add as more models are added.	*/
	
	fits = (EM_fits + SM_fits + TDDB_fits + TC_fits+NBTI_fits);		/* Over-all
	time-averaged FIT values*/
	
	ind_inst = (EM_inst + SM_inst + TDDB_inst + TC_inst + NBTI_inst);	/* Instantaneous FIT values */
	access_counter++;	/* Counter to manintain runnign average of FITS*/
} /* UnitRel::allmodels*/




/* Electromigration model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The Electromigration MTTF model is based on Black's equation and is adapted
 * from [1]*/


void UnitRel::EM(float temp,float act, float rel_voltage, float rel_freq)
{

	float act_ratio;	/* Activity ratio - between base and current sample*/
	float temp_diff;	/* Temperature difference between base and current sample*/
	float rel_exp;		/* exponent in MTTF equation for Electron migration	*/
	float fits_ratio;

	act_ratio = act;
	temp_diff = (1.0/T_base) - (1.0/temp); 
	rel_exp = EM_Ea_div_k*temp_diff; 
	fits_ratio = ((pow((act_ratio),1.1))*(pow(2.718,rel_exp))/(1.0))*(pow((6.0/6.0),1.1));

	if (access_counter>1){
		EM_fits =( EM_fits*(access_counter-1) + fits_ratio*EM_base_fits)/access_counter;	/* update running average of FITS */
	}
	else
		EM_fits =( EM_fits*(access_counter-1) + fits_ratio*EM_base_fits);

	//printf("EM_fits: %f\n-----------~\n", EM_fits);
	
	EM_inst = fits_ratio*EM_base_fits;	/* Instantaneous values of FITS*/
}	



/* Stress Migration model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The Stress Migration MTTF model is based on the stress migration equation in [1]*/
void UnitRel::SM(float temp,float act, float rel_voltage, float rel_freq)
{
		float temp_ratio; 	/* Ratio of temperature diffs*/
        float temp_diff; 	/* Temp diff*/
        float rel_exp;
        float fits_ratio;
        
		temp_ratio = (SM_T_base - temp)/(SM_T_base - T_base);
		temp_diff = (1.0/T_base) - (1.0/temp);
        rel_exp = SM_Ea_div_k*temp_diff;
        fits_ratio = (pow((temp_ratio),2.5))*(pow(2.718,rel_exp));

        if (access_counter>1)
        {
                SM_fits =( SM_fits*(access_counter-1) + fits_ratio*SM_base_fits)/access_counter; 
        }

	SM_inst = fits_ratio*SM_base_fits;	/* update instantaneous FITS for SM*/
}



/* Time-Dependent Dielectric Breakdown model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The TDDB model is based on expiremental work by Wu et al. from IBM [2] */

void UnitRel::TDDB(float temp,float act, float rel_voltage, float rel_freq)
{

	float TBDratio; 						/* Non temperature term ratio*/
	float base_exp;							/* Base temperature reliability factor*/
	float new_exp;							/* New reliability factor*/
	float fits_ratio;

	base_exp = pow(2.718,((0.759/((8.62e-5)*T_base))-(66.8/((8.62e-5)*pow(T_base,2.0)))-9.7099));
	new_exp = pow(2.718,((0.759/((8.62e-5)*(temp)))-(66.8/((8.62e-5)*pow((temp),2.0)))-9.7099));

	TBDratio = (pow((((1.10*(pow(rel_freq,0.42206))))/(1.10)),(78 - 0.081*(temp))))*(2.0*0.50)*(pow(10.0,((16.0 - 16.0 )/2.0)));  

        fits_ratio = TBDratio*(base_exp/new_exp);

	if (access_counter>1)
	{     	
               	TDDB_fits =( TDDB_fits*(access_counter-1) + fits_ratio*TDDB_base_fits)/access_counter;
     } 
	 TDDB_inst = fits_ratio*TDDB_base_fits;
}



/* Thermal Cycling model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The thermal cycling model is based on the Coffin-Manson equation and is
 * adapted from [1] */

void UnitRel::TC(float temp,float act, float rel_voltage, float rel_freq)
{

/* For TC, we calculate the average mean temperature of each structure */

	if (access_counter>1){     	
		mean = (mean*(access_counter-1) + temp)/access_counter;
	}
	else 
		mean = temp;

	float TC_base_temp_diff = T_base - ambient;	
	TC_fits = pow(((mean-ambient)/(TC_base_temp_diff)),TC_exponent)*TC_base_fits;

	TC_inst = TC_fits;
}




/* Negative Bias Temperature Instability model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The NBTI model is based on experimental work by Zafar et al. at IBM [3]*/

void UnitRel::NBTI(float temp,float act, float rel_voltage, float rel_freq)
{
	
	float temp_ratio; 	/* Ratio of temperature diffs*/
    float temp_diff; 	/* Temp diff */
    float rel_exp;
    float fits_ratio;

	float NBTI_basemttf;
	float NBTI_currentmttf;
        
	temp_ratio = T_base/temp;
	temp_diff = (1.0/T_base) - (1.0/temp);
	rel_exp = 795.2*temp_diff;

	NBTI_basemttf = pow(((log(1.6368/(1.0+2.0*pow(2.7182818,(856.1067/T_base)))) - log((1.6368/(1.0+2.0*pow(2.7182818,(856.1067/T_base))))-0.01))*(T_base/(pow(2.7182818,(-795.2/T_base))))),(1.0/NBTI_beta));
			
	NBTI_currentmttf = pow(((log(1.6368/(1.0+2.0*pow(2.7182818,(856.1067/temp)))) - log((1.6368/(1.0+2.0*pow(2.7182818,(856.1067/temp))))-0.01))*(temp/(pow(2.7182818,(-795.2/temp))))),(1.0/NBTI_beta));

	fits_ratio=NBTI_basemttf/NBTI_currentmttf;

	if (access_counter>1){
		NBTI_fits =( NBTI_fits*(access_counter-1) + fits_ratio*NBTI_base_fits)/access_counter;
	}
	NBTI_inst = fits_ratio*NBTI_base_fits;
}


/* Bibliography
 *
 * [1] "Failure Mechanisms and Models for Semiconductor Devices," In JEDEC
 * Publication JEP122-A, 2002.
 *
 * [2] "Interplay of Voltage and Temperature Acceleration of Oxide Breakdown for
 * Ultra-Thin Gate Dioxides," E. Y. Wu et al., In Solid-State Electronics
 * Journal, 2002.
 * 
 * [3] "A Model for Negative Bias Temperature Instability (NBTI) in Oxide  and
 * High-K pFETS", Sufi Zafar et al,, In 2004 Symposia on VLSI Technology  and
 * Circuits, June 2004.
 *
 * */

