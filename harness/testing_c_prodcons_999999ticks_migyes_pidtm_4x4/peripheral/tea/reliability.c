/* Reliability Aware MicroProcessors (RAMP) Version 2.0
 * Developed by Jayanth Srinivasan (srinivsn@cs.uiuc.edu)
 * */


/* reliability.cc: Main reliability file with models for each failure mechanism
 * */

/* The bibliography of references for data used in this file can be found at the
 * end of the file.*/

#include "reliability.h"

void init(struct floorplan_structure *floorplan, int unit_number){
	struct unit_t *unit;			/* Each structure is a rel_unit object	*/
	rel_unit[unit_number].unit_ref_number = unit_number;
	rel_unit[unit_number].access_counter = 0;			/* Cycle count*/
	
	unit = &floorplan->units[unit_number];		 /* The floorplan file is used for each structure*/

	rel_unit[unit_number].unit_area = (unit->height)*(unit->width); 		/* The area of each
	structure is calculated using the floorplan file*/
	
    rel_unit[unit_number].total_struct_area = 0;
	rel_unit[unit_number].total_struct_area += rel_unit[unit_number].unit_area;		/* Keeping track of total area of the processor*/
	
	rel_unit[unit_number].fits = 0; 	   	/*Initialize each structure's FITs to 0*/

	rel_unit[unit_number].ind_inst = 0;		/* Initialize each structure's instantaneous FIT value */
	
	printf("The area of structure %s is %0.12f \n",unit->name, rel_unit[unit_number].unit_area);
    return;
}

void fitinit(int unit_number){
    float unit_area = rel_unit[unit_number].unit_area;
    float total_struct_area = rel_unit[unit_number].total_struct_area;
    rel_unit[unit_number].access_counter=0;

	/* Base FITS for structure is proportional to the area
	  occupied by the structure on the processor */
	
	rel_unit[unit_number].EM_base_fits	= (TOTAL_EM_FITS	*(unit_area/total_struct_area));
	printf("EM_base_fits: %f\n", rel_unit[unit_number].EM_base_fits);
	rel_unit[unit_number].SM_base_fits	= (TOTAL_SM_FITS	*(unit_area/total_struct_area));
	printf("SM_base_fits: %f\n", rel_unit[unit_number].SM_base_fits);
	rel_unit[unit_number].TDDB_base_fits= (TOTAL_TDDB_FITS	*(unit_area/total_struct_area));
	printf("TDDB_base_fits: %f\n", rel_unit[unit_number].TDDB_base_fits);
	rel_unit[unit_number].TC_base_fits	= (TOTAL_TC_FITS	*(unit_area/total_struct_area));
	printf("TC_base_fits: %f\n", rel_unit[unit_number].TC_base_fits);
	rel_unit[unit_number].NBTI_base_fits= (TOTAL_NBTI_FITS	*(unit_area/total_struct_area));
	printf("NBTI_base_fits: %f\n", rel_unit[unit_number].NBTI_base_fits);
	
	/* Initial FIT values are same as base FIT values*/
	
	rel_unit[unit_number].EM_fits 	= (TOTAL_EM_FITS	*(unit_area/total_struct_area));
	rel_unit[unit_number].SM_fits 	= (TOTAL_SM_FITS	*(unit_area/total_struct_area));
	rel_unit[unit_number].TDDB_fits = (TOTAL_TDDB_FITS	*(unit_area/total_struct_area));
	rel_unit[unit_number].TC_fits 	= (TOTAL_TC_FITS	*(unit_area/total_struct_area));
	rel_unit[unit_number].NBTI_fits = (TOTAL_NBTI_FITS	*(unit_area/total_struct_area));
	rel_unit[unit_number].fits = rel_unit[unit_number].EM_fits + rel_unit[unit_number].SM_fits + rel_unit[unit_number].TDDB_fits + rel_unit[unit_number].TC_fits + rel_unit[unit_number].NBTI_fits;
    return;
}

/* RAMP parameters are calculated in this function for each structure
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 */

void allmodels(float all_temp, float all_act, float all_voltage, float all_freq, int unit_number){
	/* Call functions to calculate MTTF values by each failure mechanism*/
	EM(all_temp, all_act, all_voltage, all_freq, unit_number);
	SM(all_temp, all_act, all_voltage, all_freq, unit_number);
	TDDB(all_temp, all_act, all_voltage, all_freq, unit_number);
	TC(all_temp, all_act, all_voltage, all_freq, unit_number);
	NBTI(all_temp, all_act, all_voltage, all_freq, unit_number);

	/* Instantaneous and time-average FIT values are updated by the above calls
	 fits per structure - add as more models are added.	*/
	
	rel_unit[unit_number].fits = (rel_unit[unit_number].EM_fits + rel_unit[unit_number].SM_fits + rel_unit[unit_number].TDDB_fits + rel_unit[unit_number].TC_fits + rel_unit[unit_number].NBTI_fits);		/* Over-all time-averaged FIT values */
	
	rel_unit[unit_number].ind_inst = (rel_unit[unit_number].EM_inst + rel_unit[unit_number].SM_inst + rel_unit[unit_number].TDDB_inst + rel_unit[unit_number].TC_inst + rel_unit[unit_number].NBTI_inst);	/* Instantaneous FIT values */
	rel_unit[unit_number].access_counter++;	/* Counter to manintain runnign average of FITS*/
    return;
} /* UnitRel::allmodels*/


