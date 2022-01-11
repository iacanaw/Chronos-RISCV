ihwnew -name Chronos_RiscV_FreeRTOS

############################ bus ##################################

ihwaddbus -instancename cpu0Bus -addresswidth 32
ihwaddbus -instancename cpu1Bus -addresswidth 32
ihwaddbus -instancename cpu2Bus -addresswidth 32
ihwaddbus -instancename cpu3Bus -addresswidth 32
ihwaddbus -instancename cpu4Bus -addresswidth 32
ihwaddbus -instancename cpu5Bus -addresswidth 32
ihwaddbus -instancename cpu6Bus -addresswidth 32
ihwaddbus -instancename cpu7Bus -addresswidth 32
ihwaddbus -instancename cpu8Bus -addresswidth 32
ihwaddbus -instancename cpu9Bus -addresswidth 32
ihwaddbus -instancename cpu10Bus -addresswidth 32
ihwaddbus -instancename cpu11Bus -addresswidth 32
ihwaddbus -instancename cpu12Bus -addresswidth 32
ihwaddbus -instancename cpu13Bus -addresswidth 32
ihwaddbus -instancename cpu14Bus -addresswidth 32
ihwaddbus -instancename cpu15Bus -addresswidth 32

############################ processor ############################

ihwaddprocessor -instancename cpu0 -id 0  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu0 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu0 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu0 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu0  -busmasterport INSTRUCTION -bus cpu0Bus
ihwconnect -instancename cpu0  -busmasterport DATA        -bus cpu0Bus

ihwaddprocessor -instancename cpu1 -id 1  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu1 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu1 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu1 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu1  -busmasterport INSTRUCTION -bus cpu1Bus
ihwconnect -instancename cpu1  -busmasterport DATA        -bus cpu1Bus

ihwaddprocessor -instancename cpu2 -id 2  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu2 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu2 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu2 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu2  -busmasterport INSTRUCTION -bus cpu2Bus
ihwconnect -instancename cpu2  -busmasterport DATA        -bus cpu2Bus

ihwaddprocessor -instancename cpu3 -id 3  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu3 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu3 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu3 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu3  -busmasterport INSTRUCTION -bus cpu3Bus
ihwconnect -instancename cpu3  -busmasterport DATA        -bus cpu3Bus

ihwaddprocessor -instancename cpu4 -id 4  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu4 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu4 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu4 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu4  -busmasterport INSTRUCTION -bus cpu4Bus
ihwconnect -instancename cpu4  -busmasterport DATA        -bus cpu4Bus

ihwaddprocessor -instancename cpu5 -id 5  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu5 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu5 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu5 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu5  -busmasterport INSTRUCTION -bus cpu5Bus
ihwconnect -instancename cpu5  -busmasterport DATA        -bus cpu5Bus

ihwaddprocessor -instancename cpu6 -id 6  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu6 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu6 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu6 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu6  -busmasterport INSTRUCTION -bus cpu6Bus
ihwconnect -instancename cpu6  -busmasterport DATA        -bus cpu6Bus

ihwaddprocessor -instancename cpu7 -id 7  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu7 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu7 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu7 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu7  -busmasterport INSTRUCTION -bus cpu7Bus
ihwconnect -instancename cpu7  -busmasterport DATA        -bus cpu7Bus

ihwaddprocessor -instancename cpu8 -id 8  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu8 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu8 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu8 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu8  -busmasterport INSTRUCTION -bus cpu8Bus
ihwconnect -instancename cpu8  -busmasterport DATA        -bus cpu8Bus

ihwaddprocessor -instancename cpu9 -id 9  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu9 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu9 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu9 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu9  -busmasterport INSTRUCTION -bus cpu9Bus
ihwconnect -instancename cpu9  -busmasterport DATA        -bus cpu9Bus

ihwaddprocessor -instancename cpu10 -id 10  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu10 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu10 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu10 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu10  -busmasterport INSTRUCTION -bus cpu10Bus
ihwconnect -instancename cpu10  -busmasterport DATA        -bus cpu10Bus

ihwaddprocessor -instancename cpu11 -id 11  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu11 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu11 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu11 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu11  -busmasterport INSTRUCTION -bus cpu11Bus
ihwconnect -instancename cpu11  -busmasterport DATA        -bus cpu11Bus

ihwaddprocessor -instancename cpu12 -id 12  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu12 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu12 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu12 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu12  -busmasterport INSTRUCTION -bus cpu12Bus
ihwconnect -instancename cpu12  -busmasterport DATA        -bus cpu12Bus

ihwaddprocessor -instancename cpu13 -id 13  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu13 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu13 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu13 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu13  -busmasterport INSTRUCTION -bus cpu13Bus
ihwconnect -instancename cpu13  -busmasterport DATA        -bus cpu13Bus

ihwaddprocessor -instancename cpu14 -id 14  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu14 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu14 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu14 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu14  -busmasterport INSTRUCTION -bus cpu14Bus
ihwconnect -instancename cpu14  -busmasterport DATA        -bus cpu14Bus

ihwaddprocessor -instancename cpu15 -id 15  \
	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \
	-mips 1000 -simulateexceptions

ihwsetparameter -handle cpu15 -name variant        -value RVB32I -type enum
ihwsetparameter -handle cpu15 -name add_Extensions -value MSU -type string
ihwsetparameter -handle cpu15 -name external_int_id -value True -type Bool

ihwconnect -instancename cpu15  -busmasterport INSTRUCTION -bus cpu15Bus
ihwconnect -instancename cpu15  -busmasterport DATA        -bus cpu15Bus

############################## memory #############################

ihwaddmemory -instancename nvram0 -type ram
ihwconnect   -instancename nvram0 -busslaveport sp1 -bus cpu0Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr0   -type ram
ihwconnect   -instancename ddr0   -busslaveport sp1 -bus cpu0Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk0   -type ram
ihwconnect   -instancename stk0   -busslaveport sp1 -bus cpu0Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram1 -type ram
ihwconnect   -instancename nvram1 -busslaveport sp1 -bus cpu1Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr1   -type ram
ihwconnect   -instancename ddr1   -busslaveport sp1 -bus cpu1Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk1   -type ram
ihwconnect   -instancename stk1   -busslaveport sp1 -bus cpu1Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram2 -type ram
ihwconnect   -instancename nvram2 -busslaveport sp1 -bus cpu2Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr2   -type ram
ihwconnect   -instancename ddr2   -busslaveport sp1 -bus cpu2Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk2   -type ram
ihwconnect   -instancename stk2   -busslaveport sp1 -bus cpu2Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram3 -type ram
ihwconnect   -instancename nvram3 -busslaveport sp1 -bus cpu3Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr3   -type ram
ihwconnect   -instancename ddr3   -busslaveport sp1 -bus cpu3Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk3   -type ram
ihwconnect   -instancename stk3   -busslaveport sp1 -bus cpu3Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram4 -type ram
ihwconnect   -instancename nvram4 -busslaveport sp1 -bus cpu4Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr4   -type ram
ihwconnect   -instancename ddr4   -busslaveport sp1 -bus cpu4Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk4   -type ram
ihwconnect   -instancename stk4   -busslaveport sp1 -bus cpu4Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram5 -type ram
ihwconnect   -instancename nvram5 -busslaveport sp1 -bus cpu5Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr5   -type ram
ihwconnect   -instancename ddr5   -busslaveport sp1 -bus cpu5Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk5   -type ram
ihwconnect   -instancename stk5   -busslaveport sp1 -bus cpu5Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram6 -type ram
ihwconnect   -instancename nvram6 -busslaveport sp1 -bus cpu6Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr6   -type ram
ihwconnect   -instancename ddr6   -busslaveport sp1 -bus cpu6Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk6   -type ram
ihwconnect   -instancename stk6   -busslaveport sp1 -bus cpu6Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram7 -type ram
ihwconnect   -instancename nvram7 -busslaveport sp1 -bus cpu7Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr7   -type ram
ihwconnect   -instancename ddr7   -busslaveport sp1 -bus cpu7Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk7   -type ram
ihwconnect   -instancename stk7   -busslaveport sp1 -bus cpu7Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram8 -type ram
ihwconnect   -instancename nvram8 -busslaveport sp1 -bus cpu8Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr8   -type ram
ihwconnect   -instancename ddr8   -busslaveport sp1 -bus cpu8Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk8   -type ram
ihwconnect   -instancename stk8   -busslaveport sp1 -bus cpu8Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram9 -type ram
ihwconnect   -instancename nvram9 -busslaveport sp1 -bus cpu9Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr9   -type ram
ihwconnect   -instancename ddr9   -busslaveport sp1 -bus cpu9Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk9   -type ram
ihwconnect   -instancename stk9   -busslaveport sp1 -bus cpu9Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram10 -type ram
ihwconnect   -instancename nvram10 -busslaveport sp1 -bus cpu10Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr10   -type ram
ihwconnect   -instancename ddr10   -busslaveport sp1 -bus cpu10Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk10   -type ram
ihwconnect   -instancename stk10   -busslaveport sp1 -bus cpu10Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram11 -type ram
ihwconnect   -instancename nvram11 -busslaveport sp1 -bus cpu11Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr11   -type ram
ihwconnect   -instancename ddr11   -busslaveport sp1 -bus cpu11Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk11   -type ram
ihwconnect   -instancename stk11   -busslaveport sp1 -bus cpu11Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram12 -type ram
ihwconnect   -instancename nvram12 -busslaveport sp1 -bus cpu12Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr12   -type ram
ihwconnect   -instancename ddr12   -busslaveport sp1 -bus cpu12Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk12   -type ram
ihwconnect   -instancename stk12   -busslaveport sp1 -bus cpu12Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram13 -type ram
ihwconnect   -instancename nvram13 -busslaveport sp1 -bus cpu13Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr13   -type ram
ihwconnect   -instancename ddr13   -busslaveport sp1 -bus cpu13Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk13   -type ram
ihwconnect   -instancename stk13   -busslaveport sp1 -bus cpu13Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram14 -type ram
ihwconnect   -instancename nvram14 -busslaveport sp1 -bus cpu14Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr14   -type ram
ihwconnect   -instancename ddr14   -busslaveport sp1 -bus cpu14Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk14   -type ram
ihwconnect   -instancename stk14   -busslaveport sp1 -bus cpu14Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF


