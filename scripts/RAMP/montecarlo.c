/* Reliability Aware MicroProcessors (RAMP) Version 2.0
 * Developed by Jayanth Srinivasan (srinivsn@cs.uiuc.edu)
  * */


/* montecarlo.c: Monte-Carlo simulation for RAMP with MIN-MAX code for series-parallel failure
 * analysis*/


/* This file is used to calculate MTTF of series-parallel failure systems using
 * lognormal failure distributions. Details of the exact modeling methodology
 * can be found in:
 *
 * "Exploiting Structural Duplication for Lifetime Reliability Enhancement", J.
 * Srinivasan, S. V. Adve, P. Bose, J. A. Rivers, In the Proceedings of the 32nd
 * International Symposium on Computer Architecture (ISCA-2005), June 2005. */

/* The command line for using this file is 
 *
 * a.out FITFILE
 *
 * where FITFILE is obtained from RAMP. Generating the FITFILE is shown in
 * runtime.cc. The format of the FIT file is
 * FIT value due to EM for structure 1
 * FIT value due to SM for structure 1
 * FIT value due to TDDB for structure 1
 * FIT value due to TC for structure 1
 * FIT value due to NBTI for structure 1
 * FIT value due to EM for structure 2 ...
 *
 * A sample FIT file, SAMPLE_FITS, is provided for a processor with 10
 * structures and 5 failure mechanisms and will give a MTTF of around 30
 * years */

#include <stdio.h> 
#include <math.h> 
#include <stdlib.h> 
#include <time.h>
#include <string.h>

#define min(X, Y)  ((X) < (Y) ? (X) : (Y)) 
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define DIM_X 4
#define DIM_Y 4

#define TOTAL_STRUCTURES DIM_X*DIM_Y

/* Variable initalization */

float expected_mttf[5][TOTAL_STRUCTURES];
float lognorm_lambda[5][TOTAL_STRUCTURES];
float lognorm_lifetime[5][TOTAL_STRUCTURES];
float structure_lifetime[TOTAL_STRUCTURES];
float fits[5][TOTAL_STRUCTURES];

float temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9;
float min_fit;

float randomval;
float lifetime_lognorm;
float mttf_lognorm;
int totalruns=10000000; /* This sets the number of iterations in the Monte-Carlo
analysis. We use a value of 10E7. The more the iterations, the more accurate the
result. However, this will also increase simulation time.*/

int runs;
float lognorm_correc;
int tempvar1;
int fail_count;
int structure_count;

float totallife;
float mttftotal=0.0;
float finalmttf;

int read_fits(char *fitfile);

