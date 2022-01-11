/*
 * Copyright (c) 2005-2017 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20191106.0
//
////////////////////////////////////////////////////////////////////////////////


#include "tea.igen.h"
#include "../whnoc_dma/noc.h"

#define INT_CONVE 1048576   //1048576
#define INT_CONVB 8192      //8192
#define INT_CONVC 128       //128    //CONVE\CONVB

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//////////////////////////////// Callback stubs ////////////////////////////////

#define CLUSTER_SIZE        (DIM_X*DIM_Y)  
#define SYSTEM_SIZE         (DIM_X*DIM_Y)  
#define MONITOR_WINDOW      250000
#define WINDOW_TIME         0.25                 //ns - required to calculate power related to energy
#define SCALING_FACTOR      20                  //simulating McPat results with 2 GHz frequency
#define MASTER_ADDR         0x0000              // x=0 y=0 
#define HEADER_SIZE         2                   // sendTime, service
#define THERMAL_NODES       (SYSTEM_SIZE*4)+12  // 4 thermal nodes for each PE plus 12 from the environment
#define TAMB                31815
#define TAM_FLIT            32
#define CLUSTER_X		    DIM_X
#define CLUSTER_Y		    DIM_Y
#define SYSTEM_X		    DIM_X
#define SYSTEM_Y		    DIM_Y

double Binv[THERMAL_NODES][SYSTEM_SIZE];
double Cexp[THERMAL_NODES][THERMAL_NODES];

unsigned long int mac_accumulator;

unsigned int power[DIM_Y][DIM_X];
double power_trace[SYSTEM_SIZE];
double t_steady[THERMAL_NODES];
double TempTraceEnd[THERMAL_NODES];


unsigned int thePacket[SYSTEM_SIZE+13 ];
unsigned int sendPacketToMaster = 0;
unsigned int packetPointer = 0;


unsigned int flit_in_counter = 0;
unsigned int msg_size = 0;
unsigned int source_pe = 0;
unsigned int x_data_counter = 0;
unsigned int y_data_counter = 0;

unsigned int samples_received[DIM_Y][DIM_X];

FILE *fp;

void load_matrices(double Binv[THERMAL_NODES][SYSTEM_SIZE], double Cexp[THERMAL_NODES][THERMAL_NODES]){
    FILE *binvpointer;
    binvpointer = fopen("peripheral/tea/binv.txt","r");
    FILE *cexppointer;
    cexppointer = fopen("peripheral/tea/cexp.txt","r");

    char line[4000];
    char *number;
    int column, row;

    for (row = 0; row < THERMAL_NODES; row++){
        fgets(line, sizeof(line), binvpointer);
        number = strtok(line, " ");
        for(column = 0; column < SYSTEM_SIZE; column++){
            Binv[row][column] = strtod(number, NULL);
            //printf("%f ", Binv[row][column]); 
            number = strtok(NULL, " ");      
        }
    }

    for (row = 0; row < THERMAL_NODES; row++){
        fgets(line, sizeof(line), cexppointer);
        number = strtok(line, " ");
        for(column = 0; column < THERMAL_NODES; column++){
            Cexp[row][column] = strtod(number, NULL);
            //printf("%f ", Cexp[row][column]); 
            number = strtok(NULL, " ");      
        }
    }

    fclose(binvpointer);
    fclose(cexppointer);
}

#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

unsigned int htonl(unsigned int x){
    return x;//__bswap_constant_32(x);
}

///////////////////////////////////////////////////////////////////
/* Gets the X coordinate from the address */
unsigned int getXpos(unsigned int addr){
    return ((addr & 0x0000FF00) >> 8);
}

///////////////////////////////////////////////////////////////////
/* Gets the Y coordinate from the address */
unsigned int getYpos(unsigned int addr){
    return (addr & 0x000000FF);
}

///////////////////////////////////////////////////////////////////
unsigned int mac_unit(unsigned int clear, unsigned int calc, unsigned int data_in_a, unsigned int data_in_b){
    if(calc == 1 && clear == 1){
        mac_accumulator = (unsigned long int)data_in_a * (unsigned long int)data_in_b;
    }
    else if(calc == 1){
        mac_accumulator = mac_accumulator + (unsigned long int)data_in_a * (unsigned long int)data_in_b;
    } 
    return (unsigned int)mac_accumulator;
}

