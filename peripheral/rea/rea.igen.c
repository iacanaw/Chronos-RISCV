
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////


#include "rea.igen.h"
#include "reliability.h"
#include "../whnoc_dma/noc.h"

#define NUM_SAMPLES 30
int calculate = 0;
/////////////////////////////// Port Declarations //////////////////////////////

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage("I", "rea", "Example");
//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

PPM_DOC_FN(installDocs){

    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc2_node, "Reliability Estimator Accelerator");
    }
}
////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
}

///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {

    UnitRel *rel_unit;      /* Class containing all reliability data*/
    double total_struct_area;   /* Total processor die area*/
    double total_fits;   /*  Total FITS (1/MTTF) of the entire processor*/
    int rel_counter;
    long double access_counter;
    int unitc;                  /* Unit counter*/

    floorplan_structure floorplan;

    total_fits = 0.0;   
    total_struct_area=0.0;
    access_counter = 0.0; 

    /* Reliability objects and memory allocation*/
    rel_unit=(UnitRel *)calloc(TOTAL_STRUCTURES,sizeof(UnitRel));

    /* Reliability object initialization for every structure on the processor*/
    for (unitc=0;unitc< TOTAL_STRUCTURES;unitc++)
    {
        sprintf(floorplan.units[unitc].name, "p%d", unitc);
        floorplan.units[unitc].height = 0.00021801; // 0.000275 -> valor padrão
        floorplan.units[unitc].width = 0.00021801;

        rel_unit[unitc].init(&floorplan,unitc);  /* Initialize structures*/
        rel_unit[unitc].fitinit(unitc);     /* Initialize FITS for each structure*/
    }

    int structures;

    double power[NUM_SAMPLES];
    double temperatures[NUM_SAMPLES];

    double total_inst;          /* Total number of instructions executed */
    double EM_total;            /* Total EM FITS */
    double SM_total;            /* Total SM FITS */
    double TDDB_total;          /* Total TDDB FITS */
    double TC_total;            /* Total TC FITS */
    double NBTI_total;          /* Total NBTI FITS */

    static double *unit_act;
    double globalcountdown;
    float total_structure_fits;

    int EM_act_ratio[TOTAL_STRUCTURES];

    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    for (int i = 0; i < NUM_SAMPLES; i++)
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

            total_fits = total_fits+rel_unit[structures].fits;      /*  Computes Total average FIT value of processor */
            total_inst =  total_inst + rel_unit[structures].ind_inst;/*  Computes Total instantaneous FIT value of processor */
            EM_total = EM_total + rel_unit[structures].EM_fits;         /* Total average individual failure mech FIT value*/
            SM_total = SM_total + rel_unit[structures].SM_fits; 
            TDDB_total = TDDB_total + rel_unit[structures].TDDB_fits; 
            TC_total = TC_total + rel_unit[structures].TC_fits; 
            NBTI_total = NBTI_total + rel_unit[structures].NBTI_fits; 
            
            //unit_act[unitc]=0;
        }

    } /* for each new temperature measurement*/    

    while(1){



    }

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}

