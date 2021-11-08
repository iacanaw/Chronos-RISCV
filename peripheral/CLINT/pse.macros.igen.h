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

#ifndef PSE_MACROS_IGEN_H
#define PSE_MACROS_IGEN_H               

// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.

// Set the macro 'PORT0' to the base of port 'port0'
#ifndef PORT0
#error PORT0 is undefined. It needs to be set to the port base address
#endif


#define PORT0_MSIP_POFFSET              0x0
#define PORT0_MSIP_MSIP0_POFFSET        0x0
#define PORT0_MSIP_MSIP0                (PORT0 + PORT0_MSIP_POFFSET + PORT0_MSIP_MSIP0_POFFSET)

#define PORT0_MTIME_POFFSET             0x4000
#define PORT0_MTIME_MTIMECMPLO0_POFFSET  0x0
#define PORT0_MTIME_MTIMECMPLO0         (PORT0 + PORT0_MTIME_POFFSET + PORT0_MTIME_MTIMECMPLO0_POFFSET)

#define PORT0_MTIME_MTIMECMPHI0_POFFSET  0x4
#define PORT0_MTIME_MTIMECMPHI0         (PORT0 + PORT0_MTIME_POFFSET + PORT0_MTIME_MTIMECMPHI0_POFFSET)

#define PORT0_MTIME_MTIMELO_POFFSET     0x7ff8
#define PORT0_MTIME_MTIMELO             (PORT0 + PORT0_MTIME_POFFSET + PORT0_MTIME_MTIMELO_POFFSET)

#define PORT0_MTIME_MTIMEHI_POFFSET     0x7ffc
#define PORT0_MTIME_MTIMEHI             (PORT0 + PORT0_MTIME_POFFSET + PORT0_MTIME_MTIMEHI_POFFSET)

#define PORT0_MSIP_MSIPMEM_POFFSET      0x0
#define PORT0_MSIP_MSIPMEM              (PORT0 + PORT0_MSIP_POFFSET + PORT0_MSIP_MSIPMEM_POFFSET)
#define PORT0_MSIP_MSIPMEM_BYTES        0x4000

#define PORT0_MTIME_MTIMEMEM_POFFSET    0x0
#define PORT0_MTIME_MTIMEMEM            (PORT0 + PORT0_MTIME_POFFSET + PORT0_MTIME_MTIMEMEM_POFFSET)
#define PORT0_MTIME_MTIMEMEM_BYTES      0x8000


#endif