void computeSteadyStateTemp(double Tsteady[THERMAL_NODES], double power_trace[SYSTEM_SIZE]){
    int i, j;
    double heatContributionPower;

    for(i = 0; i < THERMAL_NODES; i++){

        heatContributionPower = 0;
        for(j = 0; j < SYSTEM_SIZE; j++){
            heatContributionPower += Binv[i][j]*power_trace[j];
        }
        Tsteady[i] = heatContributionPower + (double)TAMB/100; //+
        //cout << "Tsteady[i] = heatContributionPower + (double)TAMB/100 -> " << Tsteady[i] << " = " << heatContributionPower << " + " << (double)TAMB/100 << endl;
    }
}

void temp_matex(double TempTraceEnd[THERMAL_NODES], double power_trace[SYSTEM_SIZE]) {
    int i, j, k;
    double sumExponentials;

    double Tsteady[THERMAL_NODES];
    double Tdifference[THERMAL_NODES];

    FILE *filepointer;
    filepointer = fopen("simulation/matex.txt","a");

    computeSteadyStateTemp(Tsteady, power_trace);

    for(k = 0; k < THERMAL_NODES; k++)
        Tdifference[k] = TempTraceEnd[k] - Tsteady[k];

    fprintf(filepointer,"Power: "); 
    for(i = 0; i < SYSTEM_SIZE; i++)
        fprintf(filepointer,"%f ", power_trace[i]);
    fprintf(filepointer,"\n");

    fprintf(filepointer,"Tsteady: "); 
    for(i = 0; i < SYSTEM_SIZE; i++)
        fprintf(filepointer,"%f ", Tsteady[i]);
    fprintf(filepointer,"\n");

    for(k = 0; k < THERMAL_NODES; k++){
        sumExponentials = 0;
        //if (k <= 36) cout << k << ": " << Tdifference[k] << endl;
        for(j = 0; j < THERMAL_NODES; j++){
            sumExponentials += Cexp[k][j] * Tdifference[j];
        }
        TempTraceEnd[k] = Tsteady[k] + sumExponentials;
    }


    fprintf(filepointer,"Temperatures: "); 
    for(i = 0; i < SYSTEM_SIZE; i++)
        fprintf(filepointer,"%f ", TempTraceEnd[i]);
    fprintf(filepointer,"\n");
    
    //bhmMessage("I", "Input", "temp[%d]: %f\n",i, TempTraceEnd[i]);

    fclose(filepointer);
}

/////////////////////////////// Port Declarations //////////////////////////////

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage("I", "tea", "Example");
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
        ppmDocAddText(doc2_node, "Termal Estimator Accelerator");
    }
}

////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
}

////////////////////////////////// Callbacks //////////////////////////////////

unsigned int routerCredit = 0;

PPM_PACKETNET_CB(controlUpdate) {
    // Input information with 4 bytes are about flow control
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        routerCredit = ctrl;
    }
    // Information with 8 bytes are about iterations
    else if(bytes == 8) {   
        // When an iteration arrives
        if(sendPacketToMaster){
            if(routerCredit == 0){
                // send one flit to the router
                //bhmMessage("I", "Input", "send %d flit to the router: %d\n", packetPointer, thePacket[packetPointer]);
                ppmPacketnetWrite(handles.portData, &thePacket[packetPointer], sizeof(thePacket[packetPointer]));
                
                // Updates packet pointer
                if(packetPointer < SYSTEM_SIZE+12){
                    packetPointer++;
                }
                else{
                    packetPointer = 0;
                    sendPacketToMaster = 0;
                }
            }
        }
    }
}

unsigned int checkPowerReceived(){
    int i, j;
    int rtrn = 1;
    int warning = 0;
    for(i=0; i < DIM_X; i++){
        for(j=0; j < DIM_Y; j++){
            if(samples_received[j][i] > 3){
                warning = 1;
            }
        }
    }
    for(i=0; i < DIM_X; i++){
        for(j=0; j < DIM_Y; j++){
            if(samples_received[j][i] == 0){
                rtrn = 0;
                if(warning){
                    bhmMessage("I", "TEA", "PE [%x,%x] nao enviou seu power!",i, j);   
                    //while(1){}
                }
            }
        }
    }
    return rtrn;
}

