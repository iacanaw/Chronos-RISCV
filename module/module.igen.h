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
    optNetP MTimerInterrupt0_n = opNetNew(mi, "MTimerInterrupt0", 0, 0);
    optNetP MSWInterrupt0_n = opNetNew(mi, "MSWInterrupt0", 0, 0);
    optNetP intNI_TX0_n = opNetNew(mi, "intNI_TX0", 0, 0);
    optNetP intNI_RX0_n = opNetNew(mi, "intNI_RX0", 0, 0);
    optNetP MTimerInterrupt1_n = opNetNew(mi, "MTimerInterrupt1", 0, 0);
    optNetP MSWInterrupt1_n = opNetNew(mi, "MSWInterrupt1", 0, 0);
    optNetP intNI_TX1_n = opNetNew(mi, "intNI_TX1", 0, 0);
    optNetP intNI_RX1_n = opNetNew(mi, "intNI_RX1", 0, 0);
    optNetP MTimerInterrupt2_n = opNetNew(mi, "MTimerInterrupt2", 0, 0);
    optNetP MSWInterrupt2_n = opNetNew(mi, "MSWInterrupt2", 0, 0);
    optNetP intNI_TX2_n = opNetNew(mi, "intNI_TX2", 0, 0);
    optNetP intNI_RX2_n = opNetNew(mi, "intNI_RX2", 0, 0);
    optNetP MTimerInterrupt3_n = opNetNew(mi, "MTimerInterrupt3", 0, 0);
    optNetP MSWInterrupt3_n = opNetNew(mi, "MSWInterrupt3", 0, 0);
    optNetP intNI_TX3_n = opNetNew(mi, "intNI_TX3", 0, 0);
    optNetP intNI_RX3_n = opNetNew(mi, "intNI_RX3", 0, 0);
    optNetP MTimerInterrupt4_n = opNetNew(mi, "MTimerInterrupt4", 0, 0);
    optNetP MSWInterrupt4_n = opNetNew(mi, "MSWInterrupt4", 0, 0);
    optNetP intNI_TX4_n = opNetNew(mi, "intNI_TX4", 0, 0);
    optNetP intNI_RX4_n = opNetNew(mi, "intNI_RX4", 0, 0);
    optNetP MTimerInterrupt5_n = opNetNew(mi, "MTimerInterrupt5", 0, 0);
    optNetP MSWInterrupt5_n = opNetNew(mi, "MSWInterrupt5", 0, 0);
    optNetP intNI_TX5_n = opNetNew(mi, "intNI_TX5", 0, 0);
    optNetP intNI_RX5_n = opNetNew(mi, "intNI_RX5", 0, 0);
    optNetP MTimerInterrupt6_n = opNetNew(mi, "MTimerInterrupt6", 0, 0);
    optNetP MSWInterrupt6_n = opNetNew(mi, "MSWInterrupt6", 0, 0);
    optNetP intNI_TX6_n = opNetNew(mi, "intNI_TX6", 0, 0);
    optNetP intNI_RX6_n = opNetNew(mi, "intNI_RX6", 0, 0);
    optNetP MTimerInterrupt7_n = opNetNew(mi, "MTimerInterrupt7", 0, 0);
    optNetP MSWInterrupt7_n = opNetNew(mi, "MSWInterrupt7", 0, 0);
    optNetP intNI_TX7_n = opNetNew(mi, "intNI_TX7", 0, 0);
    optNetP intNI_RX7_n = opNetNew(mi, "intNI_RX7", 0, 0);
    optNetP MTimerInterrupt8_n = opNetNew(mi, "MTimerInterrupt8", 0, 0);
    optNetP MSWInterrupt8_n = opNetNew(mi, "MSWInterrupt8", 0, 0);
    optNetP intNI_TX8_n = opNetNew(mi, "intNI_TX8", 0, 0);
    optNetP intNI_RX8_n = opNetNew(mi, "intNI_RX8", 0, 0);

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

    opPacketnetNew(mi, "data_0_2_E", 0, 0);

    optPacketnetP data_0_2_W_pkn = opPacketnetNew(mi, "data_0_2_W", 0, 0);

    optPacketnetP data_0_2_N_pkn = opPacketnetNew(mi, "data_0_2_N", 0, 0);

    opPacketnetNew(mi, "data_0_2_S", 0, 0);

    opPacketnetNew(mi, "ctrl_0_2_E", 0, 0);

    optPacketnetP ctrl_0_2_W_pkn = opPacketnetNew(mi, "ctrl_0_2_W", 0, 0);

    optPacketnetP ctrl_0_2_N_pkn = opPacketnetNew(mi, "ctrl_0_2_N", 0, 0);

    opPacketnetNew(mi, "ctrl_0_2_S", 0, 0);

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

    optPacketnetP data_2_0_L_pkn = opPacketnetNew(mi, "data_2_0_L", 0, 0);

    optPacketnetP ctrl_2_0_L_pkn = opPacketnetNew(mi, "ctrl_2_0_L", 0, 0);

    optPacketnetP data_2_0_E_pkn = opPacketnetNew(mi, "data_2_0_E", 0, 0);

    opPacketnetNew(mi, "data_2_0_W", 0, 0);

    opPacketnetNew(mi, "data_2_0_N", 0, 0);

    optPacketnetP data_2_0_S_pkn = opPacketnetNew(mi, "data_2_0_S", 0, 0);

    optPacketnetP ctrl_2_0_E_pkn = opPacketnetNew(mi, "ctrl_2_0_E", 0, 0);

    opPacketnetNew(mi, "ctrl_2_0_W", 0, 0);

    opPacketnetNew(mi, "ctrl_2_0_N", 0, 0);

    optPacketnetP ctrl_2_0_S_pkn = opPacketnetNew(mi, "ctrl_2_0_S", 0, 0);

    optPacketnetP data_2_1_L_pkn = opPacketnetNew(mi, "data_2_1_L", 0, 0);

    optPacketnetP ctrl_2_1_L_pkn = opPacketnetNew(mi, "ctrl_2_1_L", 0, 0);

    optPacketnetP data_2_2_L_pkn = opPacketnetNew(mi, "data_2_2_L", 0, 0);

    optPacketnetP ctrl_2_2_L_pkn = opPacketnetNew(mi, "ctrl_2_2_L", 0, 0);

    opPacketnetNew(mi, "data_2_2_E", 0, 0);

    optPacketnetP data_2_2_W_pkn = opPacketnetNew(mi, "data_2_2_W", 0, 0);

    opPacketnetNew(mi, "data_2_2_N", 0, 0);

    optPacketnetP data_2_2_S_pkn = opPacketnetNew(mi, "data_2_2_S", 0, 0);

    opPacketnetNew(mi, "ctrl_2_2_E", 0, 0);

    optPacketnetP ctrl_2_2_W_pkn = opPacketnetNew(mi, "ctrl_2_2_W", 0, 0);

    opPacketnetNew(mi, "ctrl_2_2_N", 0, 0);

    optPacketnetP ctrl_2_2_S_pkn = opPacketnetNew(mi, "ctrl_2_2_S", 0, 0);

    optPacketnetP iteration_0_pkn = opPacketnetNew(mi, "iteration_0", 0, 0);

    optPacketnetP iteration_1_pkn = opPacketnetNew(mi, "iteration_1", 0, 0);

    optPacketnetP iteration_2_pkn = opPacketnetNew(mi, "iteration_2", 0, 0);

    optPacketnetP iteration_3_pkn = opPacketnetNew(mi, "iteration_3", 0, 0);

    optPacketnetP iteration_4_pkn = opPacketnetNew(mi, "iteration_4", 0, 0);

    optPacketnetP iteration_5_pkn = opPacketnetNew(mi, "iteration_5", 0, 0);

    optPacketnetP iteration_6_pkn = opPacketnetNew(mi, "iteration_6", 0, 0);

    optPacketnetP iteration_7_pkn = opPacketnetNew(mi, "iteration_7", 0, 0);

    optPacketnetP iteration_8_pkn = opPacketnetNew(mi, "iteration_8", 0, 0);

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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
            ,OP_PARAM_DOUBLE_SET("mips", 50)
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
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu0Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram1

    opMemoryNew(
        mi,
        "nvram1",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu1Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram2

    opMemoryNew(
        mi,
        "nvram2",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu2Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram3

    opMemoryNew(
        mi,
        "nvram3",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu3Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram4

    opMemoryNew(
        mi,
        "nvram4",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu4Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram5

    opMemoryNew(
        mi,
        "nvram5",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu5Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram6

    opMemoryNew(
        mi,
        "nvram6",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu6Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram7

    opMemoryNew(
        mi,
        "nvram7",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu7Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
            )
        ),
        0
    );

    // Memory nvram8

    opMemoryNew(
        mi,
        "nvram8",
        OP_PRIV_RWX,
        (0x6003ffffULL) - (0x60000000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "sp1", .slave=1, .addrLo=0x60000000ULL, .addrHi=0x6003ffffULL)
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
        (0xffffffffULL) - (0xfff00000ULL),
        OP_CONNECTIONS(
            OP_BUS_CONNECTIONS(
                OP_BUS_CONNECT(cpu8Bus_b, "sp1", .slave=1, .addrLo=0xfff00000ULL, .addrHi=0xffffffffULL)
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
                OP_NET_CONNECT(intNI_TX0_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX0_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci0

    const char *prci0_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_BUS_CONNECT(cpu0Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX0_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX0_n, "INT_NI_RX")
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
                OP_NET_CONNECT(intNI_TX1_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX1_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci1

    const char *prci1_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_BUS_CONNECT(cpu1Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX1_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX1_n, "INT_NI_RX")
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
                OP_NET_CONNECT(intNI_TX2_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX2_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci2

    const char *prci2_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_BUS_CONNECT(cpu2Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX2_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX2_n, "INT_NI_RX")
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
                OP_NET_CONNECT(intNI_TX3_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX3_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci3

    const char *prci3_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_PACKETNET_CONNECT(data_0_0_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_0_0_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_1_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_1_1_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_1_1_W_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_0_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_S_pkn, "portControlNorth"),
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
                OP_BUS_CONNECT(cpu3Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX3_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX3_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_0_L_pkn, "controlPort")
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
                OP_NET_CONNECT(intNI_TX4_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX4_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci4

    const char *prci4_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_BUS_CONNECT(cpu4Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX4_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX4_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_1_L_pkn, "controlPort")
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
                OP_NET_CONNECT(intNI_TX5_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX5_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci5

    const char *prci5_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_PACKETNET_CONNECT(data_0_2_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_0_2_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_1_1_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_1_1_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_1_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_1_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_2_S_pkn, "portDataNorth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_S_pkn, "portControlNorth"),
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
                OP_BUS_CONNECT(cpu5Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX5_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX5_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_1_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_1_2_L_pkn, "controlPort")
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
                OP_NET_CONNECT(intNI_TX6_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX6_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci6

    const char *prci6_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_PACKETNET_CONNECT(data_2_0_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_0_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_0_E_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_2_0_E_pkn, "portControlEast"),
                OP_PACKETNET_CONNECT(data_2_0_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_0_S_pkn, "portControlSouth"),
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
                OP_BUS_CONNECT(cpu6Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX6_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX6_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_0_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_0_L_pkn, "controlPort")
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
                OP_NET_CONNECT(intNI_TX7_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX7_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci7

    const char *prci7_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_PACKETNET_CONNECT(data_1_1_N_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_1_1_N_pkn, "portControlSouth"),
                OP_PACKETNET_CONNECT(data_2_0_E_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_2_0_E_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_2_1_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_1_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_2_W_pkn, "portDataEast"),
                OP_PACKETNET_CONNECT(ctrl_2_2_W_pkn, "portControlEast"),
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
                OP_BUS_CONNECT(cpu7Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX7_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX7_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_1_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_1_L_pkn, "controlPort")
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
                OP_NET_CONNECT(intNI_TX8_n, "irqS1"),
                OP_NET_CONNECT(intNI_RX8_n, "irqS2")
            )
        ),
        OP_PARAMS(
             OP_PARAM_UNS32_SET("num_sources", 256)
            ,OP_PARAM_UNS32_SET("num_targets", 1)
        )
    );

    // PSE prci8

    const char *prci8_path = opVLNVString(
        0, // use the default VLNV path
        "riscv.ovpworld.org",
        "peripheral",
        "CLINT",
        "1.0",
        OP_PERIPHERAL,
        1   // report errors
    );

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
                OP_PACKETNET_CONNECT(data_2_2_L_pkn, "portDataLocal"),
                OP_PACKETNET_CONNECT(ctrl_2_2_L_pkn, "portControlLocal"),
                OP_PACKETNET_CONNECT(data_2_2_W_pkn, "portDataWest"),
                OP_PACKETNET_CONNECT(ctrl_2_2_W_pkn, "portControlWest"),
                OP_PACKETNET_CONNECT(data_2_2_S_pkn, "portDataSouth"),
                OP_PACKETNET_CONNECT(ctrl_2_2_S_pkn, "portControlSouth"),
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
                OP_BUS_CONNECT(cpu8Bus_b, "DMAC", .slave=1, .addrLo=0x50000004ULL, .addrHi=0x5000000fULL)
            ),
            OP_NET_CONNECTIONS(
                OP_NET_CONNECT(intNI_TX8_n, "INT_NI_TX"),
                OP_NET_CONNECT(intNI_RX8_n, "INT_NI_RX")
            ),
            OP_PACKETNET_CONNECTIONS(
                OP_PACKETNET_CONNECT(data_2_2_L_pkn, "dataPort"),
                OP_PACKETNET_CONNECT(ctrl_2_2_L_pkn, "controlPort")
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
                OP_PACKETNET_CONNECT(iteration_8_pkn, "iterationPort8")
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