/* Electromigration model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The Electromigration MTTF model is based on Black's equation and is adapted
 * from [1]*/
void EM(float temp, float act, float rel_voltage, float rel_freq, int un){
    float act_ratio;	/* Activity ratio - between base and current sample*/
	float temp_diff;	/* Temperature difference between base and current sample*/
	float rel_exp;		/* exponent in MTTF equation for Electron migration	*/
	float fits_ratio;

	act_ratio = act;
	temp_diff = (1.0/T_base) - (1.0/temp); 
	rel_exp = EM_Ea_div_k*temp_diff; 
	fits_ratio = ((pow((act_ratio),1.1))*(pow(2.718,rel_exp))/(1.0))*(pow((6.0/6.0),1.1));

	if (rel_unit[un].access_counter>1){
		rel_unit[un].EM_fits =( rel_unit[un].EM_fits*(rel_unit[un].access_counter-1) + fits_ratio*rel_unit[un].EM_base_fits)/rel_unit[un].access_counter;	/* update running average of FITS */
	}
	
	rel_unit[un].EM_inst = fits_ratio*rel_unit[un].EM_base_fits;	/* Instantaneous values of FITS*/
    return;
}

/* Stress Migration model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The Stress Migration MTTF model is based on the stress migration equation in [1]*/
void SM(float temp, float act, float rel_voltage, float rel_freq, int un){
    float temp_ratio; 	/* Ratio of temperature diffs*/
    float temp_diff; 	/* Temp diff*/
    float rel_exp;
    float fits_ratio;
    
    temp_ratio = (SM_T_base - temp)/(SM_T_base - T_base);
    temp_diff = (1.0/T_base) - (1.0/temp);
    rel_exp = SM_Ea_div_k*temp_diff;
    fits_ratio = (pow((temp_ratio),2.5))*(pow(2.718,rel_exp));

    if (rel_unit[un].access_counter>1){
        rel_unit[un].SM_fits =( rel_unit[un].SM_fits*(rel_unit[un].access_counter-1) + fits_ratio*rel_unit[un].SM_base_fits)/rel_unit[un].access_counter;
    }

	rel_unit[un].SM_inst = fits_ratio*rel_unit[un].SM_base_fits;	/* update instantaneous FITS for SM*/
    return;
}

/* Time-Dependent Dielectric Breakdown model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The TDDB model is based on expiremental work by Wu et al. from IBM [2] */
void TDDB(float temp, float act, float rel_voltage, float rel_freq, int un){
    float TBDratio; 						/* Non temperature term ratio*/
	float base_exp;							/* Base temperature reliability factor*/
	float new_exp;							/* New reliability factor*/
	float fits_ratio;

	base_exp = pow(2.718,((0.759/((8.62e-5)*T_base))-(66.8/((8.62e-5)*pow(T_base,2.0)))-9.7099));
	new_exp = pow(2.718,((0.759/((8.62e-5)*(temp)))-(66.8/((8.62e-5)*pow((temp),2.0)))-9.7099));

	TBDratio = (pow((((1.10*(pow(rel_freq,0.42206))))/(1.10)),(78 - 0.081*(temp))))*(2.0*0.50)*(pow(10.0,((16.0 - 16.0 )/2.0)));  

        fits_ratio = TBDratio*(base_exp/new_exp);

	if (rel_unit[un].access_counter>1){
        rel_unit[un].TDDB_fits =( rel_unit[un].TDDB_fits*(rel_unit[un].access_counter-1) + fits_ratio*rel_unit[un].TDDB_base_fits)/rel_unit[un].access_counter;
    }
	rel_unit[un].TDDB_inst = fits_ratio*rel_unit[un].TDDB_base_fits;
    return;
}

/* Thermal Cycling model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The thermal cycling model is based on the Coffin-Manson equation and is
 * adapted from [1] */
void TC(float temp, float act, float rel_voltage, float rel_freq, int un){
    /* For TC, we calculate the average mean temperature of each structure */

	if (rel_unit[un].access_counter>1){     	
		rel_unit[un].mean = (rel_unit[un].mean*(rel_unit[un].access_counter-1) + temp)/rel_unit[un].access_counter;
    } else {
		rel_unit[un].mean = temp;
	}

	float TC_base_temp_diff = T_base - ambient;	
	rel_unit[un].TC_fits = pow(((rel_unit[un].mean-ambient)/(TC_base_temp_diff)),TC_exponent)*rel_unit[un].TC_base_fits;
	rel_unit[un].TC_inst = rel_unit[un].TC_fits;
    return;
}

/* Negative Bias Temperature Instability model
 * Input : Current temperature of structure, current activity factor of
 * structure, current voltage, current frequency
 * Output : None
 * The NBTI model is based on experimental work by Zafar et al. at IBM [3]*/
void NBTI(float temp, float act, float rel_voltage, float rel_freq, int un){
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

    if (rel_unit[un].access_counter>1){
        rel_unit[un].NBTI_fits =( rel_unit[un].NBTI_fits*(rel_unit[un].access_counter-1) + fits_ratio*rel_unit[un].NBTI_base_fits)/rel_unit[un].access_counter;
    }
	rel_unit[un].NBTI_inst = fits_ratio*rel_unit[un].NBTI_base_fits;
    return;
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

