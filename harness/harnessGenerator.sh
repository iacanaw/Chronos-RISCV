#!/bin/sh

# Parameters
X=$1
Y=$2
N=$((X*Y))
# Removes the old .tcl
rm -rf harness.c

echo "#include <stdio.h>" >> harness.c
echo "#include <stdlib.h>" >> harness.c
echo "#include <string.h>" >> harness.c
echo "" >> harness.c
echo "#include \"op/op.h\"" >> harness.c
echo "" >> harness.c
echo "#define MODULE_NAME \"top\"" >> harness.c
echo "#define MODULE_DIR \"module\"" >> harness.c
echo "#define MODULE_INSTANCE \"u2\"" >> harness.c
echo "#define N_PES "$N >> harness.c
echo "" >> harness.c
echo "// Quantum defines" >> harness.c
echo "#define INSTRUCTIONS_PER_SECOND 1000000000.0                                            // 1GHz (assuming 1 instruction per cycle)" >> harness.c
echo "#define INSTRUCTIONS_PER_TIME_SLICE 10000.0                                             //(INSTRUCTIONS_PER_SECOND*QUANTUM_TIME_SLICE)" >> harness.c
echo "#define QUANTUM_TIME_SLICE (double)(INSTRUCTIONS_PER_TIME_SLICE / INSTRUCTIONS_PER_SECOND)" >> harness.c
echo "" >> harness.c
echo "// Address to save the instruction counters" >> harness.c
echo "#define LOAD_ADDR       0x8FFFFFF4u" >> harness.c
echo "#define STORE_ADDR      0x8FFFFFF0u" >> harness.c
echo "#define OTHERS_ADDR     0x8FFFFFECu" >> harness.c
echo "" >> harness.c
N=$(($N-1))
for i in $(seq 0 $N);
do
    echo "unsigned int load"$i" = 0;" >> harness.c
    echo "unsigned int store"$i" = 0;" >> harness.c
    echo "unsigned int others"$i" = 0;" >> harness.c
    echo "unsigned int fetch"$i" = 0;" >> harness.c
