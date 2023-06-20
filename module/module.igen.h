/*
 * Copyright (c) 2005-2021 Imperas Software Ltd., www.imperas.com
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
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////

#define UNUSED   __attribute__((unused))

// instantiate module components
static OP_CONSTRUCT_FN(instantiateComponents) {



    // Bus cpu0Bus

    optBusP cpu0Bus_b = opBusNew(mi, "cpu0Bus", 32, 0, 0);


    // Bus cpu1Bus

    optBusP cpu1Bus_b = opBusNew(mi, "cpu1Bus", 32, 0, 0);


    // Bus cpu2Bus

    optBusP cpu2Bus_b = opBusNew(mi, "cpu2Bus", 32, 0, 0);


    // Bus cpu3Bus

    optBusP cpu3Bus_b = opBusNew(mi, "cpu3Bus", 32, 0, 0);


    // Bus cpu4Bus

    optBusP cpu4Bus_b = opBusNew(mi, "cpu4Bus", 32, 0, 0);


    // Bus cpu5Bus

    optBusP cpu5Bus_b = opBusNew(mi, "cpu5Bus", 32, 0, 0);


    // Bus cpu6Bus

    optBusP cpu6Bus_b = opBusNew(mi, "cpu6Bus", 32, 0, 0);


    // Bus cpu7Bus

    optBusP cpu7Bus_b = opBusNew(mi, "cpu7Bus", 32, 0, 0);


    // Bus cpu8Bus

    optBusP cpu8Bus_b = opBusNew(mi, "cpu8Bus", 32, 0, 0);


    // Bus cpu9Bus

    optBusP cpu9Bus_b = opBusNew(mi, "cpu9Bus", 32, 0, 0);


    // Bus cpu10Bus

    optBusP cpu10Bus_b = opBusNew(mi, "cpu10Bus", 32, 0, 0);


    // Bus cpu11Bus

    optBusP cpu11Bus_b = opBusNew(mi, "cpu11Bus", 32, 0, 0);


    // Bus cpu12Bus

    optBusP cpu12Bus_b = opBusNew(mi, "cpu12Bus", 32, 0, 0);


    // Bus cpu13Bus

    optBusP cpu13Bus_b = opBusNew(mi, "cpu13Bus", 32, 0, 0);


    // Bus cpu14Bus

    optBusP cpu14Bus_b = opBusNew(mi, "cpu14Bus", 32, 0, 0);


    // Bus cpu15Bus

    optBusP cpu15Bus_b = opBusNew(mi, "cpu15Bus", 32, 0, 0);


////////////////////////////////////////////////////////////////////////////////
//                                    NETS
////////////////////////////////////////////////////////////////////////////////

    optNetP eip0_n = opNetNew(mi, "eip0", 0, 0);
    optNetP eip1_n = opNetNew(mi, "eip1", 0, 0);
    optNetP eip2_n = opNetNew(mi, "eip2", 0, 0);
    optNetP eip3_n = opNetNew(mi, "eip3", 0, 0);
    optNetP eip4_n = opNetNew(mi, "eip4", 0, 0);
    optNetP eip5_n = opNetNew(mi, "eip5", 0, 0);
    optNetP eip6_n = opNetNew(mi, "eip6", 0, 0);
    optNetP eip7_n = opNetNew(mi, "eip7", 0, 0);
    optNetP eip8_n = opNetNew(mi, "eip8", 0, 0);
    optNetP eip9_n = opNetNew(mi, "eip9", 0, 0);
    optNetP eip10_n = opNetNew(mi, "eip10", 0, 0);
    optNetP eip11_n = opNetNew(mi, "eip11", 0, 0);
    optNetP eip12_n = opNetNew(mi, "eip12", 0, 0);
    optNetP eip13_n = opNetNew(mi, "eip13", 0, 0);
    optNetP eip14_n = opNetNew(mi, "eip14", 0, 0);
    optNetP eip15_n = opNetNew(mi, "eip15", 0, 0);
    optNetP MTimerInterrupt0_n = opNetNew(mi, "MTimerInterrupt0", 0, 0);
    optNetP MSWInterrupt0_n = opNetNew(mi, "MSWInterrupt0", 0, 0);
    optNetP intNI_RX0_n = opNetNew(mi, "intNI_RX0", 0, 0);
    optNetP intNI_TX0_n = opNetNew(mi, "intNI_TX0", 0, 0);
    optNetP intNI_TMR0_n = opNetNew(mi, "intNI_TMR0", 0, 0);
    optNetP MTimerInterrupt1_n = opNetNew(mi, "MTimerInterrupt1", 0, 0);
    optNetP MSWInterrupt1_n = opNetNew(mi, "MSWInterrupt1", 0, 0);
    optNetP intNI_RX1_n = opNetNew(mi, "intNI_RX1", 0, 0);
    optNetP intNI_TX1_n = opNetNew(mi, "intNI_TX1", 0, 0);
    optNetP intNI_TMR1_n = opNetNew(mi, "intNI_TMR1", 0, 0);
    optNetP MTimerInterrupt2_n = opNetNew(mi, "MTimerInterrupt2", 0, 0);
    optNetP MSWInterrupt2_n = opNetNew(mi, "MSWInterrupt2", 0, 0);
    optNetP intNI_RX2_n = opNetNew(mi, "intNI_RX2", 0, 0);
    optNetP intNI_TX2_n = opNetNew(mi, "intNI_TX2", 0, 0);
    optNetP intNI_TMR2_n = opNetNew(mi, "intNI_TMR2", 0, 0);
    optNetP MTimerInterrupt3_n = opNetNew(mi, "MTimerInterrupt3", 0, 0);
    optNetP MSWInterrupt3_n = opNetNew(mi, "MSWInterrupt3", 0, 0);
    optNetP intNI_RX3_n = opNetNew(mi, "intNI_RX3", 0, 0);
    optNetP intNI_TX3_n = opNetNew(mi, "intNI_TX3", 0, 0);
    optNetP intNI_TMR3_n = opNetNew(mi, "intNI_TMR3", 0, 0);
    optNetP MTimerInterrupt4_n = opNetNew(mi, "MTimerInterrupt4", 0, 0);
    optNetP MSWInterrupt4_n = opNetNew(mi, "MSWInterrupt4", 0, 0);
    optNetP intNI_RX4_n = opNetNew(mi, "intNI_RX4", 0, 0);
    optNetP intNI_TX4_n = opNetNew(mi, "intNI_TX4", 0, 0);
    optNetP intNI_TMR4_n = opNetNew(mi, "intNI_TMR4", 0, 0);
    optNetP MTimerInterrupt5_n = opNetNew(mi, "MTimerInterrupt5", 0, 0);
    optNetP MSWInterrupt5_n = opNetNew(mi, "MSWInterrupt5", 0, 0);
    optNetP intNI_RX5_n = opNetNew(mi, "intNI_RX5", 0, 0);
    optNetP intNI_TX5_n = opNetNew(mi, "intNI_TX5", 0, 0);
    optNetP intNI_TMR5_n = opNetNew(mi, "intNI_TMR5", 0, 0);
    optNetP MTimerInterrupt6_n = opNetNew(mi, "MTimerInterrupt6", 0, 0);
    optNetP MSWInterrupt6_n = opNetNew(mi, "MSWInterrupt6", 0, 0);
    optNetP intNI_RX6_n = opNetNew(mi, "intNI_RX6", 0, 0);
    optNetP intNI_TX6_n = opNetNew(mi, "intNI_TX6", 0, 0);
    optNetP intNI_TMR6_n = opNetNew(mi, "intNI_TMR6", 0, 0);
    optNetP MTimerInterrupt7_n = opNetNew(mi, "MTimerInterrupt7", 0, 0);
    optNetP MSWInterrupt7_n = opNetNew(mi, "MSWInterrupt7", 0, 0);
    optNetP intNI_RX7_n = opNetNew(mi, "intNI_RX7", 0, 0);
    optNetP intNI_TX7_n = opNetNew(mi, "intNI_TX7", 0, 0);
    optNetP intNI_TMR7_n = opNetNew(mi, "intNI_TMR7", 0, 0);
    optNetP MTimerInterrupt8_n = opNetNew(mi, "MTimerInterrupt8", 0, 0);
    optNetP MSWInterrupt8_n = opNetNew(mi, "MSWInterrupt8", 0, 0);
    optNetP intNI_RX8_n = opNetNew(mi, "intNI_RX8", 0, 0);
    optNetP intNI_TX8_n = opNetNew(mi, "intNI_TX8", 0, 0);
    optNetP intNI_TMR8_n = opNetNew(mi, "intNI_TMR8", 0, 0);
    optNetP MTimerInterrupt9_n = opNetNew(mi, "MTimerInterrupt9", 0, 0);
    optNetP MSWInterrupt9_n = opNetNew(mi, "MSWInterrupt9", 0, 0);
    optNetP intNI_RX9_n = opNetNew(mi, "intNI_RX9", 0, 0);
    optNetP intNI_TX9_n = opNetNew(mi, "intNI_TX9", 0, 0);
    optNetP intNI_TMR9_n = opNetNew(mi, "intNI_TMR9", 0, 0);
    optNetP MTimerInterrupt10_n = opNetNew(mi, "MTimerInterrupt10", 0, 0);
    optNetP MSWInterrupt10_n = opNetNew(mi, "MSWInterrupt10", 0, 0);
    optNetP intNI_RX10_n = opNetNew(mi, "intNI_RX10", 0, 0);
    optNetP intNI_TX10_n = opNetNew(mi, "intNI_TX10", 0, 0);
    optNetP intNI_TMR10_n = opNetNew(mi, "intNI_TMR10", 0, 0);
    optNetP MTimerInterrupt11_n = opNetNew(mi, "MTimerInterrupt11", 0, 0);
    optNetP MSWInterrupt11_n = opNetNew(mi, "MSWInterrupt11", 0, 0);
    optNetP intNI_RX11_n = opNetNew(mi, "intNI_RX11", 0, 0);
    optNetP intNI_TX11_n = opNetNew(mi, "intNI_TX11", 0, 0);
    optNetP intNI_TMR11_n = opNetNew(mi, "intNI_TMR11", 0, 0);
    optNetP MTimerInterrupt12_n = opNetNew(mi, "MTimerInterrupt12", 0, 0);
    optNetP MSWInterrupt12_n = opNetNew(mi, "MSWInterrupt12", 0, 0);
    optNetP intNI_RX12_n = opNetNew(mi, "intNI_RX12", 0, 0);
    optNetP intNI_TX12_n = opNetNew(mi, "intNI_TX12", 0, 0);
    optNetP intNI_TMR12_n = opNetNew(mi, "intNI_TMR12", 0, 0);
    optNetP MTimerInterrupt13_n = opNetNew(mi, "MTimerInterrupt13", 0, 0);
    optNetP MSWInterrupt13_n = opNetNew(mi, "MSWInterrupt13", 0, 0);
    optNetP intNI_RX13_n = opNetNew(mi, "intNI_RX13", 0, 0);
    optNetP intNI_TX13_n = opNetNew(mi, "intNI_TX13", 0, 0);
    optNetP intNI_TMR13_n = opNetNew(mi, "intNI_TMR13", 0, 0);
    optNetP MTimerInterrupt14_n = opNetNew(mi, "MTimerInterrupt14", 0, 0);
    optNetP MSWInterrupt14_n = opNetNew(mi, "MSWInterrupt14", 0, 0);
    optNetP intNI_RX14_n = opNetNew(mi, "intNI_RX14", 0, 0);
    optNetP intNI_TX14_n = opNetNew(mi, "intNI_TX14", 0, 0);
    optNetP intNI_TMR14_n = opNetNew(mi, "intNI_TMR14", 0, 0);
    optNetP MTimerInterrupt15_n = opNetNew(mi, "MTimerInterrupt15", 0, 0);
    optNetP MSWInterrupt15_n = opNetNew(mi, "MSWInterrupt15", 0, 0);
    optNetP intNI_RX15_n = opNetNew(mi, "intNI_RX15", 0, 0);
    optNetP intNI_TX15_n = opNetNew(mi, "intNI_TX15", 0, 0);
    optNetP intNI_TMR15_n = opNetNew(mi, "intNI_TMR15", 0, 0);

    optPacketnetP data_0_0_TEA_pkn = opPacketnetNew(mi, "data_0_0_TEA", 0, 0);

    optPacketnetP ctrl_0_0_TEA_pkn = opPacketnetNew(mi, "ctrl_0_0_TEA", 0, 0);

    optPacketnetP data_0_0_REPOSITORY_pkn = opPacketnetNew(mi, "data_0_0_REPOSITORY", 0, 0);

    optPacketnetP ctrl_0_0_REPOSITORY_pkn = opPacketnetNew(mi, "ctrl_0_0_REPOSITORY", 0, 0);

    optPacketnetP data_0_0_L_pkn = opPacketnetNew(mi, "data_0_0_L", 0, 0);

    optPacketnetP ctrl_0_0_L_pkn = opPacketnetNew(mi, "ctrl_0_0_L", 0, 0);

    optPacketnetP data_0_0_E_pkn = opPacketnetNew(mi, "data_0_0_E", 0, 0);

    opPacketnetNew(mi, "data_0_0_W", 0, 0);

    optPacketnetP data_0_0_N_pkn = opPacketnetNew(mi, "data_0_0_N", 0, 0);

    opPacketnetNew(mi, "data_0_0_S", 0, 0);

    optPacketnetP ctrl_0_0_E_pkn = opPacketnetNew(mi, "ctrl_0_0_E", 0, 0);

    opPacketnetNew(mi, "ctrl_0_0_W", 0, 0);

    optPacketnetP ctrl_0_0_N_pkn = opPacketnetNew(mi, "ctrl_0_0_N", 0, 0);

    opPacketnetNew(mi, "ctrl_0_0_S", 0, 0);

    optPacketnetP data_0_1_L_pkn = opPacketnetNew(mi, "data_0_1_L", 0, 0);

    optPacketnetP ctrl_0_1_L_pkn = opPacketnetNew(mi, "ctrl_0_1_L", 0, 0);

    optPacketnetP data_0_2_L_pkn = opPacketnetNew(mi, "data_0_2_L", 0, 0);

    optPacketnetP ctrl_0_2_L_pkn = opPacketnetNew(mi, "ctrl_0_2_L", 0, 0);

    optPacketnetP data_0_2_E_pkn = opPacketnetNew(mi, "data_0_2_E", 0, 0);

    optPacketnetP data_0_2_W_pkn = opPacketnetNew(mi, "data_0_2_W", 0, 0);

    optPacketnetP data_0_2_N_pkn = opPacketnetNew(mi, "data_0_2_N", 0, 0);

    opPacketnetNew(mi, "data_0_2_S", 0, 0);

    optPacketnetP ctrl_0_2_E_pkn = opPacketnetNew(mi, "ctrl_0_2_E", 0, 0);

    optPacketnetP ctrl_0_2_W_pkn = opPacketnetNew(mi, "ctrl_0_2_W", 0, 0);

    optPacketnetP ctrl_0_2_N_pkn = opPacketnetNew(mi, "ctrl_0_2_N", 0, 0);

    opPacketnetNew(mi, "ctrl_0_2_S", 0, 0);

    optPacketnetP data_0_3_L_pkn = opPacketnetNew(mi, "data_0_3_L", 0, 0);

    optPacketnetP ctrl_0_3_L_pkn = opPacketnetNew(mi, "ctrl_0_3_L", 0, 0);

    optPacketnetP data_1_0_L_pkn = opPacketnetNew(mi, "data_1_0_L", 0, 0);

    optPacketnetP ctrl_1_0_L_pkn = opPacketnetNew(mi, "ctrl_1_0_L", 0, 0);

    optPacketnetP data_1_1_L_pkn = opPacketnetNew(mi, "data_1_1_L", 0, 0);

    optPacketnetP ctrl_1_1_L_pkn = opPacketnetNew(mi, "ctrl_1_1_L", 0, 0);

    optPacketnetP data_1_1_E_pkn = opPacketnetNew(mi, "data_1_1_E", 0, 0);

    optPacketnetP data_1_1_W_pkn = opPacketnetNew(mi, "data_1_1_W", 0, 0);

    optPacketnetP data_1_1_N_pkn = opPacketnetNew(mi, "data_1_1_N", 0, 0);

    optPacketnetP data_1_1_S_pkn = opPacketnetNew(mi, "data_1_1_S", 0, 0);

    optPacketnetP ctrl_1_1_E_pkn = opPacketnetNew(mi, "ctrl_1_1_E", 0, 0);

    optPacketnetP ctrl_1_1_W_pkn = opPacketnetNew(mi, "ctrl_1_1_W", 0, 0);

    optPacketnetP ctrl_1_1_N_pkn = opPacketnetNew(mi, "ctrl_1_1_N", 0, 0);

    optPacketnetP ctrl_1_1_S_pkn = opPacketnetNew(mi, "ctrl_1_1_S", 0, 0);

    optPacketnetP data_1_2_L_pkn = opPacketnetNew(mi, "data_1_2_L", 0, 0);

    optPacketnetP ctrl_1_2_L_pkn = opPacketnetNew(mi, "ctrl_1_2_L", 0, 0);

    optPacketnetP data_1_3_L_pkn = opPacketnetNew(mi, "data_1_3_L", 0, 0);

    optPacketnetP ctrl_1_3_L_pkn = opPacketnetNew(mi, "ctrl_1_3_L", 0, 0);

    opPacketnetNew(mi, "data_1_3_E", 0, 0);

    optPacketnetP data_1_3_W_pkn = opPacketnetNew(mi, "data_1_3_W", 0, 0);

    optPacketnetP data_1_3_N_pkn = opPacketnetNew(mi, "data_1_3_N", 0, 0);

    optPacketnetP data_1_3_S_pkn = opPacketnetNew(mi, "data_1_3_S", 0, 0);

    opPacketnetNew(mi, "ctrl_1_3_E", 0, 0);

    optPacketnetP ctrl_1_3_W_pkn = opPacketnetNew(mi, "ctrl_1_3_W", 0, 0);

    optPacketnetP ctrl_1_3_N_pkn = opPacketnetNew(mi, "ctrl_1_3_N", 0, 0);

    optPacketnetP ctrl_1_3_S_pkn = opPacketnetNew(mi, "ctrl_1_3_S", 0, 0);

    optPacketnetP data_2_0_L_pkn = opPacketnetNew(mi, "data_2_0_L", 0, 0);

    optPacketnetP ctrl_2_0_L_pkn = opPacketnetNew(mi, "ctrl_2_0_L", 0, 0);

    optPacketnetP data_2_0_E_pkn = opPacketnetNew(mi, "data_2_0_E", 0, 0);

    opPacketnetNew(mi, "data_2_0_W", 0, 0);

    optPacketnetP data_2_0_N_pkn = opPacketnetNew(mi, "data_2_0_N", 0, 0);

    optPacketnetP data_2_0_S_pkn = opPacketnetNew(mi, "data_2_0_S", 0, 0);

    optPacketnetP ctrl_2_0_E_pkn = opPacketnetNew(mi, "ctrl_2_0_E", 0, 0);

    opPacketnetNew(mi, "ctrl_2_0_W", 0, 0);

    optPacketnetP ctrl_2_0_N_pkn = opPacketnetNew(mi, "ctrl_2_0_N", 0, 0);

    optPacketnetP ctrl_2_0_S_pkn = opPacketnetNew(mi, "ctrl_2_0_S", 0, 0);

    optPacketnetP data_2_1_L_pkn = opPacketnetNew(mi, "data_2_1_L", 0, 0);

    optPacketnetP ctrl_2_1_L_pkn = opPacketnetNew(mi, "ctrl_2_1_L", 0, 0);

    optPacketnetP data_2_2_L_pkn = opPacketnetNew(mi, "data_2_2_L", 0, 0);

    optPacketnetP ctrl_2_2_L_pkn = opPacketnetNew(mi, "ctrl_2_2_L", 0, 0);

    optPacketnetP data_2_2_E_pkn = opPacketnetNew(mi, "data_2_2_E", 0, 0);

    optPacketnetP data_2_2_W_pkn = opPacketnetNew(mi, "data_2_2_W", 0, 0);

    optPacketnetP data_2_2_N_pkn = opPacketnetNew(mi, "data_2_2_N", 0, 0);

    optPacketnetP data_2_2_S_pkn = opPacketnetNew(mi, "data_2_2_S", 0, 0);

    optPacketnetP ctrl_2_2_E_pkn = opPacketnetNew(mi, "ctrl_2_2_E", 0, 0);

    optPacketnetP ctrl_2_2_W_pkn = opPacketnetNew(mi, "ctrl_2_2_W", 0, 0);

    optPacketnetP ctrl_2_2_N_pkn = opPacketnetNew(mi, "ctrl_2_2_N", 0, 0);

    optPacketnetP ctrl_2_2_S_pkn = opPacketnetNew(mi, "ctrl_2_2_S", 0, 0);

    optPacketnetP data_2_3_L_pkn = opPacketnetNew(mi, "data_2_3_L", 0, 0);

    optPacketnetP ctrl_2_3_L_pkn = opPacketnetNew(mi, "ctrl_2_3_L", 0, 0);

    optPacketnetP data_3_0_L_pkn = opPacketnetNew(mi, "data_3_0_L", 0, 0);

    optPacketnetP ctrl_3_0_L_pkn = opPacketnetNew(mi, "ctrl_3_0_L", 0, 0);

    optPacketnetP data_3_1_L_pkn = opPacketnetNew(mi, "data_3_1_L", 0, 0);

    optPacketnetP ctrl_3_1_L_pkn = opPacketnetNew(mi, "ctrl_3_1_L", 0, 0);

    optPacketnetP data_3_1_E_pkn = opPacketnetNew(mi, "data_3_1_E", 0, 0);

    optPacketnetP data_3_1_W_pkn = opPacketnetNew(mi, "data_3_1_W", 0, 0);

    opPacketnetNew(mi, "data_3_1_N", 0, 0);

    optPacketnetP data_3_1_S_pkn = opPacketnetNew(mi, "data_3_1_S", 0, 0);

    optPacketnetP ctrl_3_1_E_pkn = opPacketnetNew(mi, "ctrl_3_1_E", 0, 0);

    optPacketnetP ctrl_3_1_W_pkn = opPacketnetNew(mi, "ctrl_3_1_W", 0, 0);

    opPacketnetNew(mi, "ctrl_3_1_N", 0, 0);

    optPacketnetP ctrl_3_1_S_pkn = opPacketnetNew(mi, "ctrl_3_1_S", 0, 0);

    optPacketnetP data_3_2_L_pkn = opPacketnetNew(mi, "data_3_2_L", 0, 0);

    optPacketnetP ctrl_3_2_L_pkn = opPacketnetNew(mi, "ctrl_3_2_L", 0, 0);

    optPacketnetP data_3_3_L_pkn = opPacketnetNew(mi, "data_3_3_L", 0, 0);

    optPacketnetP ctrl_3_3_L_pkn = opPacketnetNew(mi, "ctrl_3_3_L", 0, 0);

    opPacketnetNew(mi, "data_3_3_E", 0, 0);

    optPacketnetP data_3_3_W_pkn = opPacketnetNew(mi, "data_3_3_W", 0, 0);

    opPacketnetNew(mi, "data_3_3_N", 0, 0);

    optPacketnetP data_3_3_S_pkn = opPacketnetNew(mi, "data_3_3_S", 0, 0);

    opPacketnetNew(mi, "ctrl_3_3_E", 0, 0);

    optPacketnetP ctrl_3_3_W_pkn = opPacketnetNew(mi, "ctrl_3_3_W", 0, 0);

    opPacketnetNew(mi, "ctrl_3_3_N", 0, 0);

    optPacketnetP ctrl_3_3_S_pkn = opPacketnetNew(mi, "ctrl_3_3_S", 0, 0);

    optPacketnetP iteration_0_pkn = opPacketnetNew(mi, "iteration_0", 0, 0);

    optPacketnetP iteration_1_pkn = opPacketnetNew(mi, "iteration_1", 0, 0);

    optPacketnetP iteration_2_pkn = opPacketnetNew(mi, "iteration_2", 0, 0);

    optPacketnetP iteration_3_pkn = opPacketnetNew(mi, "iteration_3", 0, 0);

    optPacketnetP iteration_4_pkn = opPacketnetNew(mi, "iteration_4", 0, 0);

    optPacketnetP iteration_5_pkn = opPacketnetNew(mi, "iteration_5", 0, 0);

    optPacketnetP iteration_6_pkn = opPacketnetNew(mi, "iteration_6", 0, 0);

    optPacketnetP iteration_7_pkn = opPacketnetNew(mi, "iteration_7", 0, 0);

    optPacketnetP iteration_8_pkn = opPacketnetNew(mi, "iteration_8", 0, 0);

    optPacketnetP iteration_9_pkn = opPacketnetNew(mi, "iteration_9", 0, 0);

    optPacketnetP iteration_10_pkn = opPacketnetNew(mi, "iteration_10", 0, 0);

    optPacketnetP iteration_11_pkn = opPacketnetNew(mi, "iteration_11", 0, 0);

    optPacketnetP iteration_12_pkn = opPacketnetNew(mi, "iteration_12", 0, 0);

    optPacketnetP iteration_13_pkn = opPacketnetNew(mi, "iteration_13", 0, 0);

    optPacketnetP iteration_14_pkn = opPacketnetNew(mi, "iteration_14", 0, 0);

    optPacketnetP iteration_15_pkn = opPacketnetNew(mi, "iteration_15", 0, 0);

    // Processor cpu0

    const char *cpu0_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu0_path,
        "cpu0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu0Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip0_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt0_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt0_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu1

    const char *cpu1_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu1_path,
        "cpu1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu1Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip1_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt1_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt1_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 1)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu2

    const char *cpu2_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu2_path,
        "cpu2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu2Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip2_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt2_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt2_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 2)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu3

    const char *cpu3_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu3_path,
        "cpu3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu3Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip3_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt3_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt3_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 3)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu4

    const char *cpu4_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu4_path,
        "cpu4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu4Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip4_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt4_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt4_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 4)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu5

    const char *cpu5_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu5_path,
        "cpu5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu5Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip5_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt5_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt5_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 5)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu6

    const char *cpu6_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu6_path,
        "cpu6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu6Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip6_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt6_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt6_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 6)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu7

    const char *cpu7_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu7_path,
        "cpu7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu7Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip7_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt7_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt7_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 7)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu8

    const char *cpu8_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu8_path,
        "cpu8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu8Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip8_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt8_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt8_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 8)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu9

    const char *cpu9_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu9_path,
        "cpu9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu9Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip9_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt9_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt9_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 9)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu10

    const char *cpu10_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu10_path,
        "cpu10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu10Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip10_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt10_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt10_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 10)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu11

    const char *cpu11_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu11_path,
        "cpu11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu11Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip11_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt11_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt11_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 11)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu12

    const char *cpu12_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu12_path,
        "cpu12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu12Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip12_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt12_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt12_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 12)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu13

    const char *cpu13_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu13_path,
        "cpu13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu13Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip13_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt13_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt13_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 13)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu14

    const char *cpu14_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu14_path,
        "cpu14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu14Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip14_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt14_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt14_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 14)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Processor cpu15

    const char *cpu15_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "processor",
        "riscv",
        "1.0",
        OP_PROCESSOR,
        1   // report errors
    );

    opProcessorNew(
        mi,
        cpu15_path,
        "cpu15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "INSTRUCTION"),
                OP_BUS_CONNECT(cpu15Bus_b, "DATA")
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip15_n, "MExternalInterrupt"),
                OP_NET_CONNECT(MTimerInterrupt15_n, "MTimerInterrupt"),
                OP_NET_CONNECT(MSWInterrupt15_n, "MSWInterrupt")
            )
        ),
        OP_PARAMS(
             OP_PARAM_BOOL_SET("simulateexceptions", 1)
            ,OP_PARAM_UNS32_SET("cpuid", 15)
            ,OP_PARAM_DOUBLE_SET("mips", 1000)
            ,OP_PARAM_ENUM_SET("variant", "RVB32I")
            ,OP_PARAM_STRING_SET("add_Extensions", "MSU")
            ,OP_PARAM_BOOL_SET("external_int_id", True)
        )
    );


    // Memory nvram0

    opMemoryNew(
        mi,
        "nvram0",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr0

    opMemoryNew(
        mi,
        "ddr0",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk0

    opMemoryNew(
        mi,
        "stk0",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram1

    opMemoryNew(
        mi,
        "nvram1",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr1

    opMemoryNew(
        mi,
        "ddr1",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk1

    opMemoryNew(
        mi,
        "stk1",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram2

    opMemoryNew(
        mi,
        "nvram2",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr2

    opMemoryNew(
        mi,
        "ddr2",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk2

    opMemoryNew(
        mi,
        "stk2",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram3

    opMemoryNew(
        mi,
        "nvram3",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr3

    opMemoryNew(
        mi,
        "ddr3",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk3

    opMemoryNew(
        mi,
        "stk3",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram4

    opMemoryNew(
        mi,
        "nvram4",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr4

    opMemoryNew(
        mi,
        "ddr4",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk4

    opMemoryNew(
        mi,
        "stk4",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram5

    opMemoryNew(
        mi,
        "nvram5",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr5

    opMemoryNew(
        mi,
        "ddr5",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk5

    opMemoryNew(
        mi,
        "stk5",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram6

    opMemoryNew(
        mi,
        "nvram6",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr6

    opMemoryNew(
        mi,
        "ddr6",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk6

    opMemoryNew(
        mi,
        "stk6",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram7

    opMemoryNew(
        mi,
        "nvram7",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr7

    opMemoryNew(
        mi,
        "ddr7",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk7

    opMemoryNew(
        mi,
        "stk7",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram8

    opMemoryNew(
        mi,
        "nvram8",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr8

    opMemoryNew(
        mi,
        "ddr8",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk8

    opMemoryNew(
        mi,
        "stk8",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram9

    opMemoryNew(
        mi,
        "nvram9",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr9

    opMemoryNew(
        mi,
        "ddr9",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk9

    opMemoryNew(
        mi,
        "stk9",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram10

    opMemoryNew(
        mi,
        "nvram10",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr10

    opMemoryNew(
        mi,
        "ddr10",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk10

    opMemoryNew(
        mi,
        "stk10",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram11

    opMemoryNew(
        mi,
        "nvram11",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr11

    opMemoryNew(
        mi,
        "ddr11",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk11

    opMemoryNew(
        mi,
        "stk11",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram12

    opMemoryNew(
        mi,
        "nvram12",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr12

    opMemoryNew(
        mi,
        "ddr12",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk12

    opMemoryNew(
        mi,
        "stk12",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram13

    opMemoryNew(
        mi,
        "nvram13",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr13

    opMemoryNew(
        mi,
        "ddr13",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk13

    opMemoryNew(
        mi,
        "stk13",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram14

    opMemoryNew(
        mi,
        "nvram14",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr14

    opMemoryNew(
        mi,
        "ddr14",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk14

    opMemoryNew(
        mi,
        "stk14",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram15

    opMemoryNew(
        mi,
        "nvram15",
        OP_PRIV_RWX,
        (0x6fffffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6fffffffULL)
            )
        ),
        0
    );

    // Memory ddr15

    opMemoryNew(
        mi,
        "ddr15",
        OP_PRIV_RWX,
        (0x8fffffffULL) - (0x80000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "sp1", .slave=1, .addrLo=0x80000000ULL, .addrHi=0x8fffffffULL)
            )
        ),
        0
    );

    // Memory stk15

    opMemoryNew(
        mi,
        "stk15",
        OP_PRIV_RWX,
        (0xffffffffULL) - (0xf0000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "sp1", .slave=1, .addrLo=0xf0000000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // PSE tea

    const char *tea_path = "peripheral/tea/pse.pse";
    opPeripheralNew(
        mi,
        tea_path,
        "tea",
        OP_CONNECTIONS(
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_TEA_pkn, "portData"),
                OP_PACKETNET_CONNECT(ctrl_0_0_TEA_pkn, "portControl")
            )
        ),
        0
    );

    // PSE repository

    const char *repository_path = "peripheral/repository/pse.pse";
    opPeripheralNew(
        mi,
        repository_path,
        "repository",
        OP_CONNECTIONS(
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_REPOSITORY_pkn, "portData"),
                OP_PACKETNET_CONNECT(ctrl_0_0_REPOSITORY_pkn, "portControl")
            )
        ),
        0
    );

    // PSE uart0

    const char *uart0_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart0_path,
        "uart0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic0

    const char *plic0_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic0_path,
        "plic0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip0_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX0_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX0_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR0_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci0

    const char *prci0_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci0_path,
        "prci0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt0_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt0_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router0

    const char *router0_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router0_path,
        "router0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu0Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu0Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_TEA_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_0_0_TEA_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_0_0_REPOSITORY_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_0_0_REPOSITORY_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_0_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_0_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_0_0_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_0_0_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_0_0_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_0_0_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_0_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni0

    const char *ni0_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni0_path,
        "ni0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu0Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu0Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX0_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX0_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR0_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_0_0_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer0

    const char *printer0_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer0_path,
        "printer0",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart1

    const char *uart1_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart1_path,
        "uart1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic1

    const char *plic1_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic1_path,
        "plic1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip1_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX1_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX1_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR1_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci1

    const char *prci1_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci1_path,
        "prci1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt1_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt1_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router1

    const char *router1_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router1_path,
        "router1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu1Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu1Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_0_0_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_0_1_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_0_1_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_0_2_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_0_2_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_1_1_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_1_1_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_1_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni1

    const char *ni1_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni1_path,
        "ni1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu1Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu1Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX1_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX1_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR1_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_0_1_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer1

    const char *printer1_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer1_path,
        "printer1",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart2

    const char *uart2_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart2_path,
        "uart2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic2

    const char *plic2_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic2_path,
        "plic2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip2_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX2_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX2_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR2_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci2

    const char *prci2_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci2_path,
        "prci2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt2_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt2_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router2

    const char *router2_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router2_path,
        "router2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu2Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu2Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_0_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_0_2_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_0_2_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_0_2_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_0_2_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_0_2_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_0_2_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_2_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni2

    const char *ni2_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni2_path,
        "ni2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu2Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu2Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX2_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX2_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR2_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_0_2_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer2

    const char *printer2_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer2_path,
        "printer2",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart3

    const char *uart3_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart3_path,
        "uart3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic3

    const char *plic3_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic3_path,
        "plic3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip3_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX3_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX3_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR3_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci3

    const char *prci3_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci3_path,
        "prci3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt3_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt3_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router3

    const char *router3_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router3_path,
        "router3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu3Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu3Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_2_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_0_2_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_0_3_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_0_3_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_3_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_1_3_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_3_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni3

    const char *ni3_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni3_path,
        "ni3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu3Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu3Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX3_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX3_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR3_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_3_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_0_3_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer3

    const char *printer3_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer3_path,
        "printer3",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart4

    const char *uart4_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart4_path,
        "uart4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic4

    const char *plic4_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic4_path,
        "plic4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip4_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX4_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX4_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR4_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci4

    const char *prci4_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci4_path,
        "prci4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt4_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt4_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router4

    const char *router4_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router4_path,
        "router4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu4Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu4Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_0_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_0_0_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_1_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_1_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_1_1_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_0_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_4_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni4

    const char *ni4_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni4_path,
        "ni4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu4Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu4Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX4_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX4_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR4_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_0_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer4

    const char *printer4_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer4_path,
        "printer4",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart5

    const char *uart5_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart5_path,
        "uart5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic5

    const char *plic5_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic5_path,
        "plic5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip5_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX5_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX5_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR5_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci5

    const char *prci5_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci5_path,
        "prci5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt5_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt5_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router5

    const char *router5_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router5_path,
        "router5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu5Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu5Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_1_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_1_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_1_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_1_1_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_1_1_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_1_1_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_1_1_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_1_1_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(data_1_1_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_1_1_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_5_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni5

    const char *ni5_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni5_path,
        "ni5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu5Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu5Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX5_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX5_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR5_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_1_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer5

    const char *printer5_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer5_path,
        "printer5",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart6

    const char *uart6_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart6_path,
        "uart6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic6

    const char *plic6_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic6_path,
        "plic6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip6_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX6_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX6_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR6_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci6

    const char *prci6_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci6_path,
        "prci6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt6_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt6_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router6

    const char *router6_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router6_path,
        "router6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu6Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu6Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_0_2_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_0_2_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_1_1_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_1_1_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_1_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_3_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_1_3_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_2_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_6_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni6

    const char *ni6_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni6_path,
        "ni6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu6Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu6Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX6_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX6_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR6_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_2_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer6

    const char *printer6_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer6_path,
        "printer6",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart7

    const char *uart7_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart7_path,
        "uart7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic7

    const char *plic7_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic7_path,
        "plic7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip7_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX7_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX7_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR7_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci7

    const char *prci7_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci7_path,
        "prci7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt7_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt7_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router7

    const char *router7_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router7_path,
        "router7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu7Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu7Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_3_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_3_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_3_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_1_3_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_1_3_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_1_3_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(data_1_3_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_1_3_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_7_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni7

    const char *ni7_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni7_path,
        "ni7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu7Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu7Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX7_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX7_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR7_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_3_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_3_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer7

    const char *printer7_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer7_path,
        "printer7",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart8

    const char *uart8_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart8_path,
        "uart8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic8

    const char *plic8_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic8_path,
        "plic8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip8_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX8_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX8_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR8_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci8

    const char *prci8_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci8_path,
        "prci8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt8_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt8_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router8

    const char *router8_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router8_path,
        "router8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu8Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu8Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_0_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_2_0_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_0_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(data_2_0_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_8_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni8

    const char *ni8_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni8_path,
        "ni8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu8Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu8Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX8_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX8_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR8_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_0_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer8

    const char *printer8_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer8_path,
        "printer8",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart9

    const char *uart9_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart9_path,
        "uart9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic9

    const char *plic9_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic9_path,
        "plic9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip9_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX9_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX9_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR9_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci9

    const char *prci9_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci9_path,
        "prci9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt9_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt9_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router9

    const char *router9_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router9_path,
        "router9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu9Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu9Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_1_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_1_1_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_2_0_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_2_0_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_2_1_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_1_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_2_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_2_2_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_3_1_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_3_1_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_9_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni9

    const char *ni9_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni9_path,
        "ni9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu9Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu9Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX9_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX9_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR9_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_1_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer9

    const char *printer9_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer9_path,
        "printer9",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu9Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart10

    const char *uart10_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart10_path,
        "uart10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic10

    const char *plic10_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic10_path,
        "plic10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip10_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX10_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX10_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR10_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci10

    const char *prci10_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci10_path,
        "prci10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt10_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt10_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router10

    const char *router10_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router10_path,
        "router10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu10Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu10Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_2_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_2_2_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_2_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_2_2_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_2_2_N_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_N_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(data_2_2_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_10_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni10

    const char *ni10_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni10_path,
        "ni10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu10Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu10Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX10_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX10_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR10_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_2_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer10

    const char *printer10_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer10_path,
        "printer10",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu10Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart11

    const char *uart11_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart11_path,
        "uart11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic11

    const char *plic11_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic11_path,
        "plic11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip11_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX11_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX11_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR11_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci11

    const char *prci11_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci11_path,
        "prci11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt11_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt11_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router11

    const char *router11_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router11_path,
        "router11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu11Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu11Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_3_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_1_3_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_2_2_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_2_2_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_2_3_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_3_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_3_3_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_3_3_S_pkn, "portControlNorth"),
                OP_PACKETNET_CONNECT(iteration_11_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni11

    const char *ni11_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni11_path,
        "ni11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu11Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu11Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX11_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX11_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR11_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_3_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_3_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer11

    const char *printer11_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer11_path,
        "printer11",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu11Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart12

    const char *uart12_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart12_path,
        "uart12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic12

    const char *plic12_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic12_path,
        "plic12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip12_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX12_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX12_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR12_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci12

    const char *prci12_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci12_path,
        "prci12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt12_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt12_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router12

    const char *router12_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router12_path,
        "router12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu12Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu12Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_0_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_3_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_3_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_3_1_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_3_1_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(iteration_12_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni12

    const char *ni12_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni12_path,
        "ni12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu12Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu12Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX12_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX12_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR12_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_3_0_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer12

    const char *printer12_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer12_path,
        "printer12",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu12Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart13

    const char *uart13_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart13_path,
        "uart13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic13

    const char *plic13_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic13_path,
        "plic13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip13_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX13_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX13_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR13_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci13

    const char *prci13_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci13_path,
        "prci13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt13_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt13_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router13

    const char *router13_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router13_path,
        "router13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu13Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu13Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_1_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_3_1_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_3_1_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_3_1_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_3_1_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_3_1_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_3_1_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_3_1_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_13_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni13

    const char *ni13_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni13_path,
        "ni13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu13Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu13Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX13_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX13_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR13_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_3_1_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer13

    const char *printer13_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer13_path,
        "printer13",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu13Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart14

    const char *uart14_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart14_path,
        "uart14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic14

    const char *plic14_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic14_path,
        "plic14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip14_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX14_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX14_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR14_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci14

    const char *prci14_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci14_path,
        "prci14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt14_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt14_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router14

    const char *router14_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router14_path,
        "router14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu14Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu14Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_2_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_3_1_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_3_1_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_3_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_3_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_3_3_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_3_3_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(iteration_14_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni14

    const char *ni14_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni14_path,
        "ni14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu14Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu14Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX14_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX14_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR14_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_3_2_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer14

    const char *printer14_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer14_path,
        "printer14",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu14Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE uart15

    const char *uart15_path = opVLNVString(
        0, // use the default VLNV path
        "microsemi.ovpworld.org",
        "peripheral",
        "CoreUARTapb",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

    opPeripheralNew(
        mi,
        uart15_path,
        "uart15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "port0", .slave=1, .addrLo=0x70001000ULL, .addrHi=0x70001017ULL)
            )
        ),
        0
    );

    // PSE plic15

    const char *plic15_path = "peripheral/plic/pse.pse";
    opPeripheralNew(
        mi,
        plic15_path,
        "plic15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "port0", .slave=1, .addrLo=0x40000000ULL, .addrHi=0x43ffffffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(eip15_n, "irqT0"),
                OP_NET_CONNECT(intNI_RX15_n, "irqS2"),
                OP_NET_CONNECT(intNI_TX15_n, "irqS3"),
                OP_NET_CONNECT(intNI_TMR15_n, "irqS4")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci15

    const char *prci15_path = "peripheral/CLINT/pse.pse";
    opPeripheralNew(
        mi,
        prci15_path,
        "prci15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "port0", .slave=1, .addrLo=0x44000000ULL, .addrHi=0x4400bfffULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(MTimerInterrupt15_n, "MTimerInterrupt0"),
                OP_NET_CONNECT(MSWInterrupt15_n, "MSWInterrupt0")
            )
        ),
        OP_PARAMS(
             OP_PARAM_DOUBLE_SET("clockMHz", 1000.0)
        )
    );

    // PSE router15

    const char *router15_path = "peripheral/whnoc_dma/pse.pse";
    opPeripheralNew(
        mi,
        router15_path,
        "router15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "RREAD"),
                OP_BUS_CONNECT(cpu15Bus_b, "RWRITE"),
                OP_BUS_CONNECT(cpu15Bus_b, "localPort", .slave=1, .addrLo=0x50000000ULL, .addrHi=0x50000003ULL)
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_3_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_3_3_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_3_3_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_3_3_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_3_3_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_3_3_S_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(iteration_15_pkn, "iterationsPort")
            )
        ),
        0
    );

    // PSE ni15

    const char *ni15_path = "peripheral/networkInterface/pse.pse";
    opPeripheralNew(
        mi,
        ni15_path,
        "ni15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "MREAD"),
                OP_BUS_CONNECT(cpu15Bus_b, "MWRITE"),
                OP_BUS_CONNECT(cpu15Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x50000013ULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_RX15_n, "INT_NI_RX"),
                OP_NET_CONNECT(intNI_TX15_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_TMR15_n, "INT_NI_TMR")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_3_3_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_3_3_L_pkn, "controlPort")
            )
        ),
        0
    );

    // PSE printer15

    const char *printer15_path = "peripheral/printer/pse.pse";
    opPeripheralNew(
        mi,
        printer15_path,
        "printer15",
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu15Bus_b, "PRINTREGS", .slave=1, .addrLo=0x50000020ULL, .addrHi=0x50000027ULL)
            )
        ),
        0
    );

    // PSE iterator

    const char *iterator_path = "peripheral/iterator/pse.pse";
    opPeripheralNew(
        mi,
        iterator_path,
        "iterator",
        OP_CONNECTIONS(
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(iteration_0_pkn, "iterationPort0"),
                OP_PACKETNET_CONNECT(iteration_1_pkn, "iterationPort1"),
                OP_PACKETNET_CONNECT(iteration_2_pkn, "iterationPort2"),
                OP_PACKETNET_CONNECT(iteration_3_pkn, "iterationPort3"),
                OP_PACKETNET_CONNECT(iteration_4_pkn, "iterationPort4"),
                OP_PACKETNET_CONNECT(iteration_5_pkn, "iterationPort5"),
                OP_PACKETNET_CONNECT(iteration_6_pkn, "iterationPort6"),
                OP_PACKETNET_CONNECT(iteration_7_pkn, "iterationPort7"),
                OP_PACKETNET_CONNECT(iteration_8_pkn, "iterationPort8"),
                OP_PACKETNET_CONNECT(iteration_9_pkn, "iterationPort9"),
                OP_PACKETNET_CONNECT(iteration_10_pkn, "iterationPort10"),
                OP_PACKETNET_CONNECT(iteration_11_pkn, "iterationPort11"),
                OP_PACKETNET_CONNECT(iteration_12_pkn, "iterationPort12"),
                OP_PACKETNET_CONNECT(iteration_13_pkn, "iterationPort13"),
                OP_PACKETNET_CONNECT(iteration_14_pkn, "iterationPort14"),
                OP_PACKETNET_CONNECT(iteration_15_pkn, "iterationPort15")
            )
        ),
        0
    );

}

optModuleAttr modelAttrs = {
    .versionString        = OP_VERSION,
    .type                 = OP_MODULE,
    .name                 = MODULE_NAME,
    .objectSize           = sizeof(optModuleObject),
    .releaseStatus        = OP_UNSET,
    .purpose              = OP_PP_BAREMETAL,
    .visibility           = OP_VISIBLE,
    .vlnv          = {
        .vendor  = "defaultVendor",
        .library = "work",
        .name    = "Chronos_RiscV_FreeRTOS",
        .version = "1.0"
    },
    .constructCB          = moduleConstructor,
    .preSimulateCB        = modulePreSimulate,
    .simulateCB           = moduleSimulateStart,
    .postSimulateCB       = modulePostSimulate,
    .destructCB           = moduleDestruct,
};