ihwaddmemory -instancename nvram15 -type ram
ihwconnect   -instancename nvram15 -busslaveport sp1 -bus cpu15Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF

ihwaddmemory -instancename ddr15   -type ram
ihwconnect   -instancename ddr15   -busslaveport sp1 -bus cpu15Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF

ihwaddmemory -instancename stk15   -type ram
ihwconnect   -instancename stk15   -busslaveport sp1 -bus cpu15Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF

########################### peripherals ###########################

# From FreeRTOS hw_platform.h
#define COREUARTAPB0_BASE_ADDR 0x70001000UL
#define COREGPIO_IN_BASE_ADDR  0x70002000UL
#define CORETIMER0_BASE_ADDR   0x70003000UL
#define CORETIMER1_BASE_ADDR   0x70004000UL
#define COREGPIO_OUT_BASE_ADDR 0x70005000UL
#define CORE16550_BASE_ADDR    0x70007000UL
#define PLIC_BASE_ADDR         0x40000000UL

ihwaddperipheral -instancename tea -modelfile peripheral/tea/pse.pse
ihwaddperipheral -instancename repository -modelfile peripheral/repository/pse.pse

ihwaddperipheral -instancename uart0 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart0 -bus cpu0Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic0 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic0 -bus cpu0Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic0 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic0 -type Uns32 -value 1

ihwaddperipheral -instancename prci0 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci0 -bus cpu0Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci0 -type double -value 1000.0

ihwaddperipheral -instancename router0 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router0 -busslaveport localPort -bus cpu0Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router0 -busmasterport RREAD  -bus cpu0Bus
ihwconnect -instancename router0 -busmasterport RWRITE -bus cpu0Bus

ihwaddperipheral -instancename ni0 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni0 -busslaveport DMAC -bus cpu0Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni0 -busmasterport MREAD  -bus cpu0Bus
ihwconnect -instancename ni0 -busmasterport MWRITE -bus cpu0Bus

ihwaddperipheral -instancename printer0 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer0 -busslaveport PRINTREGS -bus cpu0Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart1 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart1 -bus cpu1Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic1 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic1 -bus cpu1Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic1 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic1 -type Uns32 -value 1

ihwaddperipheral -instancename prci1 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci1 -bus cpu1Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci1 -type double -value 1000.0

ihwaddperipheral -instancename router1 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router1 -busslaveport localPort -bus cpu1Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router1 -busmasterport RREAD  -bus cpu1Bus
ihwconnect -instancename router1 -busmasterport RWRITE -bus cpu1Bus

ihwaddperipheral -instancename ni1 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni1 -busslaveport DMAC -bus cpu1Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni1 -busmasterport MREAD  -bus cpu1Bus
ihwconnect -instancename ni1 -busmasterport MWRITE -bus cpu1Bus

ihwaddperipheral -instancename printer1 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer1 -busslaveport PRINTREGS -bus cpu1Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart2 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart2 -bus cpu2Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic2 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic2 -bus cpu2Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic2 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic2 -type Uns32 -value 1

ihwaddperipheral -instancename prci2 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci2 -bus cpu2Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci2 -type double -value 1000.0

ihwaddperipheral -instancename router2 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router2 -busslaveport localPort -bus cpu2Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router2 -busmasterport RREAD  -bus cpu2Bus
ihwconnect -instancename router2 -busmasterport RWRITE -bus cpu2Bus

ihwaddperipheral -instancename ni2 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni2 -busslaveport DMAC -bus cpu2Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni2 -busmasterport MREAD  -bus cpu2Bus
ihwconnect -instancename ni2 -busmasterport MWRITE -bus cpu2Bus

ihwaddperipheral -instancename printer2 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer2 -busslaveport PRINTREGS -bus cpu2Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart3 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart3 -bus cpu3Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic3 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic3 -bus cpu3Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic3 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic3 -type Uns32 -value 1

ihwaddperipheral -instancename prci3 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci3 -bus cpu3Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci3 -type double -value 1000.0

ihwaddperipheral -instancename router3 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router3 -busslaveport localPort -bus cpu3Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router3 -busmasterport RREAD  -bus cpu3Bus
ihwconnect -instancename router3 -busmasterport RWRITE -bus cpu3Bus

ihwaddperipheral -instancename ni3 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni3 -busslaveport DMAC -bus cpu3Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni3 -busmasterport MREAD  -bus cpu3Bus
ihwconnect -instancename ni3 -busmasterport MWRITE -bus cpu3Bus

ihwaddperipheral -instancename printer3 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer3 -busslaveport PRINTREGS -bus cpu3Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart4 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart4 -bus cpu4Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic4 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic4 -bus cpu4Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic4 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic4 -type Uns32 -value 1

ihwaddperipheral -instancename prci4 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci4 -bus cpu4Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci4 -type double -value 1000.0

ihwaddperipheral -instancename router4 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router4 -busslaveport localPort -bus cpu4Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router4 -busmasterport RREAD  -bus cpu4Bus
ihwconnect -instancename router4 -busmasterport RWRITE -bus cpu4Bus

ihwaddperipheral -instancename ni4 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni4 -busslaveport DMAC -bus cpu4Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni4 -busmasterport MREAD  -bus cpu4Bus
ihwconnect -instancename ni4 -busmasterport MWRITE -bus cpu4Bus

ihwaddperipheral -instancename printer4 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer4 -busslaveport PRINTREGS -bus cpu4Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart5 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart5 -bus cpu5Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic5 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic5 -bus cpu5Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic5 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic5 -type Uns32 -value 1

ihwaddperipheral -instancename prci5 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci5 -bus cpu5Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci5 -type double -value 1000.0

ihwaddperipheral -instancename router5 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router5 -busslaveport localPort -bus cpu5Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router5 -busmasterport RREAD  -bus cpu5Bus
ihwconnect -instancename router5 -busmasterport RWRITE -bus cpu5Bus

ihwaddperipheral -instancename ni5 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni5 -busslaveport DMAC -bus cpu5Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni5 -busmasterport MREAD  -bus cpu5Bus
ihwconnect -instancename ni5 -busmasterport MWRITE -bus cpu5Bus

ihwaddperipheral -instancename printer5 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer5 -busslaveport PRINTREGS -bus cpu5Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart6 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart6 -bus cpu6Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic6 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic6 -bus cpu6Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic6 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic6 -type Uns32 -value 1

ihwaddperipheral -instancename prci6 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci6 -bus cpu6Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci6 -type double -value 1000.0

ihwaddperipheral -instancename router6 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router6 -busslaveport localPort -bus cpu6Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router6 -busmasterport RREAD  -bus cpu6Bus
ihwconnect -instancename router6 -busmasterport RWRITE -bus cpu6Bus

ihwaddperipheral -instancename ni6 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni6 -busslaveport DMAC -bus cpu6Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni6 -busmasterport MREAD  -bus cpu6Bus
ihwconnect -instancename ni6 -busmasterport MWRITE -bus cpu6Bus