done
echo "" >> harness.c
echo "struct optionsS {" >> harness.c
echo "	Bool configurecpuinstance;" >> harness.c
echo "} options = {" >> harness.c
echo "    .configurecpuinstance = False," >> harness.c
echo "};" >> harness.c
echo "" >> harness.c
echo "unsigned int peCount[N_PES];" >> harness.c
echo "double pe_DeRate[N_PES];" >> harness.c
echo "double myTime = 0;" >> harness.c
echo "" >> harness.c
echo "unsigned int vec2usi(char *vec){" >> harness.c
echo "    unsigned int auxValue = 0x00000000;" >> harness.c
echo "    unsigned int aux;" >> harness.c
echo "    aux = 0x000000FF & vec[3]; " >> harness.c
echo "    auxValue = ((aux << 24) & 0xFF000000);" >> harness.c
echo "    aux = 0x000000FF & vec[2];" >> harness.c
echo "    auxValue = auxValue | ((aux << 16) & 0x00FF0000);" >> harness.c
echo "    aux = 0x000000FF & vec[1];" >> harness.c
echo "    auxValue = auxValue | ((aux << 8) & 0x0000FF00);" >> harness.c
echo "    aux = 0x000000FF & vec[0];" >> harness.c
echo "    auxValue = auxValue | ((aux) & 0x000000FF);" >> harness.c
echo "    return auxValue;" >> harness.c
echo "}" >> harness.c
echo "" >> harness.c
echo "static OP_POST_SIMULATE_FN(modulePostSimulate) {" >> harness.c
echo "// insert modulePostSimulate code here" >> harness.c
echo "}" >> harness.c
echo "" >> harness.c
echo "static OP_DESTRUCT_FN(moduleDestruct) {" >> harness.c
echo "// insert moduleDestruct code here" >> harness.c
echo "}" >> harness.c
echo "" >> harness.c
echo "/*Attributes Set in Module Construction */" >> harness.c
echo "optModuleAttr modelAttrs = {" >> harness.c
echo "    .versionString = OP_VERSION," >> harness.c
echo "    .type = OP_MODULE," >> harness.c
echo "    .name = MODULE_NAME," >> harness.c
echo "    .releaseStatus = OP_UNSET," >> harness.c
echo "    .purpose = OP_PP_BAREMETAL," >> harness.c
echo "    .visibility = OP_VISIBLE," >> harness.c
echo "    //.constructCB          = moduleConstruct," >> harness.c
echo "    .postSimulateCB = modulePostSimulate," >> harness.c
echo "    .destructCB = moduleDestruct," >> harness.c
echo "};" >> harness.c
echo "" >> harness.c
echo "/*int getProcessorID(optProcessorP processor){" >> harness.c
echo "    int processorID;" >> harness.c
echo "    char processorName[7] = \"@@@@@@@\";" >> harness.c
echo "    strcpy(processorName,opObjectName(processor)); " >> harness.c
echo "    if(((int)processorName[5] - 48) >= 0 && ((int)processorName[5] - 48) <= 9){" >> harness.c
echo "        processorID = ((int)processorName[3] - 48)*100 + ((int)processorName[4] - 48)*10 + ((int)processorName[5] - 48);" >> harness.c
echo "    }" >> harness.c
echo "    else if(((int)processorName[4] - 48) >= 0 && ((int)processorName[4] - 48) <= 9){" >> harness.c
echo "        processorID = ((int)processorName[3] - 48)*10 + ((int)processorName[4] - 48);" >> harness.c
echo "    }" >> harness.c
echo "    else processorID = ((int)processorName[3] - 48);" >> harness.c
echo "    //ERROR CATCHER!" >> harness.c
echo "    if(processorID < 0 || processorID > N_PES){" >> harness.c
echo "        opMessage(\"I\", \"FETCH CALLBACK\", \"~~~~> Ocorreu um erro! %d\",processorID);" >> harness.c
echo "        while(1){}     " >> harness.c
echo "    }" >> harness.c
echo "    return processorID;" >> harness.c
echo "}*/" >> harness.c
echo "" >> harness.c
echo "// unsigned int saiu_int = 0;" >> harness.c
echo "// unsigned int entrou_int = 0;" >> harness.c
echo "" >> harness.c
for i in $(seq 0 $N);
do
    echo "static OP_MONITOR_FN(FetchCallback"$i") { " >> harness.c
    echo "    char aux_8bits[4];" >> harness.c
    echo "    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "    unsigned int instruction32 = vec2usi(aux_8bits);" >> harness.c
    echo "    fetch"$i"++;" >> harness.c
    echo "" >> harness.c
    echo "    //opMessage(\"I\", \"FETCH\", \"PE%d- %s @ %x\", "$i", opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);" >> harness.c
    echo "    " >> harness.c
    echo "    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60" >> harness.c
    echo "    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) " >> harness.c
    echo "        load"$i"++;" >> harness.c
    echo "    }" >> harness.c
    echo "    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)" >> harness.c
    echo "        store"$i"++;" >> harness.c
    echo "    }" >> harness.c
    echo "    else{" >> harness.c
    echo "        others"$i"++;" >> harness.c
    echo "    }" >> harness.c
    echo "" >> harness.c
    echo "    if(fetch"$i" > 100){" >> harness.c
    echo "        fetch"$i" = 0;" >> harness.c
    echo "" >> harness.c
    echo "        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        load"$i" = load"$i" + vec2usi(aux_8bits);" >> harness.c
    echo "        aux_8bits[3] = (load"$i" >> 24) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[2] = (load"$i" >> 16) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[1] = (load"$i" >> 8) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[0] =  load"$i" & 0x000000FF;" >> harness.c
    echo "        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        load"$i" = 0;" >> harness.c
    echo "" >> harness.c
    echo "        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        store"$i" = store"$i" + vec2usi(aux_8bits);" >> harness.c
    echo "        aux_8bits[3] = (store"$i" >> 24) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[2] = (store"$i" >> 16) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[1] = (store"$i" >> 8) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[0] =  store"$i" & 0x000000FF;" >> harness.c
    echo "        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        store"$i" = 0;" >> harness.c
    echo "" >> harness.c
    echo "        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        others"$i" = others"$i" + vec2usi(aux_8bits);" >> harness.c
    echo "        aux_8bits[3] = (others"$i" >> 24) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[2] = (others"$i" >> 16) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[1] = (others"$i" >> 8) & 0x000000FF;" >> harness.c
    echo "        aux_8bits[0] =  others"$i" & 0x000000FF;" >> harness.c
    echo "        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "        others"$i" = 0;" >> harness.c
    echo "    }" >> harness.c
    echo "" >> harness.c
    echo "    return;" >> harness.c
    echo "}" >> harness.c
