/* Reliability Aware MicroProcessors (RAMP) Version 2.0
 * Developed by Jayanth Srinivasan (srinivsn@cs.uiuc.edu)
 * */


#include "reliability.h"

#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

#define SIDE_SIZE 0.000194 // mem 8Kb

int main(){

    UnitRel *rel_unit;      /* Class containing all reliability data*/
    double total_struct_area;   /* Total processor die area*/
    double total_fits;   /*  Total FITS (1/MTTF) of the entire processor*/
    int rel_counter;
    long double access_counter;
    int unitc;                  /* Unit counter*/

    floorplan_structure floorplan;

    total_fits = 0.0;   
    total_struct_area = 0.0;
    access_counter = 0.0; 

    /* Reliability objects and memory allocation*/
    rel_unit=(UnitRel *)calloc(TOTAL_STRUCTURES,sizeof(UnitRel));


    /* Reliability object initialization for every structure on the processor*/
    for (unitc=0;unitc< TOTAL_STRUCTURES;unitc++)
    {
        sprintf(floorplan.units[unitc].name, "p%d", unitc);
        floorplan.units[unitc].height = SIDE_SIZE;
        floorplan.units[unitc].width = SIDE_SIZE;

        rel_unit[unitc].init(&floorplan,unitc);  /* Initialize structures*/
        rel_unit[unitc].fitinit(unitc);     /* Initialize FITS for each structure*/
    }

    int structures;

    double **power;
    double **temperatures;

    //==============PARSER==========================
    ifstream inFile;
    string line;
    int num_lines = 0;
    int num_samples;

    inFile.open("matex.log");
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    while ( getline (inFile,line) )
        num_lines++;

    num_samples = (num_lines/3) - 20;
    inFile.close();

    power = (double**) malloc((TOTAL_STRUCTURES+1)*sizeof(double));
    temperatures = (double**) malloc((TOTAL_STRUCTURES+1)*sizeof(double));

    for (int i = 0; i < TOTAL_STRUCTURES+1; i++){
        power[i] = (double*) malloc(num_samples*sizeof(double));
        temperatures[i] = (double*) malloc(num_samples*sizeof(double));
    }

    int line_temp = 0;
    int line_power = 0;
    int idx = 0;

    int line_count = 0;
    int line_total = 0;
    inFile.open("matex.log");
    while ( getline (inFile,line) ){
        line_total++;
    }
    inFile.close();

    inFile.open("matex.log");
    while ( getline (inFile,line) ){
        string line_name;
        stringstream lineStream(line);
        lineStream >> line_name;
        
        if(line_count >= 10*3 && line_count < (line_total-10*3)){
            idx = 0;
            if (line_name.compare("Power:") == 0){
                while(lineStream.good()){
                    double auxValue;
                    lineStream >> auxValue;
                    power[idx][line_power] = auxValue;
                    idx++;
                }
                line_power++;
            }
            if (line_name.compare("Temperatures:") == 0){
                while(lineStream.good()){
                    double auxValue;
                    lineStream >> auxValue;
                    temperatures[idx][line_temp] = auxValue;
                    idx++;
                }
                line_temp++;
            }
        }
        line_count++;
    }
    inFile.close();

    //==============END PARSER==========================

    double total_inst = 0.0;          /* Total number of instructions executed */
    double EM_total = 0.0;            /* Total EM FITS */
    double SM_total = 0.0;            /* Total SM FITS */
    double TDDB_total = 0.0;          /* Total TDDB FITS */
    double TC_total = 0.0;            /* Total TC FITS */
    double NBTI_total = 0.0;          /* Total NBTI FITS */

    static double *unit_act;
    double globalcountdown;
    float total_structure_fits;

    double EM_act_ratio[TOTAL_STRUCTURES];

    for (int i = 0; i < num_samples; i++)
    {

        /* As explained earlier, activity factors are calculated based on a structures
         * power consumption.*/

        for (structures=0;structures<TOTAL_STRUCTURES;structures++)
        {
                //power_consumption[structures]=energy_consumption[structures]/temperature_sampling_period;
                EM_act_ratio[structures] = power[structures][i];
        }

        /* For every structure on the processor, calculate FITS (time-average and
         * instantaneous) for each temperature sample (given by the temperature
         * simulator).  FITS are calculated at the same temporal granularity as
         * temperature. Also calculate total FIT values due to each failure mechanism.
         * The variable total_fits keeps track of the total processor FIT value over the
         * entire application. At the end of the application simulation, total_fits will
         * give processor MTTF. total_inst gives instantaneous processor FIT values for
         * each temperature sample.
         * */

        for (structures=0;structures< TOTAL_STRUCTURES;structures++)
        {

            /* Calculate FIT value by feeding in each structures temperature, activity
             * factor, processor supply voltage, and processor frequency. */

            rel_unit[structures].allmodels(temperatures[structures][i],EM_act_ratio[structures],1.0,1.0);

            total_fits = total_fits + rel_unit[structures].fits;      /*  Computes Total average FIT value of processor */
            total_inst =  total_inst + rel_unit[structures].ind_inst;/*  Computes Total instantaneous FIT value of processor */
            EM_total = EM_total + rel_unit[structures].EM_fits;         /* Total average individual failure mech FIT value*/
            SM_total = SM_total + rel_unit[structures].SM_fits; 
            TDDB_total = TDDB_total + rel_unit[structures].TDDB_fits; 
            TC_total = TC_total + rel_unit[structures].TC_fits; 
            NBTI_total = NBTI_total + rel_unit[structures].NBTI_fits; 
            
            //unit_act[unitc]=0;
        }


    } /* for each new temperature measurement*/


    /* The following code should be inserted after the application simulation ends*/

    /* After application simulation, print out the simulation results of RAMP */

    printf("Total Processor FIT value over entire application %f \n", total_fits);
    printf("Total Processor EM FIT value over entire application %f \n", EM_total);
    printf("Total Processor SM FIT value over entire application %f \n", SM_total);
    printf("Total Processor TDDB FIT value over entire application %f \n", TDDB_total);
    printf("Total Processor TC FIT value over entire application %f \n", TC_total);
    printf("Total Processor NBTI FIT value over entire application %f \n", NBTI_total);


    /* After application simulation, print file for use in the Monte-Carlo analysis.
     * The file should contain the FIT values for each structure due to each failure
     * mechanism in the following format:
     *
     * FIT value due to EM for structure 1
     * FIT value due to SM for structure 1
     * FIT value due to TDDB for structure 1
     * FIT value due to TC for structure 1
     * FIT value due to NBTI for structure 1
     * FIT value due to EM for structure 2 ...
     *
     * SAMPLE_FITS is a sample FIT file. When fed into the Monte-Carlo simulation
     * tool (montecarlo.c), a processor MTTF of around 30 years will be calculated.
     */


    FILE *montecarlofile;
    montecarlofile = fopen("montecarlofile", "w");


    for (structures=0;structures< TOTAL_STRUCTURES;structures++)
    {

        /*Print  FIT values for each failure mechanism and structure */

        fprintf(montecarlofile,"%f\n",rel_unit[structures].EM_fits);
        fprintf(montecarlofile,"%f\n",rel_unit[structures].SM_fits);
        fprintf(montecarlofile,"%f\n",rel_unit[structures].TDDB_fits);
        fprintf(montecarlofile,"%f\n",rel_unit[structures].TC_fits);
        fprintf(montecarlofile,"%f\n",rel_unit[structures].NBTI_fits);

    }


}