ihwaddperipheral -instancename printer6 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer6 -busslaveport PRINTREGS -bus cpu6Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart7 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart7 -bus cpu7Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic7 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic7 -bus cpu7Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic7 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic7 -type Uns32 -value 1

ihwaddperipheral -instancename prci7 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci7 -bus cpu7Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci7 -type double -value 1000.0

ihwaddperipheral -instancename router7 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router7 -busslaveport localPort -bus cpu7Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router7 -busmasterport RREAD  -bus cpu7Bus
ihwconnect -instancename router7 -busmasterport RWRITE -bus cpu7Bus

ihwaddperipheral -instancename ni7 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni7 -busslaveport DMAC -bus cpu7Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni7 -busmasterport MREAD  -bus cpu7Bus
ihwconnect -instancename ni7 -busmasterport MWRITE -bus cpu7Bus

ihwaddperipheral -instancename printer7 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer7 -busslaveport PRINTREGS -bus cpu7Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart8 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart8 -bus cpu8Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic8 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic8 -bus cpu8Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic8 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic8 -type Uns32 -value 1

ihwaddperipheral -instancename prci8 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci8 -bus cpu8Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci8 -type double -value 1000.0

ihwaddperipheral -instancename router8 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router8 -busslaveport localPort -bus cpu8Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router8 -busmasterport RREAD  -bus cpu8Bus
ihwconnect -instancename router8 -busmasterport RWRITE -bus cpu8Bus

ihwaddperipheral -instancename ni8 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni8 -busslaveport DMAC -bus cpu8Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni8 -busmasterport MREAD  -bus cpu8Bus
ihwconnect -instancename ni8 -busmasterport MWRITE -bus cpu8Bus

ihwaddperipheral -instancename printer8 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer8 -busslaveport PRINTREGS -bus cpu8Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart9 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart9 -bus cpu9Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic9 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic9 -bus cpu9Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic9 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic9 -type Uns32 -value 1

ihwaddperipheral -instancename prci9 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci9 -bus cpu9Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci9 -type double -value 1000.0

ihwaddperipheral -instancename router9 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router9 -busslaveport localPort -bus cpu9Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router9 -busmasterport RREAD  -bus cpu9Bus
ihwconnect -instancename router9 -busmasterport RWRITE -bus cpu9Bus

ihwaddperipheral -instancename ni9 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni9 -busslaveport DMAC -bus cpu9Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni9 -busmasterport MREAD  -bus cpu9Bus
ihwconnect -instancename ni9 -busmasterport MWRITE -bus cpu9Bus

ihwaddperipheral -instancename printer9 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer9 -busslaveport PRINTREGS -bus cpu9Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart10 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart10 -bus cpu10Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic10 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic10 -bus cpu10Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic10 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic10 -type Uns32 -value 1

ihwaddperipheral -instancename prci10 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci10 -bus cpu10Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci10 -type double -value 1000.0

ihwaddperipheral -instancename router10 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router10 -busslaveport localPort -bus cpu10Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router10 -busmasterport RREAD  -bus cpu10Bus
ihwconnect -instancename router10 -busmasterport RWRITE -bus cpu10Bus

ihwaddperipheral -instancename ni10 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni10 -busslaveport DMAC -bus cpu10Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni10 -busmasterport MREAD  -bus cpu10Bus
ihwconnect -instancename ni10 -busmasterport MWRITE -bus cpu10Bus

ihwaddperipheral -instancename printer10 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer10 -busslaveport PRINTREGS -bus cpu10Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart11 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart11 -bus cpu11Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic11 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic11 -bus cpu11Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic11 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic11 -type Uns32 -value 1

ihwaddperipheral -instancename prci11 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci11 -bus cpu11Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci11 -type double -value 1000.0

ihwaddperipheral -instancename router11 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router11 -busslaveport localPort -bus cpu11Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router11 -busmasterport RREAD  -bus cpu11Bus
ihwconnect -instancename router11 -busmasterport RWRITE -bus cpu11Bus

ihwaddperipheral -instancename ni11 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni11 -busslaveport DMAC -bus cpu11Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni11 -busmasterport MREAD  -bus cpu11Bus
ihwconnect -instancename ni11 -busmasterport MWRITE -bus cpu11Bus

ihwaddperipheral -instancename printer11 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer11 -busslaveport PRINTREGS -bus cpu11Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart12 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart12 -bus cpu12Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic12 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic12 -bus cpu12Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic12 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic12 -type Uns32 -value 1

ihwaddperipheral -instancename prci12 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci12 -bus cpu12Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci12 -type double -value 1000.0

ihwaddperipheral -instancename router12 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router12 -busslaveport localPort -bus cpu12Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router12 -busmasterport RREAD  -bus cpu12Bus
ihwconnect -instancename router12 -busmasterport RWRITE -bus cpu12Bus

ihwaddperipheral -instancename ni12 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni12 -busslaveport DMAC -bus cpu12Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni12 -busmasterport MREAD  -bus cpu12Bus
ihwconnect -instancename ni12 -busmasterport MWRITE -bus cpu12Bus

ihwaddperipheral -instancename printer12 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer12 -busslaveport PRINTREGS -bus cpu12Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart13 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart13 -bus cpu13Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic13 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic13 -bus cpu13Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic13 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic13 -type Uns32 -value 1

ihwaddperipheral -instancename prci13 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci13 -bus cpu13Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci13 -type double -value 1000.0

ihwaddperipheral -instancename router13 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router13 -busslaveport localPort -bus cpu13Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router13 -busmasterport RREAD  -bus cpu13Bus
ihwconnect -instancename router13 -busmasterport RWRITE -bus cpu13Bus

ihwaddperipheral -instancename ni13 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni13 -busslaveport DMAC -bus cpu13Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni13 -busmasterport MREAD  -bus cpu13Bus
ihwconnect -instancename ni13 -busmasterport MWRITE -bus cpu13Bus

ihwaddperipheral -instancename printer13 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer13 -busslaveport PRINTREGS -bus cpu13Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart14 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart14 -bus cpu14Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic14 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic14 -bus cpu14Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic14 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic14 -type Uns32 -value 1

ihwaddperipheral -instancename prci14 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci14 -bus cpu14Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci14 -type double -value 1000.0

ihwaddperipheral -instancename router14 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router14 -busslaveport localPort -bus cpu14Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router14 -busmasterport RREAD  -bus cpu14Bus
ihwconnect -instancename router14 -busmasterport RWRITE -bus cpu14Bus

ihwaddperipheral -instancename ni14 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni14 -busslaveport DMAC -bus cpu14Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni14 -busmasterport MREAD  -bus cpu14Bus
ihwconnect -instancename ni14 -busmasterport MWRITE -bus cpu14Bus

ihwaddperipheral -instancename printer14 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer14 -busslaveport PRINTREGS -bus cpu14Bus -loaddress 0x50000020 -hiaddress 0x50000027

ihwaddperipheral -instancename uart15 \
                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0
ihwconnect -instancename uart15 -bus cpu15Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017

ihwaddperipheral -instancename plic15 -modelfile peripheral/plic/pse.pse
ihwconnect -instancename plic15 -bus cpu15Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF
ihwsetparameter -name num_sources -handle plic15 -type Uns32 -value 256
ihwsetparameter -name num_targets -handle plic15 -type Uns32 -value 1

ihwaddperipheral -instancename prci15 -modelfile peripheral/CLINT/pse.pse
ihwconnect -instancename prci15 -bus cpu15Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF
ihwsetparameter -name clockMHz -handle prci15 -type double -value 1000.0

ihwaddperipheral -instancename router15 -modelfile peripheral/whnoc_dma/pse.pse
ihwconnect -instancename router15 -busslaveport localPort -bus cpu15Bus -loaddress 0x50000000 -hiaddress 0x50000003
ihwconnect -instancename router15 -busmasterport RREAD  -bus cpu15Bus
ihwconnect -instancename router15 -busmasterport RWRITE -bus cpu15Bus

ihwaddperipheral -instancename ni15 -modelfile peripheral/networkInterface/pse.pse
ihwconnect -instancename ni15 -busslaveport DMAC -bus cpu15Bus -loaddress 0x50000004 -hiaddress 0x50000013
ihwconnect -instancename ni15 -busmasterport MREAD  -bus cpu15Bus
ihwconnect -instancename ni15 -busmasterport MWRITE -bus cpu15Bus

ihwaddperipheral -instancename printer15 -modelfile peripheral/printer/pse.pse
ihwconnect -instancename printer15 -busslaveport PRINTREGS -bus cpu15Bus -loaddress 0x50000020 -hiaddress 0x50000027

########################### interrupts ############################

# External Interrupt Port

