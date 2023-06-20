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

#ifndef NETWORKINTERFACE_MACROS_IGEN_H
#define NETWORKINTERFACE_MACROS_IGEN_H  

// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.

// Set the macro 'DMAC' to the base of port 'DMAC'
#ifndef DMAC
#error DMAC is undefined. It needs to be set to the port base address
#endif


#define DMAC_AB8_POFFSET                0x0
#define DMAC_AB8_ADDRESS_POFFSET        0x0
#define DMAC_AB8_ADDRESS                (DMAC + DMAC_AB8_POFFSET + DMAC_AB8_ADDRESS_POFFSET)

#define DMAC_AB8_STATUSTX_POFFSET       0x4
#define DMAC_AB8_STATUSTX               (DMAC + DMAC_AB8_POFFSET + DMAC_AB8_STATUSTX_POFFSET)

#define DMAC_AB8_STATUSRX_POFFSET       0x8
#define DMAC_AB8_STATUSRX               (DMAC + DMAC_AB8_POFFSET + DMAC_AB8_STATUSRX_POFFSET)

#define DMAC_AB8_TIMER_POFFSET          0xc
#define DMAC_AB8_TIMER                  (DMAC + DMAC_AB8_POFFSET + DMAC_AB8_TIMER_POFFSET)


#endif
