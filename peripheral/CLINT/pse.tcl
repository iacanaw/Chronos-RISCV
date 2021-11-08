#
# Copyright (c) 2005-2021 Imperas Software Ltd., www.imperas.com
#
# The contents of this file are provided under the Software License
# Agreement that you accepted before downloading this file.
#
# This source forms part of the Software and can be used for educational,
# training, and demonstration purposes but cannot be used for derivative
# works except in cases where the derivative works require OVP technology
# to run.
#
# For open source models released under licenses that you can use for
# derivative works, please visit www.OVPworld.org or www.imperas.com
# for the location of the open source models.
#

set vendor  riscv.ovpworld.org
set library peripheral
set name    CLINT
set version 1.0

imodelnewperipheral -name $name -imagefile pse.pse -library $library -vendor $vendor -version $version \
                    -constructor constructor -releasestatus ovp -netportuserfunction nextNetPortUser

iadddocumentation -name Licensing   -text "Open Source Apache 2.0"
iadddocumentation -name Description -text "
Risc-V Core Local Interruptor (CLINT).
Use the num_harts parameter to specify the number of harts suported (default 1).
For each supported hart there will be an MTimerInterruptN and MSWInterruptN net port, plus msipN and mtimecmpN registers implemented, where N is a value from 0..num_harts-1
There is also a single mtime register.
"
iadddocumentation -name Limitations -text "Writes to mtime register are not supported"
iadddocumentation -name Reference   -text "SiFive Freedom U540-C000 Manual FU540-C000-v1.0.pdf (https://www.sifive.com/documentation/chips/freedom-u540-c000-manual)"

imodeladdformal   -name num_harts -type Uns32 -defaultvalue 1 -min 1 -max 4095
iadddocumentation -name num_harts -text "Number of harts implemented"

imodeladdformal   -name clockMHz -type double -defaultvalue 10.0 -min 0.001 -max 1000.0
iadddocumentation -name clockMHz -text "Clock rate used in timer calculations (in MHz)"

#
# Net ports
#
imodeladdnetport -name MTimerInterrupt0 -type output
imodeladdnetport -name MSWInterrupt0    -type output

#
# Slave ports
#
imodeladdbusslaveport -name port0 -size 0xC000

#
# msip address block
#
imodeladdaddressblock -name msip -port port0 -size 0x4000 -offset 0x0 -width 32
imodeladdlocalmemory  -name msipMem -addressblock port0/msip -size 0x4000 \
                      -writefunction reservedWR -readfunction reservedRD -userdata 0x0

imodeladdmmregister   -name msip0 -addressblock port0/msip -offset 0x0 -width 32 -access rw -writemask 0x1 -nonvolatile \
                      -readfunction msipRead -writefunction msipWrite -viewfunction msipView -userdata 0
  iadddocumentation   -name msip0 -text "Hart 0 Machine-Mode Software Interrupt"

#
# mtime address block (Note: 64 bit registers must be modeled as Hi/Lo 32 bit registers due to API restrictions)
#
imodeladdaddressblock -name mtime -port port0 -size 0x8000 -offset 0x4000 -width 32
imodeladdlocalmemory  -name mtimeMem -addressblock port0/mtime -size 0x8000 \
                      -writefunction reservedWR -readfunction reservedRD -userdata 0x4000

imodeladdmmregister   -name mtimecmpLo0 -addressblock port0/mtime -offset 0x0 -width 32 -access rw \
                      -readfunction mtimecmpRead -writefunction mtimecmpWrite -viewfunction mtimecmpViewLo -userdata 0
  iadddocumentation   -name mtimecmpLo0 -text "Hart 0 Machine-Mode Timer Compare Low"
  
imodeladdmmregister   -name mtimecmpHi0 -addressblock port0/mtime -offset 0x4 -width 32 -access rw \
                      -readfunction mtimecmpRead -writefunction mtimecmpWrite -viewfunction mtimecmpViewHi -userdata 0
  iadddocumentation   -name mtimecmpHi0 -text "Hart 0 Machine-Mode Timer Compare High"

imodeladdmmregister   -name mtimeLo     -addressblock port0/mtime -offset 0x7ff8 -width 32 -access r \
                      -readfunction mtimeRead  -viewfunction mtimeViewLo
  iadddocumentation   -name mtime -text "Machine-Mode mtime Low"
  
imodeladdmmregister   -name mtimeHi     -addressblock port0/mtime -offset 0x7ffc -width 32 -access r \
                      -readfunction mtimeRead  -viewfunction mtimeViewHi
  iadddocumentation   -name mtime -text "Machine-Mode mtime High"