ihwaddnet -instancename eip0
ihwconnect -net eip0 -instancename plic0 -netport irqT0
ihwconnect -net eip0 -instancename cpu0  -netport MExternalInterrupt

ihwaddnet -instancename eip1
ihwconnect -net eip1 -instancename plic1 -netport irqT0
ihwconnect -net eip1 -instancename cpu1  -netport MExternalInterrupt

ihwaddnet -instancename eip2
ihwconnect -net eip2 -instancename plic2 -netport irqT0
ihwconnect -net eip2 -instancename cpu2  -netport MExternalInterrupt

ihwaddnet -instancename eip3
ihwconnect -net eip3 -instancename plic3 -netport irqT0
ihwconnect -net eip3 -instancename cpu3  -netport MExternalInterrupt

ihwaddnet -instancename eip4
ihwconnect -net eip4 -instancename plic4 -netport irqT0
ihwconnect -net eip4 -instancename cpu4  -netport MExternalInterrupt

ihwaddnet -instancename eip5
ihwconnect -net eip5 -instancename plic5 -netport irqT0
ihwconnect -net eip5 -instancename cpu5  -netport MExternalInterrupt

ihwaddnet -instancename eip6
ihwconnect -net eip6 -instancename plic6 -netport irqT0
ihwconnect -net eip6 -instancename cpu6  -netport MExternalInterrupt

ihwaddnet -instancename eip7
ihwconnect -net eip7 -instancename plic7 -netport irqT0
ihwconnect -net eip7 -instancename cpu7  -netport MExternalInterrupt

ihwaddnet -instancename eip8
ihwconnect -net eip8 -instancename plic8 -netport irqT0
ihwconnect -net eip8 -instancename cpu8  -netport MExternalInterrupt

ihwaddnet -instancename eip9
ihwconnect -net eip9 -instancename plic9 -netport irqT0
ihwconnect -net eip9 -instancename cpu9  -netport MExternalInterrupt

ihwaddnet -instancename eip10
ihwconnect -net eip10 -instancename plic10 -netport irqT0
ihwconnect -net eip10 -instancename cpu10  -netport MExternalInterrupt

ihwaddnet -instancename eip11
ihwconnect -net eip11 -instancename plic11 -netport irqT0
ihwconnect -net eip11 -instancename cpu11  -netport MExternalInterrupt

ihwaddnet -instancename eip12
ihwconnect -net eip12 -instancename plic12 -netport irqT0
ihwconnect -net eip12 -instancename cpu12  -netport MExternalInterrupt

ihwaddnet -instancename eip13
ihwconnect -net eip13 -instancename plic13 -netport irqT0
ihwconnect -net eip13 -instancename cpu13  -netport MExternalInterrupt

ihwaddnet -instancename eip14
ihwconnect -net eip14 -instancename plic14 -netport irqT0
ihwconnect -net eip14 -instancename cpu14  -netport MExternalInterrupt

ihwaddnet -instancename eip15
ihwconnect -net eip15 -instancename plic15 -netport irqT0
ihwconnect -net eip15 -instancename cpu15  -netport MExternalInterrupt

# Local Interrupt Port

ihwaddnet -instancename MTimerInterrupt0
ihwconnect -net MTimerInterrupt0 -instancename prci0 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt0 -instancename cpu0  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt0
ihwconnect -net MSWInterrupt0 -instancename prci0 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt0 -instancename cpu0  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX0
ihwconnect -instancename plic0 -netport irqS2 -net intNI_RX0
ihwconnect -instancename ni0 -netport INT_NI_RX  -net intNI_RX0
ihwaddnet -instancename intNI_TX0
ihwconnect -instancename plic0 -netport irqS3 -net intNI_TX0
ihwconnect -instancename ni0 -netport INT_NI_TX  -net intNI_TX0
ihwaddnet -instancename intNI_TMR0
ihwconnect -instancename plic0 -netport irqS4 -net intNI_TMR0
ihwconnect -instancename ni0 -netport INT_NI_TMR  -net intNI_TMR0

ihwaddnet -instancename MTimerInterrupt1
ihwconnect -net MTimerInterrupt1 -instancename prci1 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt1 -instancename cpu1  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt1
ihwconnect -net MSWInterrupt1 -instancename prci1 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt1 -instancename cpu1  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX1
ihwconnect -instancename plic1 -netport irqS2 -net intNI_RX1
ihwconnect -instancename ni1 -netport INT_NI_RX  -net intNI_RX1
ihwaddnet -instancename intNI_TX1
ihwconnect -instancename plic1 -netport irqS3 -net intNI_TX1
ihwconnect -instancename ni1 -netport INT_NI_TX  -net intNI_TX1
ihwaddnet -instancename intNI_TMR1
ihwconnect -instancename plic1 -netport irqS4 -net intNI_TMR1
ihwconnect -instancename ni1 -netport INT_NI_TMR  -net intNI_TMR1

ihwaddnet -instancename MTimerInterrupt2
ihwconnect -net MTimerInterrupt2 -instancename prci2 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt2 -instancename cpu2  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt2
ihwconnect -net MSWInterrupt2 -instancename prci2 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt2 -instancename cpu2  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX2
ihwconnect -instancename plic2 -netport irqS2 -net intNI_RX2
ihwconnect -instancename ni2 -netport INT_NI_RX  -net intNI_RX2
ihwaddnet -instancename intNI_TX2
ihwconnect -instancename plic2 -netport irqS3 -net intNI_TX2
ihwconnect -instancename ni2 -netport INT_NI_TX  -net intNI_TX2
ihwaddnet -instancename intNI_TMR2
ihwconnect -instancename plic2 -netport irqS4 -net intNI_TMR2
ihwconnect -instancename ni2 -netport INT_NI_TMR  -net intNI_TMR2

ihwaddnet -instancename MTimerInterrupt3
ihwconnect -net MTimerInterrupt3 -instancename prci3 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt3 -instancename cpu3  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt3
ihwconnect -net MSWInterrupt3 -instancename prci3 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt3 -instancename cpu3  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX3
ihwconnect -instancename plic3 -netport irqS2 -net intNI_RX3
ihwconnect -instancename ni3 -netport INT_NI_RX  -net intNI_RX3
ihwaddnet -instancename intNI_TX3
ihwconnect -instancename plic3 -netport irqS3 -net intNI_TX3
ihwconnect -instancename ni3 -netport INT_NI_TX  -net intNI_TX3
ihwaddnet -instancename intNI_TMR3
ihwconnect -instancename plic3 -netport irqS4 -net intNI_TMR3
ihwconnect -instancename ni3 -netport INT_NI_TMR  -net intNI_TMR3

ihwaddnet -instancename MTimerInterrupt4
ihwconnect -net MTimerInterrupt4 -instancename prci4 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt4 -instancename cpu4  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt4
ihwconnect -net MSWInterrupt4 -instancename prci4 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt4 -instancename cpu4  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX4
ihwconnect -instancename plic4 -netport irqS2 -net intNI_RX4
ihwconnect -instancename ni4 -netport INT_NI_RX  -net intNI_RX4
ihwaddnet -instancename intNI_TX4
ihwconnect -instancename plic4 -netport irqS3 -net intNI_TX4
ihwconnect -instancename ni4 -netport INT_NI_TX  -net intNI_TX4
ihwaddnet -instancename intNI_TMR4
ihwconnect -instancename plic4 -netport irqS4 -net intNI_TMR4
ihwconnect -instancename ni4 -netport INT_NI_TMR  -net intNI_TMR4

ihwaddnet -instancename MTimerInterrupt5
ihwconnect -net MTimerInterrupt5 -instancename prci5 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt5 -instancename cpu5  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt5
ihwconnect -net MSWInterrupt5 -instancename prci5 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt5 -instancename cpu5  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX5
ihwconnect -instancename plic5 -netport irqS2 -net intNI_RX5
ihwconnect -instancename ni5 -netport INT_NI_RX  -net intNI_RX5
ihwaddnet -instancename intNI_TX5
ihwconnect -instancename plic5 -netport irqS3 -net intNI_TX5
ihwconnect -instancename ni5 -netport INT_NI_TX  -net intNI_TX5
ihwaddnet -instancename intNI_TMR5
ihwconnect -instancename plic5 -netport irqS4 -net intNI_TMR5
ihwconnect -instancename ni5 -netport INT_NI_TMR  -net intNI_TMR5

