
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PRINTER_MACROS_IGEN_H
#define PRINTER_MACROS_IGEN_H           

// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.

// Set the macro 'PRINTREGS' to the base of port 'PRINTREGS'
#ifndef PRINTREGS
#error PRINTREGS is undefined. It needs to be set to the port base address
#endif


#define PRINTREGS_AB8_POFFSET           0x0
#define PRINTREGS_AB8_PRINTVALUE_CHAR_POFFSET  0x0
#define PRINTREGS_AB8_PRINTVALUE_CHAR   (PRINTREGS + PRINTREGS_AB8_POFFSET + PRINTREGS_AB8_PRINTVALUE_CHAR_POFFSET)

#define PRINTREGS_AB8_PRINTVALUE_INT_POFFSET  0x4
#define PRINTREGS_AB8_PRINTVALUE_INT    (PRINTREGS + PRINTREGS_AB8_POFFSET + PRINTREGS_AB8_PRINTVALUE_INT_POFFSET)


#endif