done
echo "" >> harness.c
echo "static OP_MONITOR_FN(FinishCallback){" >> harness.c
echo "    opMessage(\"I\", \"HARNESS\", \" >>> SIMULAÇÃO FINALIZADA COM SUCESSO!\");" >> harness.c
echo "    opProcessorFinish(processor, 0);" >> harness.c
echo "    return;" >> harness.c
echo "}" >> harness.c
echo "" >> harness.c
for i in $(seq 0 $N);
do
    echo "static OP_MONITOR_FN(FreqCallback"$i"){" >> harness.c
    echo "" >> harness.c
    echo "    // reads the frequency scale, provided by the system" >> harness.c
    echo "    char readFreq_8bit[4];" >> harness.c
    echo "    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
    echo "    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);" >> harness.c
    echo "" >> harness.c
    echo "    // translates from fixed point to float" >> harness.c
    echo "    float freqScale = 100 - (float)((float)readFreq_32bit / 10);" >> harness.c
    echo "    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency" >> harness.c
    echo "        freqScale = 90;" >> harness.c
    echo "    }" >> harness.c
    echo "    opMessage(\"I\", \"HARNESS\", \" >>> PE "$i" changing to %.1f%% of its nominal frequency -- (%.0fMHz)\", 100-freqScale, 10*(100-freqScale));" >> harness.c
    echo "    opProcessorDerate(processor, freqScale);" >> harness.c
    echo "    return;" >> harness.c
    echo "}" >> harness.c
    echo "" >> harness.c