ihwaddnet -instancename MTimerInterrupt6
ihwconnect -net MTimerInterrupt6 -instancename prci6 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt6 -instancename cpu6  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt6
ihwconnect -net MSWInterrupt6 -instancename prci6 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt6 -instancename cpu6  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX6
ihwconnect -instancename plic6 -netport irqS2 -net intNI_RX6
ihwconnect -instancename ni6 -netport INT_NI_RX  -net intNI_RX6
ihwaddnet -instancename intNI_TX6
ihwconnect -instancename plic6 -netport irqS3 -net intNI_TX6
ihwconnect -instancename ni6 -netport INT_NI_TX  -net intNI_TX6
ihwaddnet -instancename intNI_TMR6
ihwconnect -instancename plic6 -netport irqS4 -net intNI_TMR6
ihwconnect -instancename ni6 -netport INT_NI_TMR  -net intNI_TMR6

ihwaddnet -instancename MTimerInterrupt7
ihwconnect -net MTimerInterrupt7 -instancename prci7 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt7 -instancename cpu7  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt7
ihwconnect -net MSWInterrupt7 -instancename prci7 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt7 -instancename cpu7  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX7
ihwconnect -instancename plic7 -netport irqS2 -net intNI_RX7
ihwconnect -instancename ni7 -netport INT_NI_RX  -net intNI_RX7
ihwaddnet -instancename intNI_TX7
ihwconnect -instancename plic7 -netport irqS3 -net intNI_TX7
ihwconnect -instancename ni7 -netport INT_NI_TX  -net intNI_TX7
ihwaddnet -instancename intNI_TMR7
ihwconnect -instancename plic7 -netport irqS4 -net intNI_TMR7
ihwconnect -instancename ni7 -netport INT_NI_TMR  -net intNI_TMR7

ihwaddnet -instancename MTimerInterrupt8
ihwconnect -net MTimerInterrupt8 -instancename prci8 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt8 -instancename cpu8  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt8
ihwconnect -net MSWInterrupt8 -instancename prci8 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt8 -instancename cpu8  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX8
ihwconnect -instancename plic8 -netport irqS2 -net intNI_RX8
ihwconnect -instancename ni8 -netport INT_NI_RX  -net intNI_RX8
ihwaddnet -instancename intNI_TX8
ihwconnect -instancename plic8 -netport irqS3 -net intNI_TX8
ihwconnect -instancename ni8 -netport INT_NI_TX  -net intNI_TX8
ihwaddnet -instancename intNI_TMR8
ihwconnect -instancename plic8 -netport irqS4 -net intNI_TMR8
ihwconnect -instancename ni8 -netport INT_NI_TMR  -net intNI_TMR8

ihwaddnet -instancename MTimerInterrupt9
ihwconnect -net MTimerInterrupt9 -instancename prci9 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt9 -instancename cpu9  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt9
ihwconnect -net MSWInterrupt9 -instancename prci9 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt9 -instancename cpu9  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX9
ihwconnect -instancename plic9 -netport irqS2 -net intNI_RX9
ihwconnect -instancename ni9 -netport INT_NI_RX  -net intNI_RX9
ihwaddnet -instancename intNI_TX9
ihwconnect -instancename plic9 -netport irqS3 -net intNI_TX9
ihwconnect -instancename ni9 -netport INT_NI_TX  -net intNI_TX9
ihwaddnet -instancename intNI_TMR9
ihwconnect -instancename plic9 -netport irqS4 -net intNI_TMR9
ihwconnect -instancename ni9 -netport INT_NI_TMR  -net intNI_TMR9

ihwaddnet -instancename MTimerInterrupt10
ihwconnect -net MTimerInterrupt10 -instancename prci10 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt10 -instancename cpu10  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt10
ihwconnect -net MSWInterrupt10 -instancename prci10 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt10 -instancename cpu10  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX10
ihwconnect -instancename plic10 -netport irqS2 -net intNI_RX10
ihwconnect -instancename ni10 -netport INT_NI_RX  -net intNI_RX10
ihwaddnet -instancename intNI_TX10
ihwconnect -instancename plic10 -netport irqS3 -net intNI_TX10
ihwconnect -instancename ni10 -netport INT_NI_TX  -net intNI_TX10
ihwaddnet -instancename intNI_TMR10
ihwconnect -instancename plic10 -netport irqS4 -net intNI_TMR10
ihwconnect -instancename ni10 -netport INT_NI_TMR  -net intNI_TMR10

ihwaddnet -instancename MTimerInterrupt11
ihwconnect -net MTimerInterrupt11 -instancename prci11 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt11 -instancename cpu11  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt11
ihwconnect -net MSWInterrupt11 -instancename prci11 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt11 -instancename cpu11  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX11
ihwconnect -instancename plic11 -netport irqS2 -net intNI_RX11
ihwconnect -instancename ni11 -netport INT_NI_RX  -net intNI_RX11
ihwaddnet -instancename intNI_TX11
ihwconnect -instancename plic11 -netport irqS3 -net intNI_TX11
ihwconnect -instancename ni11 -netport INT_NI_TX  -net intNI_TX11
ihwaddnet -instancename intNI_TMR11
ihwconnect -instancename plic11 -netport irqS4 -net intNI_TMR11
ihwconnect -instancename ni11 -netport INT_NI_TMR  -net intNI_TMR11

ihwaddnet -instancename MTimerInterrupt12
ihwconnect -net MTimerInterrupt12 -instancename prci12 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt12 -instancename cpu12  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt12
ihwconnect -net MSWInterrupt12 -instancename prci12 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt12 -instancename cpu12  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX12
ihwconnect -instancename plic12 -netport irqS2 -net intNI_RX12
ihwconnect -instancename ni12 -netport INT_NI_RX  -net intNI_RX12
ihwaddnet -instancename intNI_TX12
ihwconnect -instancename plic12 -netport irqS3 -net intNI_TX12
ihwconnect -instancename ni12 -netport INT_NI_TX  -net intNI_TX12
ihwaddnet -instancename intNI_TMR12
ihwconnect -instancename plic12 -netport irqS4 -net intNI_TMR12
ihwconnect -instancename ni12 -netport INT_NI_TMR  -net intNI_TMR12

ihwaddnet -instancename MTimerInterrupt13
ihwconnect -net MTimerInterrupt13 -instancename prci13 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt13 -instancename cpu13  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt13
ihwconnect -net MSWInterrupt13 -instancename prci13 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt13 -instancename cpu13  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX13
ihwconnect -instancename plic13 -netport irqS2 -net intNI_RX13
ihwconnect -instancename ni13 -netport INT_NI_RX  -net intNI_RX13
ihwaddnet -instancename intNI_TX13
ihwconnect -instancename plic13 -netport irqS3 -net intNI_TX13
ihwconnect -instancename ni13 -netport INT_NI_TX  -net intNI_TX13
ihwaddnet -instancename intNI_TMR13
ihwconnect -instancename plic13 -netport irqS4 -net intNI_TMR13
ihwconnect -instancename ni13 -netport INT_NI_TMR  -net intNI_TMR13

ihwaddnet -instancename MTimerInterrupt14
ihwconnect -net MTimerInterrupt14 -instancename prci14 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt14 -instancename cpu14  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt14
ihwconnect -net MSWInterrupt14 -instancename prci14 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt14 -instancename cpu14  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX14
ihwconnect -instancename plic14 -netport irqS2 -net intNI_RX14
ihwconnect -instancename ni14 -netport INT_NI_RX  -net intNI_RX14
ihwaddnet -instancename intNI_TX14
ihwconnect -instancename plic14 -netport irqS3 -net intNI_TX14
ihwconnect -instancename ni14 -netport INT_NI_TX  -net intNI_TX14
ihwaddnet -instancename intNI_TMR14
ihwconnect -instancename plic14 -netport irqS4 -net intNI_TMR14
ihwconnect -instancename ni14 -netport INT_NI_TMR  -net intNI_TMR14

ihwaddnet -instancename MTimerInterrupt15
ihwconnect -net MTimerInterrupt15 -instancename prci15 -netport MTimerInterrupt0
ihwconnect -net MTimerInterrupt15 -instancename cpu15  -netport MTimerInterrupt
ihwaddnet -instancename MSWInterrupt15
ihwconnect -net MSWInterrupt15 -instancename prci15 -netport MSWInterrupt0
ihwconnect -net MSWInterrupt15 -instancename cpu15  -netport MSWInterrupt
ihwaddnet -instancename intNI_RX15
ihwconnect -instancename plic15 -netport irqS2 -net intNI_RX15
ihwconnect -instancename ni15 -netport INT_NI_RX  -net intNI_RX15
ihwaddnet -instancename intNI_TX15
ihwconnect -instancename plic15 -netport irqS3 -net intNI_TX15
ihwconnect -instancename ni15 -netport INT_NI_TX  -net intNI_TX15
ihwaddnet -instancename intNI_TMR15
ihwconnect -instancename plic15 -netport irqS4 -net intNI_TMR15
ihwconnect -instancename ni15 -netport INT_NI_TMR  -net intNI_TMR15