PPM_PACKETNET_CB(dataUpdate) {
    unsigned int newFlit = *(unsigned int *)data;
    //bhmMessage("I", "TEA", "Chegou um flit: %x", htonl(newFlit));
    flit_in_counter++;
    if(flit_in_counter == 2){       // Segundo flit - SIZE
        msg_size = htonl(newFlit);
        x_data_counter = 0; // atualmente sempre vem do 00
        y_data_counter = 0; // atualmente sempre vem do 00
        source_pe = 0x0000;
    }
    else if(flit_in_counter == 4){  // quarto flit - SOURCE
        source_pe =  htonl(newFlit);
        x_data_counter = getXpos(source_pe);
        y_data_counter = getYpos(source_pe);
    }
    else if(flit_in_counter == 5){  // quinto flit - energia do PE
        power[y_data_counter][x_data_counter] =  htonl(newFlit);
        bhmMessage("I", "Input", "power[%d][%d]: %d\n",x_data_counter, y_data_counter, power[y_data_counter][x_data_counter]);
        samples_received[y_data_counter][x_data_counter]++;
        //samples_received++;
    }
    else if(flit_in_counter >= 13){
        flit_in_counter = 0;
    }
    
    if(checkPowerReceived()){ // Acabou de receber as energias
    //if(samples_received >= DIM_X*DIM_Y){ // Acabou de receber as energias
        //samples_received = 0;
        for(y_data_counter=0; y_data_counter < DIM_Y; y_data_counter++){
            for(x_data_counter=0; x_data_counter < DIM_X; x_data_counter++){
                samples_received[y_data_counter][x_data_counter] = 0;
            }
        }
        //bhmMessage("I", "INFO", "TEA RECEBEU TODAS AS ENERGIAS!");
        ////////////////////////////////////////////////////////////////////////
        /*CALCULAR STEADY*/
        /*Avança os ponteiros da matriz B*/
        //bhmMessage("I", "Input", "Calculando STEADY!\n");
        int index = 0;
        int yi, xi;
        for (yi = 0; yi < DIM_Y; yi++)
            for(xi = 0; xi < DIM_X; xi++){
                power_trace[index] = ((((double)power[yi][xi] * 64) / 100) / (1000000000)) * 10; //0.001));
                //power_trace[index] = ((((double)power[yi][xi]* 64 / 1000 / 100) * 128 / 100) * 20)/(1280000*0.001);
                //bhmMessage("I", "input", "power: %.6lf", power_trace[index]);
                index++;
            }
                //power_trace[index++] = (double)(power[yi][xi]*SCALING_FACTOR)/(1280000*WINDOW_TIME);
        computeSteadyStateTemp(t_steady, power_trace);

        ////////////////////////////////////////////////////////////////////////
        /*CALCULAR TRANSIENT*/
        //bhmMessage("I", "Input", "Calculando TRANSIENT!\n");

        temp_matex(TempTraceEnd, power_trace);

        source_pe =  0;
        x_data_counter = 0;
        y_data_counter = 0;

        // Packet to master
        int i;
        int tempi;
        thePacket[0] = MASTER_ADDR; // Header (destine address)
        thePacket[1] = htonl(DIM_X*DIM_Y + 11); 
        thePacket[2] = htonl(0x55); // #define TEMPERATURE_PACKET  0x55 (api.h)
        fp = fopen("simulation/SystemTemperature.tsv", "a");
        fprintf(fp, "%.4f", (bhmGetCurrentTime()/1000000));
        for(i = 0; i < DIM_Y*DIM_X; i++){
            tempi = TempTraceEnd[i]*100;
            thePacket[i+13] = htonl(tempi);
            //bhmMessage("I", "Input", "temperature %d: %.2f", i, (((float)tempi/100)-273.15));
            fprintf(fp, "\t%.2f", (((float)tempi/100)-273.15));
        }
        fprintf(fp, "\n");
        fclose(fp);
        sendPacketToMaster = 1;
    }
}

PPM_CONSTRUCTOR_CB(constructor) {
    // YOUR CODE HERE (pre constructor)
    periphConstructor();
    // YOUR CODE HERE (post constructor)
}

PPM_DESTRUCTOR_CB(destructor) {
    // YOUR CODE HERE (destructor)
}


PPM_SAVE_STATE_FN(peripheralSaveState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}

PPM_RESTORE_STATE_FN(peripheralRestoreState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}


///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {
    int i, j;
    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc2_node, "Termal Estimator Accelerator");
    }

    fp = fopen("simulation/SystemTemperature.tsv", "w");
    fprintf(fp, "time");
    for(i=0;i<DIM_X*DIM_Y;i++){
        fprintf(fp, "\t%d",i);
    }
    fprintf(fp, "\n");
    fclose(fp);

    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    load_matrices(Binv, Cexp);

    for(i=0;i<DIM_X;i++){
        for(j=0;j<DIM_Y;j++){
            power[i][j] = 0;
        }
    }
    for(i=0;i<THERMAL_NODES;i++){
        TempTraceEnd[i] = 318.15;
    }

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}