done
echo "int main(int argc, const char *argv[]) {" >> harness.c
echo "    int             runningPE       = 0;" >> harness.c
echo "    optProcessorP   stopProcessor   = 0;" >> harness.c
echo "    Bool            finished        = False;" >> harness.c
echo "	optProcessorP proc;" >> harness.c
echo "    opSessionInit(OP_VERSION);" >> harness.c
echo "    /* create the root module with reduced Quantum (in line with Custom Scheduler) */" >> harness.c
echo "	optParamP params = OP_PARAMS(OP_PARAM_DOUBLE_SET(OP_FP_QUANTUM, QUANTUM_TIME_SLICE));" >> harness.c
echo "    optCmdParserP parser = opCmdParserNew(MODULE_NAME, OP_AC_ALL);" >> harness.c
echo "    opCmdParserAdd(parser, \"configurecpuinstance\", 0, \"bool\", \"user\", OP_FT_BOOLVAL, &options.configurecpuinstance, \"Add configuration to enable Imperas Intercepts to CPU instance\", OP_AC_ALL, 0, 0);  // enable interception" >> harness.c
echo "    if (!opCmdParseArgs(parser, argc, argv)) {" >> harness.c
echo "		opMessage(\"E\", MODULE_NAME, \"Command line parse incomplete\");" >> harness.c
echo "	}" >> harness.c
echo "    opCmdParseArgs(parser, argc, argv);" >> harness.c
echo "    optModuleP mi = opRootModuleNew(&modelAttrs, MODULE_NAME, params);" >> harness.c
echo "    optModuleP modNew = opModuleNew(mi, MODULE_DIR, MODULE_INSTANCE, 0, 0);" >> harness.c
echo "" >> harness.c
echo "    while ((proc = opProcessorNext(modNew, proc))) {" >> harness.c
echo "        char id[4];" >> harness.c
echo "        id[3] = (runningPE >> 24) & 0x000000FF;" >> harness.c
echo "        id[2] = (runningPE >> 16) & 0x000000FF;" >> harness.c
echo "        id[1] = (runningPE >> 8) & 0x000000FF;" >> harness.c
echo "        id[0] = runningPE & 0x000000FF;" >> harness.c
echo "        opProcessorWrite(proc, 0x8FFFFFFC, id, 4, 1, True, OP_HOSTENDIAN_TARGET);" >> harness.c
echo "" >> harness.c
echo "        // Go to the next processor" >> harness.c
echo "        opMessage(\"I\", \"HARNESS INFO\", \"================== INICIALIZANDO PE %d ==================\", runningPE);" >> harness.c
echo "    " >> harness.c
echo "        if (runningPE == 0){" >> harness.c
echo "            opMessage(\"I\", \"HARNESS INFO\", \"\t > MONITOR DE FINALIZAÇÃO ADICIONADO!\");" >> harness.c
echo "            opProcessorFetchMonitorAdd(proc, 0x80000da4, 0x80000df0, FinishCallback, \"finish\");" >> harness.c
echo "        }" >> harness.c
echo "        " >> harness.c
echo "        switch(runningPE){" >> harness.c
for i in $(seq 0 $N);
do
    echo "            case "$i":" >> harness.c
    echo "                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback"$i", \"frequency"$i"\");" >> harness.c
    echo "                opMessage(\"I\", \"HARNESS INFO\", \"\t > MONITOR DE FREQUENCIA "$i" ADICIONADO!\");" >> harness.c

    echo "                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback"$i", \"fetch"$i"\");" >> harness.c
    echo "                opMessage(\"I\", \"HARNESS INFO\", \"	 > MONITOR DE FETCH "$i" ADICIONADO!\");" >> harness.c
    echo "                break;" >> harness.c
echo "" >> harness.c
done
echo "            default: " >> harness.c
echo "                break;" >> harness.c
echo "        }" >> harness.c
echo "        runningPE++;" >> harness.c
echo "    }" >> harness.c
echo "" >> harness.c
echo "    /* Simulation loop */" >> harness.c
echo "    while(!finished) {" >> harness.c
echo "        myTime += 0.1;" >> harness.c
echo "        opRootModuleSetSimulationStopTime(mi, myTime);" >> harness.c
echo "        stopProcessor = opRootModuleSimulate(mi);" >> harness.c
echo "        " >> harness.c
echo "        optStopReason sr = stopProcessor ? opProcessorStopReason(stopProcessor)" >> harness.c
echo "                                         : OP_SR_EXIT;" >> harness.c
echo "        switch(sr) {" >> harness.c
echo "            case OP_SR_EXIT:" >> harness.c
echo "                opMessage(\"I\", \"HARNESS\", \"Simulation time: %.1f seconds elapsed...\", myTime);" >> harness.c
echo "                break; " >> harness.c
echo "            " >> harness.c
echo "            case OP_SR_FINISH:" >> harness.c
echo "                finished = True;" >> harness.c
echo "                break;" >> harness.c
echo "            " >> harness.c
echo "            default:" >> harness.c
echo "                opMessage(\"I\", \"HARNESS\", \"Processor %s icount %u stopped with unexpected reason 0x%x (%s)\", opObjectName (stopProcessor), (Uns32)opProcessorICount(stopProcessor), sr, opStopReasonString(sr) );" >> harness.c
echo "                break;" >> harness.c
echo "        }" >> harness.c
echo "        " >> harness.c
echo "    }" >> harness.c
echo "" >> harness.c
echo "    opSessionTerminate();" >> harness.c
echo "" >> harness.c
echo "    return (opErrors() ? 1 : 0);    // set exit based upon any errors" >> harness.c
echo "}" >> harness.c
echo "" >> harness.c