ihwaddpacketnet -instancename data_0_0_TEA
ihwaddpacketnet -instancename ctrl_0_0_TEA
ihwaddpacketnet -instancename data_0_0_REPOSITORY
ihwaddpacketnet -instancename ctrl_0_0_REPOSITORY
ihwaddpacketnet -instancename data_0_0_L
ihwaddpacketnet -instancename ctrl_0_0_L
ihwaddpacketnet -instancename data_0_0_E
ihwaddpacketnet -instancename data_0_0_W
ihwaddpacketnet -instancename data_0_0_N
ihwaddpacketnet -instancename data_0_0_S
ihwaddpacketnet -instancename ctrl_0_0_E
ihwaddpacketnet -instancename ctrl_0_0_W
ihwaddpacketnet -instancename ctrl_0_0_N
ihwaddpacketnet -instancename ctrl_0_0_S
ihwaddpacketnet -instancename data_0_1_L
ihwaddpacketnet -instancename ctrl_0_1_L
ihwaddpacketnet -instancename data_0_2_L
ihwaddpacketnet -instancename ctrl_0_2_L
ihwaddpacketnet -instancename data_0_2_E
ihwaddpacketnet -instancename data_0_2_W
ihwaddpacketnet -instancename data_0_2_N
ihwaddpacketnet -instancename data_0_2_S
ihwaddpacketnet -instancename ctrl_0_2_E
ihwaddpacketnet -instancename ctrl_0_2_W
ihwaddpacketnet -instancename ctrl_0_2_N
ihwaddpacketnet -instancename ctrl_0_2_S
ihwaddpacketnet -instancename data_0_3_L
ihwaddpacketnet -instancename ctrl_0_3_L
ihwaddpacketnet -instancename data_1_0_L
ihwaddpacketnet -instancename ctrl_1_0_L
ihwaddpacketnet -instancename data_1_1_L
ihwaddpacketnet -instancename ctrl_1_1_L
ihwaddpacketnet -instancename data_1_1_E
ihwaddpacketnet -instancename data_1_1_W
ihwaddpacketnet -instancename data_1_1_N
ihwaddpacketnet -instancename data_1_1_S
ihwaddpacketnet -instancename ctrl_1_1_E
ihwaddpacketnet -instancename ctrl_1_1_W
ihwaddpacketnet -instancename ctrl_1_1_N
ihwaddpacketnet -instancename ctrl_1_1_S
ihwaddpacketnet -instancename data_1_2_L
ihwaddpacketnet -instancename ctrl_1_2_L
ihwaddpacketnet -instancename data_1_3_L
ihwaddpacketnet -instancename ctrl_1_3_L
ihwaddpacketnet -instancename data_1_3_E
ihwaddpacketnet -instancename data_1_3_W
ihwaddpacketnet -instancename data_1_3_N
ihwaddpacketnet -instancename data_1_3_S
ihwaddpacketnet -instancename ctrl_1_3_E
ihwaddpacketnet -instancename ctrl_1_3_W
ihwaddpacketnet -instancename ctrl_1_3_N
ihwaddpacketnet -instancename ctrl_1_3_S
ihwaddpacketnet -instancename data_2_0_L
ihwaddpacketnet -instancename ctrl_2_0_L
ihwaddpacketnet -instancename data_2_0_E
ihwaddpacketnet -instancename data_2_0_W
ihwaddpacketnet -instancename data_2_0_N
ihwaddpacketnet -instancename data_2_0_S
ihwaddpacketnet -instancename ctrl_2_0_E
ihwaddpacketnet -instancename ctrl_2_0_W
ihwaddpacketnet -instancename ctrl_2_0_N
ihwaddpacketnet -instancename ctrl_2_0_S
ihwaddpacketnet -instancename data_2_1_L
ihwaddpacketnet -instancename ctrl_2_1_L
ihwaddpacketnet -instancename data_2_2_L
ihwaddpacketnet -instancename ctrl_2_2_L
ihwaddpacketnet -instancename data_2_2_E
ihwaddpacketnet -instancename data_2_2_W
ihwaddpacketnet -instancename data_2_2_N
ihwaddpacketnet -instancename data_2_2_S
ihwaddpacketnet -instancename ctrl_2_2_E
ihwaddpacketnet -instancename ctrl_2_2_W
ihwaddpacketnet -instancename ctrl_2_2_N
ihwaddpacketnet -instancename ctrl_2_2_S
ihwaddpacketnet -instancename data_2_3_L
ihwaddpacketnet -instancename ctrl_2_3_L
ihwaddpacketnet -instancename data_3_0_L
ihwaddpacketnet -instancename ctrl_3_0_L
ihwaddpacketnet -instancename data_3_1_L
ihwaddpacketnet -instancename ctrl_3_1_L
ihwaddpacketnet -instancename data_3_1_E
ihwaddpacketnet -instancename data_3_1_W
ihwaddpacketnet -instancename data_3_1_N
ihwaddpacketnet -instancename data_3_1_S
ihwaddpacketnet -instancename ctrl_3_1_E
ihwaddpacketnet -instancename ctrl_3_1_W
ihwaddpacketnet -instancename ctrl_3_1_N
ihwaddpacketnet -instancename ctrl_3_1_S
ihwaddpacketnet -instancename data_3_2_L
ihwaddpacketnet -instancename ctrl_3_2_L
ihwaddpacketnet -instancename data_3_3_L
ihwaddpacketnet -instancename ctrl_3_3_L
ihwaddpacketnet -instancename data_3_3_E
ihwaddpacketnet -instancename data_3_3_W
ihwaddpacketnet -instancename data_3_3_N
ihwaddpacketnet -instancename data_3_3_S
ihwaddpacketnet -instancename ctrl_3_3_E
ihwaddpacketnet -instancename ctrl_3_3_W
ihwaddpacketnet -instancename ctrl_3_3_N
ihwaddpacketnet -instancename ctrl_3_3_S

