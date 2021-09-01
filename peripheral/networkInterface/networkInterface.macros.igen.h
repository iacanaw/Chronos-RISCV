
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


#endif
