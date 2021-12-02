#!/bin/sh

# Parameters
Y=$1
X=$2

N=$((X*Y))
N=$(($N-1))

# Removes the old .tcl
rm -rf iterator.tcl

# Creating the new iterator.tcl
echo "imodelnewperipheral -name iterator \\" >> iterator.tcl
echo "                    -constructor constructor \\" >> iterator.tcl
echo "                    -destructor  destructor \\" >> iterator.tcl
echo "                    -vendor gaph \\" >> iterator.tcl
echo "                    -library peripheral \\" >> iterator.tcl
echo "                    -version 1.0" >> iterator.tcl
echo "" >> iterator.tcl
echo "iadddocumentation -name Description \\" >> iterator.tcl
echo "                  -text \"The NoC iterator\"" >> iterator.tcl
echo "" >> iterator.tcl
echo "#########################################" >> iterator.tcl
echo "## A slave port on the processor bus" >> iterator.tcl
echo "#########################################" >> iterator.tcl

echo "" >> iterator.tcl
echo "" >> iterator.tcl
echo "#############################################" >> iterator.tcl
echo "## Data ports between iterator and routers ##" >> iterator.tcl
echo "#############################################" >> iterator.tcl
for i in $(seq 0 $N);
do
	echo "imodeladdpacketnetport  \\" >> iterator.tcl
    echo "      -name iterationPort"$i" \\" >> iterator.tcl
    echo "      -maxbytes 8 \\" >> iterator.tcl
    echo "      -updatefunction iteration"$i" \\" >> iterator.tcl
    echo "      -updatefunctionargument 0x00" >> iterator.tcl
    echo "" >> iterator.tcl
done

# Runs igen to create the peripheral
igen.exe --batch iterator.tcl --writec iterator
rm iterator.c.igen.stubs
rm iterator.igen.c

# Generates the iterator 
echo "////////////////////////////////////////////////////////////////////////////////" >> iterator.igen.c
echo "//" >> iterator.igen.c
echo "//                W R I T T E N   B Y   I M P E R A S   I G E N" >> iterator.igen.c
echo "//" >> iterator.igen.c
echo "//                             Version 20191106.0" >> iterator.igen.c
echo "//" >> iterator.igen.c
echo "////////////////////////////////////////////////////////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "#include \"iterator.igen.h\"" >> iterator.igen.c
echo "/////////////////////////////// Port Declarations //////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "handlesT handles;" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "// Iteration counter" >> iterator.igen.c
echo "unsigned long long int iterationN = 0;" >> iterator.igen.c
echo "unsigned long long int iteration;" >> iterator.igen.c
echo "unsigned int activity;" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "////////////////////////////// Functions ///////////////////////////////////////" >> iterator.igen.c
echo "/* Make a callback to each router */" >> iterator.igen.c
echo "void runIterations(){  " >> iterator.igen.c
echo "    unsigned int i=0;" >> iterator.igen.c
echo "    unsigned int tryAgain = 0;" >> iterator.igen.c
echo "    do{" >> iterator.igen.c
echo "        activity = 0;" >> iterator.igen.c
echo "        iterationN++;" >> iterator.igen.c
for i in $(seq 0 $N)
do
    echo "        iteration = iterationN;" >> iterator.igen.c
    echo "        ppmPacketnetWrite(handles.iterationPort"$i", &iteration, sizeof(iteration));" >> iterator.igen.c
done
echo "        i++;" >> iterator.igen.c
echo "        if(tryAgain == 0 && activity == 0){" >> iterator.igen.c
echo "            activity++; " >> iterator.igen.c
echo "            tryAgain++;// segunda chance" >> iterator.igen.c
echo "        }" >> iterator.igen.c
echo "        else{" >> iterator.igen.c
echo "            tryAgain = 0;" >> iterator.igen.c
echo "        }" >> iterator.igen.c
echo "    }while(activity != 0);" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "/////////////////////////////// Diagnostic level ///////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "// Test this variable to determine what diagnostics to output." >> iterator.igen.c
echo "// eg. if (diagnosticLevel >= 1) bhmMessage("I", "iterator", "Example");" >> iterator.igen.c
echo "//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used" >> iterator.igen.c
echo "Uns32 diagnosticLevel;" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "/////////////////////////// Diagnostic level callback //////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "static void setDiagLevel(Uns32 new) {" >> iterator.igen.c
echo "    diagnosticLevel = new;" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "////////////////////////////////// Constructor /////////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_CONSTRUCTOR_CB(periphConstructor) {" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "//////////////////////////////// Callback stubs ////////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "/* Read callback from the extra PE */" >> iterator.igen.c
echo "PPM_REG_READ_CB(iterateRead) { " >> iterator.igen.c
echo "    iterationN++;" >> iterator.igen.c
echo "    runIterations();" >> iterator.igen.c
echo "    return *(Uns32*)user;" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_REG_WRITE_CB(iterateWrite) {" >> iterator.igen.c
echo "    // YOUR CODE HERE (iterateWrite)" >> iterator.igen.c
echo "    *(Uns32*)user = data;" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
for i in $(seq 0 $N)
do
    echo "PPM_PACKETNET_CB(iteration"$i") {" >> iterator.igen.c
echo "    unsigned int act = *(unsigned int *)data;" >> iterator.igen.c
echo "    if(act > 0){" >> iterator.igen.c
echo "        activity++;" >> iterator.igen.c
echo "    }" >> iterator.igen.c
echo " }" >> iterator.igen.c
echo "" >> iterator.igen.c
done
echo "PPM_CONSTRUCTOR_CB(constructor) {" >> iterator.igen.c
echo "    // YOUR CODE HERE (pre constructor)" >> iterator.igen.c
echo "    periphConstructor();" >> iterator.igen.c
echo "    // YOUR CODE HERE (post constructor)" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_DESTRUCTOR_CB(destructor) {" >> iterator.igen.c
echo "    // YOUR CODE HERE (destructor)" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_SAVE_STATE_FN(peripheralSaveState) {" >> iterator.igen.c
echo "    bhmMessage(\"E\", \"PPM_RSNI\", \"Model does not implement save/restore\");" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_RESTORE_STATE_FN(peripheralRestoreState) {" >> iterator.igen.c
echo "    bhmMessage(\"E\", \"PPM_RSNI\", \"Model does not implement save/restore\");" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "PPM_DOC_FN(installDocs){" >> iterator.igen.c
echo "    ppmDocNodeP Root1_node = ppmDocAddSection(0, \"Root\");" >> iterator.igen.c
echo "    {" >> iterator.igen.c
echo "        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, \"Description\");" >> iterator.igen.c
echo "        ppmDocAddText(doc2_node, \"The NoC iterator\");" >> iterator.igen.c
echo "    }" >> iterator.igen.c
echo "}" >> iterator.igen.c
echo "///////////////////////////////////// Main /////////////////////////////////////" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "int main(int argc, char *argv[]) {" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "    diagnosticLevel = 0;" >> iterator.igen.c
echo "    bhmInstallDiagCB(setDiagLevel);" >> iterator.igen.c
echo "    constructor();" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "    while(1){" >> iterator.igen.c
echo "        bhmWaitDelay(QUANTUM_DELAY);" >> iterator.igen.c
echo "        runIterations();" >> iterator.igen.c
echo "    }" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "" >> iterator.igen.c
echo "    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));" >> iterator.igen.c
echo "    destructor();" >> iterator.igen.c
echo "    return 0;" >> iterator.igen.c
echo "}" >> iterator.igen.c