ihwconnect -instancename router0 -packetnetport portDataWest -packetnet data_0_0_TEA
ihwconnect -instancename router0 -packetnetport portControlWest -packetnet ctrl_0_0_TEA
ihwconnect -instancename tea -packetnetport portData -packetnet data_0_0_TEA
ihwconnect -instancename tea -packetnetport portControl -packetnet ctrl_0_0_TEA
ihwconnect -instancename router0 -packetnetport portDataSouth -packetnet data_0_0_REPOSITORY
ihwconnect -instancename router0 -packetnetport portControlSouth -packetnet ctrl_0_0_REPOSITORY
ihwconnect -instancename repository -packetnetport portData -packetnet data_0_0_REPOSITORY
ihwconnect -instancename repository -packetnetport portControl -packetnet ctrl_0_0_REPOSITORY
ihwconnect -instancename router0 -packetnetport portDataLocal -packetnet data_0_0_L
ihwconnect -instancename ni0 -packetnetport dataPort -packetnet data_0_0_L
ihwconnect -instancename router0 -packetnetport portControlLocal -packetnet ctrl_0_0_L
ihwconnect -instancename ni0 -packetnetport controlPort -packetnet ctrl_0_0_L
ihwconnect -instancename router0 -packetnetport portDataEast -packetnet data_0_0_E
ihwconnect -instancename router1 -packetnetport portDataWest -packetnet data_0_0_E
ihwconnect -instancename router0 -packetnetport portControlEast -packetnet ctrl_0_0_E
ihwconnect -instancename router1 -packetnetport portControlWest -packetnet ctrl_0_0_E
ihwconnect -instancename router0 -packetnetport portDataNorth -packetnet data_0_0_N
ihwconnect -instancename router4 -packetnetport portDataSouth -packetnet data_0_0_N
ihwconnect -instancename router0 -packetnetport portControlNorth -packetnet ctrl_0_0_N
ihwconnect -instancename router4 -packetnetport portControlSouth -packetnet ctrl_0_0_N
ihwconnect -instancename router1 -packetnetport portDataLocal -packetnet data_0_1_L
ihwconnect -instancename ni1 -packetnetport dataPort -packetnet data_0_1_L
ihwconnect -instancename router1 -packetnetport portControlLocal -packetnet ctrl_0_1_L
ihwconnect -instancename ni1 -packetnetport controlPort -packetnet ctrl_0_1_L
ihwconnect -instancename router2 -packetnetport portDataLocal -packetnet data_0_2_L
ihwconnect -instancename ni2 -packetnetport dataPort -packetnet data_0_2_L
ihwconnect -instancename router2 -packetnetport portControlLocal -packetnet ctrl_0_2_L
ihwconnect -instancename ni2 -packetnetport controlPort -packetnet ctrl_0_2_L
ihwconnect -instancename router2 -packetnetport portDataEast -packetnet data_0_2_E
ihwconnect -instancename router3 -packetnetport portDataWest -packetnet data_0_2_E
ihwconnect -instancename router2 -packetnetport portControlEast -packetnet ctrl_0_2_E
ihwconnect -instancename router3 -packetnetport portControlWest -packetnet ctrl_0_2_E
ihwconnect -instancename router2 -packetnetport portDataWest -packetnet data_0_2_W
ihwconnect -instancename router1 -packetnetport portDataEast -packetnet data_0_2_W
ihwconnect -instancename router2 -packetnetport portControlWest -packetnet ctrl_0_2_W
ihwconnect -instancename router1 -packetnetport portControlEast -packetnet ctrl_0_2_W
ihwconnect -instancename router2 -packetnetport portDataNorth -packetnet data_0_2_N
ihwconnect -instancename router6 -packetnetport portDataSouth -packetnet data_0_2_N
ihwconnect -instancename router2 -packetnetport portControlNorth -packetnet ctrl_0_2_N
ihwconnect -instancename router6 -packetnetport portControlSouth -packetnet ctrl_0_2_N
ihwconnect -instancename router3 -packetnetport portDataLocal -packetnet data_0_3_L
ihwconnect -instancename ni3 -packetnetport dataPort -packetnet data_0_3_L
ihwconnect -instancename router3 -packetnetport portControlLocal -packetnet ctrl_0_3_L
ihwconnect -instancename ni3 -packetnetport controlPort -packetnet ctrl_0_3_L
ihwconnect -instancename router4 -packetnetport portDataLocal -packetnet data_1_0_L
ihwconnect -instancename ni4 -packetnetport dataPort -packetnet data_1_0_L
ihwconnect -instancename router4 -packetnetport portControlLocal -packetnet ctrl_1_0_L
ihwconnect -instancename ni4 -packetnetport controlPort -packetnet ctrl_1_0_L
ihwconnect -instancename router5 -packetnetport portDataLocal -packetnet data_1_1_L
ihwconnect -instancename ni5 -packetnetport dataPort -packetnet data_1_1_L
ihwconnect -instancename router5 -packetnetport portControlLocal -packetnet ctrl_1_1_L
ihwconnect -instancename ni5 -packetnetport controlPort -packetnet ctrl_1_1_L
ihwconnect -instancename router5 -packetnetport portDataEast -packetnet data_1_1_E
ihwconnect -instancename router6 -packetnetport portDataWest -packetnet data_1_1_E
ihwconnect -instancename router5 -packetnetport portControlEast -packetnet ctrl_1_1_E
ihwconnect -instancename router6 -packetnetport portControlWest -packetnet ctrl_1_1_E
ihwconnect -instancename router5 -packetnetport portDataWest -packetnet data_1_1_W
ihwconnect -instancename router4 -packetnetport portDataEast -packetnet data_1_1_W
ihwconnect -instancename router5 -packetnetport portControlWest -packetnet ctrl_1_1_W
ihwconnect -instancename router4 -packetnetport portControlEast -packetnet ctrl_1_1_W
ihwconnect -instancename router5 -packetnetport portDataNorth -packetnet data_1_1_N
ihwconnect -instancename router9 -packetnetport portDataSouth -packetnet data_1_1_N
ihwconnect -instancename router5 -packetnetport portControlNorth -packetnet ctrl_1_1_N
ihwconnect -instancename router9 -packetnetport portControlSouth -packetnet ctrl_1_1_N
ihwconnect -instancename router5 -packetnetport portDataSouth -packetnet data_1_1_S
ihwconnect -instancename router1 -packetnetport portDataNorth -packetnet data_1_1_S
ihwconnect -instancename router5 -packetnetport portControlSouth -packetnet ctrl_1_1_S
ihwconnect -instancename router1 -packetnetport portControlNorth -packetnet ctrl_1_1_S
ihwconnect -instancename router6 -packetnetport portDataLocal -packetnet data_1_2_L
ihwconnect -instancename ni6 -packetnetport dataPort -packetnet data_1_2_L
ihwconnect -instancename router6 -packetnetport portControlLocal -packetnet ctrl_1_2_L
ihwconnect -instancename ni6 -packetnetport controlPort -packetnet ctrl_1_2_L
ihwconnect -instancename router7 -packetnetport portDataLocal -packetnet data_1_3_L
ihwconnect -instancename ni7 -packetnetport dataPort -packetnet data_1_3_L
ihwconnect -instancename router7 -packetnetport portControlLocal -packetnet ctrl_1_3_L
ihwconnect -instancename ni7 -packetnetport controlPort -packetnet ctrl_1_3_L
ihwconnect -instancename router7 -packetnetport portDataWest -packetnet data_1_3_W
ihwconnect -instancename router6 -packetnetport portDataEast -packetnet data_1_3_W
ihwconnect -instancename router7 -packetnetport portControlWest -packetnet ctrl_1_3_W
ihwconnect -instancename router6 -packetnetport portControlEast -packetnet ctrl_1_3_W
ihwconnect -instancename router7 -packetnetport portDataNorth -packetnet data_1_3_N
ihwconnect -instancename router11 -packetnetport portDataSouth -packetnet data_1_3_N
ihwconnect -instancename router7 -packetnetport portControlNorth -packetnet ctrl_1_3_N
ihwconnect -instancename router11 -packetnetport portControlSouth -packetnet ctrl_1_3_N
ihwconnect -instancename router7 -packetnetport portDataSouth -packetnet data_1_3_S
ihwconnect -instancename router3 -packetnetport portDataNorth -packetnet data_1_3_S
ihwconnect -instancename router7 -packetnetport portControlSouth -packetnet ctrl_1_3_S
ihwconnect -instancename router3 -packetnetport portControlNorth -packetnet ctrl_1_3_S
ihwconnect -instancename router8 -packetnetport portDataLocal -packetnet data_2_0_L
ihwconnect -instancename ni8 -packetnetport dataPort -packetnet data_2_0_L
ihwconnect -instancename router8 -packetnetport portControlLocal -packetnet ctrl_2_0_L
ihwconnect -instancename ni8 -packetnetport controlPort -packetnet ctrl_2_0_L
ihwconnect -instancename router8 -packetnetport portDataEast -packetnet data_2_0_E
ihwconnect -instancename router9 -packetnetport portDataWest -packetnet data_2_0_E
ihwconnect -instancename router8 -packetnetport portControlEast -packetnet ctrl_2_0_E
ihwconnect -instancename router9 -packetnetport portControlWest -packetnet ctrl_2_0_E
ihwconnect -instancename router8 -packetnetport portDataNorth -packetnet data_2_0_N
ihwconnect -instancename router12 -packetnetport portDataSouth -packetnet data_2_0_N
ihwconnect -instancename router8 -packetnetport portControlNorth -packetnet ctrl_2_0_N
ihwconnect -instancename router12 -packetnetport portControlSouth -packetnet ctrl_2_0_N
ihwconnect -instancename router8 -packetnetport portDataSouth -packetnet data_2_0_S
ihwconnect -instancename router4 -packetnetport portDataNorth -packetnet data_2_0_S
ihwconnect -instancename router8 -packetnetport portControlSouth -packetnet ctrl_2_0_S
ihwconnect -instancename router4 -packetnetport portControlNorth -packetnet ctrl_2_0_S
ihwconnect -instancename router9 -packetnetport portDataLocal -packetnet data_2_1_L
ihwconnect -instancename ni9 -packetnetport dataPort -packetnet data_2_1_L
ihwconnect -instancename router9 -packetnetport portControlLocal -packetnet ctrl_2_1_L
ihwconnect -instancename ni9 -packetnetport controlPort -packetnet ctrl_2_1_L
ihwconnect -instancename router10 -packetnetport portDataLocal -packetnet data_2_2_L
ihwconnect -instancename ni10 -packetnetport dataPort -packetnet data_2_2_L
ihwconnect -instancename router10 -packetnetport portControlLocal -packetnet ctrl_2_2_L
ihwconnect -instancename ni10 -packetnetport controlPort -packetnet ctrl_2_2_L
ihwconnect -instancename router10 -packetnetport portDataEast -packetnet data_2_2_E
ihwconnect -instancename router11 -packetnetport portDataWest -packetnet data_2_2_E
ihwconnect -instancename router10 -packetnetport portControlEast -packetnet ctrl_2_2_E
ihwconnect -instancename router11 -packetnetport portControlWest -packetnet ctrl_2_2_E
ihwconnect -instancename router10 -packetnetport portDataWest -packetnet data_2_2_W
ihwconnect -instancename router9 -packetnetport portDataEast -packetnet data_2_2_W
ihwconnect -instancename router10 -packetnetport portControlWest -packetnet ctrl_2_2_W
ihwconnect -instancename router9 -packetnetport portControlEast -packetnet ctrl_2_2_W
ihwconnect -instancename router10 -packetnetport portDataNorth -packetnet data_2_2_N
ihwconnect -instancename router14 -packetnetport portDataSouth -packetnet data_2_2_N
ihwconnect -instancename router10 -packetnetport portControlNorth -packetnet ctrl_2_2_N
ihwconnect -instancename router14 -packetnetport portControlSouth -packetnet ctrl_2_2_N
ihwconnect -instancename router10 -packetnetport portDataSouth -packetnet data_2_2_S
ihwconnect -instancename router6 -packetnetport portDataNorth -packetnet data_2_2_S
ihwconnect -instancename router10 -packetnetport portControlSouth -packetnet ctrl_2_2_S
ihwconnect -instancename router6 -packetnetport portControlNorth -packetnet ctrl_2_2_S
ihwconnect -instancename router11 -packetnetport portDataLocal -packetnet data_2_3_L
ihwconnect -instancename ni11 -packetnetport dataPort -packetnet data_2_3_L
ihwconnect -instancename router11 -packetnetport portControlLocal -packetnet ctrl_2_3_L
ihwconnect -instancename ni11 -packetnetport controlPort -packetnet ctrl_2_3_L
ihwconnect -instancename router12 -packetnetport portDataLocal -packetnet data_3_0_L
ihwconnect -instancename ni12 -packetnetport dataPort -packetnet data_3_0_L
ihwconnect -instancename router12 -packetnetport portControlLocal -packetnet ctrl_3_0_L
ihwconnect -instancename ni12 -packetnetport controlPort -packetnet ctrl_3_0_L
ihwconnect -instancename router13 -packetnetport portDataLocal -packetnet data_3_1_L
ihwconnect -instancename ni13 -packetnetport dataPort -packetnet data_3_1_L
ihwconnect -instancename router13 -packetnetport portControlLocal -packetnet ctrl_3_1_L
ihwconnect -instancename ni13 -packetnetport controlPort -packetnet ctrl_3_1_L
ihwconnect -instancename router13 -packetnetport portDataEast -packetnet data_3_1_E
ihwconnect -instancename router14 -packetnetport portDataWest -packetnet data_3_1_E
ihwconnect -instancename router13 -packetnetport portControlEast -packetnet ctrl_3_1_E
ihwconnect -instancename router14 -packetnetport portControlWest -packetnet ctrl_3_1_E
ihwconnect -instancename router13 -packetnetport portDataWest -packetnet data_3_1_W
ihwconnect -instancename router12 -packetnetport portDataEast -packetnet data_3_1_W
ihwconnect -instancename router13 -packetnetport portControlWest -packetnet ctrl_3_1_W
ihwconnect -instancename router12 -packetnetport portControlEast -packetnet ctrl_3_1_W
ihwconnect -instancename router13 -packetnetport portDataSouth -packetnet data_3_1_S
ihwconnect -instancename router9 -packetnetport portDataNorth -packetnet data_3_1_S
ihwconnect -instancename router13 -packetnetport portControlSouth -packetnet ctrl_3_1_S
ihwconnect -instancename router9 -packetnetport portControlNorth -packetnet ctrl_3_1_S
ihwconnect -instancename router14 -packetnetport portDataLocal -packetnet data_3_2_L
ihwconnect -instancename ni14 -packetnetport dataPort -packetnet data_3_2_L
ihwconnect -instancename router14 -packetnetport portControlLocal -packetnet ctrl_3_2_L
ihwconnect -instancename ni14 -packetnetport controlPort -packetnet ctrl_3_2_L
ihwconnect -instancename router15 -packetnetport portDataLocal -packetnet data_3_3_L
ihwconnect -instancename ni15 -packetnetport dataPort -packetnet data_3_3_L
ihwconnect -instancename router15 -packetnetport portControlLocal -packetnet ctrl_3_3_L
ihwconnect -instancename ni15 -packetnetport controlPort -packetnet ctrl_3_3_L
ihwconnect -instancename router15 -packetnetport portDataWest -packetnet data_3_3_W
ihwconnect -instancename router14 -packetnetport portDataEast -packetnet data_3_3_W
ihwconnect -instancename router15 -packetnetport portControlWest -packetnet ctrl_3_3_W
ihwconnect -instancename router14 -packetnetport portControlEast -packetnet ctrl_3_3_W
ihwconnect -instancename router15 -packetnetport portDataSouth -packetnet data_3_3_S
ihwconnect -instancename router11 -packetnetport portDataNorth -packetnet data_3_3_S
ihwconnect -instancename router15 -packetnetport portControlSouth -packetnet ctrl_3_3_S
ihwconnect -instancename router11 -packetnetport portControlNorth -packetnet ctrl_3_3_S