/* Main Monte-Carlo function */
int main(int argc,char *argv[])
{

/* Reading of FIT values from FITFILE. The read_fits function can be found after
 * main() */

char *fitfile = argv[1];
strcpy(fitfile,argv[1]);
read_fits(fitfile);

/* The MTTF value of each structure due to each failure mechanism depends on its
 * FIT value. 1 FIT represents one failure in 10E9 operating hours. Therefore, a
 * FIT value of 3805.17503 gives an MTTF of 30 years. */

for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
	{
	for (fail_count=0;fail_count<5;fail_count++)
	{
		expected_mttf[fail_count][structure_count]=(30.0*3805.17503)/fits[fail_count][structure_count];
	}
}


srand((unsigned)time(NULL));

/* Generating lognormal correction factor. This is used to normalize the MTTF of
 * the lognormal distribution.*/

for (runs =0; runs < totalruns; runs++)
{
	randomval = (float)rand()/RAND_MAX;
	
	if ((-1.52245 - log(randomval/4.58344)) < 0.0)
		lifetime_lognorm = 0.0;	
	else
		lifetime_lognorm = pow(2.7182818,(-0.5 + sqrt(-1.52245 - log(randomval/4.58344)))) - pow(2.7182818,(-0.5));

	if (runs ==0)
		mttf_lognorm = lifetime_lognorm;		

	if (runs > 0)
		mttf_lognorm = ((mttf_lognorm/(runs+1))*runs)+((lifetime_lognorm)/(runs+1));
	
}

lognorm_correc = 1.0/mttf_lognorm; 

/* Based on the  MTTF of each structure due to each failure mecahnism, a lambda
 * in the lognormal equation is generated for each structure and failure
 * mechanism. */

for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
{
	for (fail_count=0;fail_count<5;fail_count++)
	{
		lognorm_lambda[fail_count][structure_count]=expected_mttf[fail_count][structure_count]/mttf_lognorm;
	}
}


/* This is the main Monte-Carlo simulation loop.*/

for (runs=0; runs<totalruns; runs++)
{
	
/* Random lifetimes are generated for each structure and failure mechanism based
 * on the structure and failure mechanism's MTTF. */

for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
{
	for (fail_count=0;fail_count<5;fail_count++)
	{
		randomval = (float)rand()/RAND_MAX;
	
		if ((-1.52245 - log(randomval/4.58344)) < 0.0)
		lognorm_lifetime[fail_count][structure_count] = 0.0;	
		else
		lognorm_lifetime[fail_count][structure_count] = lognorm_lambda[fail_count][structure_count]*(pow(2.7182818,(-0.5 + pow((-1.52245 - log(randomval/4.58344)),0.5))) - pow(2.7182818,(-0.5)))/0.92986;
	}
}

/* In order to calculate lifetime for a series-parallel failure system, we use
 * the MIN-MAX method. Simply put, the lifetime of two structures in series is
 * the minimum of the lifetime of the individual structures. Similarly, the
 * lifetime of two structures in parallel is the maximum of the lifetime of the
 * individual structures. */



/* Within each structure, the individual failure mechanisms act like a series
 * failure system. That is, the first failure due to any mechanism will cause
 * the entire structure to fail. 	structure_lifetime gives the lifetime of the
 * structure due to all mechanisms.*/

for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
{
	structure_lifetime[structure_count]=min(min(min(min(lognorm_lifetime[0][structure_count],lognorm_lifetime[1][structure_count]),lognorm_lifetime[2][structure_count]),lognorm_lifetime[3][structure_count]),lognorm_lifetime[4][structure_count]);

}


/* The next step is to calculate the lifetime of the entire series-parallel
 * processor using the MIN-MAX method. We assume a sample processor with 10
 * structures, with structures 0 and 1 in parallel, structures 2 and 3 in
 * parallel, and everything else in series. In other words, structures 0 and 1
 * are duplicates, structures 2 and 3 are duplicates, and everything else is
 * critical and not duplicated.*/



// temp0=max(structure_lifetime[0],structure_lifetime[1]); /* Max for structures 0
// and 1*/
// temp1=max(structure_lifetime[2],structure_lifetime[3]); /* Max for structures 2
// and 3*/
// temp2=min(structure_lifetime[4],structure_lifetime[5]); /* Min for all other
// structures */
// temp3=min(structure_lifetime[6],structure_lifetime[7]);
// temp4=min(structure_lifetime[8],structure_lifetime[9]);


// temp5=min(temp0,temp1);
// temp6=min(temp2,temp3);
// temp7=min(temp5,temp6);

// totallife=min(temp4,temp7); /* This is the final lifetime of the processor for
// this iteration of the Monte-Carlo algorithm. */


min_fit = structure_lifetime[0];
for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
{
        if (structure_lifetime[structure_count] < min_fit)
                min_fit = structure_lifetime[structure_count];
}
totallife = min_fit;

mttftotal+=totallife;	

} /* end Monte-Carlo iterations */

finalmttf=mttftotal/totalruns; /* The average across all iterations of the
Monte-Carlo algorithm give the final MTTF of the processor. */ 

printf("Lognormal MTTF of the series-parallel processor with FIT values from %s is %f\n",fitfile,finalmttf);

} /* end main()*/


int read_fits(char *fitfile)
{
        int tempvar=0;
        int structure_count=0; /* Variable to keep track of structures */
        int fail_count=0; 	   /* Variable to keep track of failure mechs */
        float tempfit=0.0;
        char s[100];

        FILE *fp = fopen(fitfile,"r");	/* Open file containing FIT values from
		RAMP */


        if(!fp)
        {
                printf("ERROR - Can't open FIT file %s \n",fitfile);
        }

        for (fail_count=0;fail_count<5;fail_count++)
        {
        for (structure_count=0;structure_count<TOTAL_STRUCTURES;structure_count++)
        {
                fgets(s,1000,fp);
                tempvar =   sscanf(s,"%f",&tempfit);
                fits[fail_count][structure_count]=tempfit;
        }
        }

        fclose(fp);
        return 0;
}