ihwaddperipheral -instancename iterator -modelfile peripheral/iterator/pse.pse

ihwaddpacketnet -instancename iteration_0
ihwconnect -instancename router0 -packetnetport iterationsPort -packetnet iteration_0
ihwconnect -instancename iterator -packetnetport iterationPort0 -packetnet iteration_0
ihwaddpacketnet -instancename iteration_1
ihwconnect -instancename router1 -packetnetport iterationsPort -packetnet iteration_1
ihwconnect -instancename iterator -packetnetport iterationPort1 -packetnet iteration_1
ihwaddpacketnet -instancename iteration_2
ihwconnect -instancename router2 -packetnetport iterationsPort -packetnet iteration_2
ihwconnect -instancename iterator -packetnetport iterationPort2 -packetnet iteration_2
ihwaddpacketnet -instancename iteration_3
ihwconnect -instancename router3 -packetnetport iterationsPort -packetnet iteration_3
ihwconnect -instancename iterator -packetnetport iterationPort3 -packetnet iteration_3
ihwaddpacketnet -instancename iteration_4
ihwconnect -instancename router4 -packetnetport iterationsPort -packetnet iteration_4
ihwconnect -instancename iterator -packetnetport iterationPort4 -packetnet iteration_4
ihwaddpacketnet -instancename iteration_5
ihwconnect -instancename router5 -packetnetport iterationsPort -packetnet iteration_5
ihwconnect -instancename iterator -packetnetport iterationPort5 -packetnet iteration_5
ihwaddpacketnet -instancename iteration_6
ihwconnect -instancename router6 -packetnetport iterationsPort -packetnet iteration_6
ihwconnect -instancename iterator -packetnetport iterationPort6 -packetnet iteration_6
ihwaddpacketnet -instancename iteration_7
ihwconnect -instancename router7 -packetnetport iterationsPort -packetnet iteration_7
ihwconnect -instancename iterator -packetnetport iterationPort7 -packetnet iteration_7
ihwaddpacketnet -instancename iteration_8
ihwconnect -instancename router8 -packetnetport iterationsPort -packetnet iteration_8
ihwconnect -instancename iterator -packetnetport iterationPort8 -packetnet iteration_8
ihwaddpacketnet -instancename iteration_9
ihwconnect -instancename router9 -packetnetport iterationsPort -packetnet iteration_9
ihwconnect -instancename iterator -packetnetport iterationPort9 -packetnet iteration_9
ihwaddpacketnet -instancename iteration_10
ihwconnect -instancename router10 -packetnetport iterationsPort -packetnet iteration_10
ihwconnect -instancename iterator -packetnetport iterationPort10 -packetnet iteration_10
ihwaddpacketnet -instancename iteration_11
ihwconnect -instancename router11 -packetnetport iterationsPort -packetnet iteration_11
ihwconnect -instancename iterator -packetnetport iterationPort11 -packetnet iteration_11
ihwaddpacketnet -instancename iteration_12
ihwconnect -instancename router12 -packetnetport iterationsPort -packetnet iteration_12
ihwconnect -instancename iterator -packetnetport iterationPort12 -packetnet iteration_12
ihwaddpacketnet -instancename iteration_13
ihwconnect -instancename router13 -packetnetport iterationsPort -packetnet iteration_13
ihwconnect -instancename iterator -packetnetport iterationPort13 -packetnet iteration_13
ihwaddpacketnet -instancename iteration_14
ihwconnect -instancename router14 -packetnetport iterationsPort -packetnet iteration_14
ihwconnect -instancename iterator -packetnetport iterationPort14 -packetnet iteration_14
ihwaddpacketnet -instancename iteration_15
ihwconnect -instancename router15 -packetnetport iterationsPort -packetnet iteration_15
ihwconnect -instancename iterator -packetnetport iterationPort15 -packetnet iteration_15

