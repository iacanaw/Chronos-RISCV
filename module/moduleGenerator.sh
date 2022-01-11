#!/bin/sh

# Parameters
X=$1
Y=$2

# Removes the old .tcl
rm -rf module.op.tcl

# Defines the module name
echo "ihwnew -name Chronos_RiscV_FreeRTOS" >> module.op.tcl
echo "" >> module.op.tcl

# Creates one N processors that will be connected to the NoC
echo "############################ bus ##################################" >> module.op.tcl
echo "" >> module.op.tcl
N=$((X*Y))
N=$(($N-1))
for i in $(seq 0 $N);
do
	echo "ihwaddbus -instancename cpu"$i"Bus -addresswidth 32" >> module.op.tcl
done
echo "" >> module.op.tcl

# Defines the processor type
echo "############################ processor ############################" >> module.op.tcl
echo "" >> module.op.tcl
for i in $(seq 0 $N);
do
	echo "ihwaddprocessor -instancename cpu"$i" -id "$i"  \\" >> module.op.tcl
	echo "	-type riscv -vendor riscv.ovpworld.org -library processor -version 1.0 \\" >> module.op.tcl
	echo "	-mips 1000 -simulateexceptions" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwsetparameter -handle cpu"$i" -name variant        -value RVB32I -type enum" >> module.op.tcl
	echo "ihwsetparameter -handle cpu"$i" -name add_Extensions -value MSU -type string" >> module.op.tcl
	echo "ihwsetparameter -handle cpu"$i" -name external_int_id -value True -type Bool" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwconnect -instancename cpu"$i"  -busmasterport INSTRUCTION -bus cpu"$i"Bus" >> module.op.tcl
	echo "ihwconnect -instancename cpu"$i"  -busmasterport DATA        -bus cpu"$i"Bus" >> module.op.tcl
	echo "" >> module.op.tcl
done

# Configures the memory
echo "############################## memory #############################" >> module.op.tcl
for i in $(seq 0 $N);
do
	echo "" >> module.op.tcl
	echo "ihwaddmemory -instancename nvram"$i" -type ram" >> module.op.tcl
	echo "ihwconnect   -instancename nvram"$i" -busslaveport sp1 -bus cpu"$i"Bus -loaddress 0x60000000 -hiaddress 0x6FFFFFFF" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwaddmemory -instancename ddr"$i"   -type ram" >> module.op.tcl
	echo "ihwconnect   -instancename ddr"$i"   -busslaveport sp1 -bus cpu"$i"Bus -loaddress 0x80000000 -hiaddress 0x8FFFFFFF" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwaddmemory -instancename stk"$i"   -type ram" >> module.op.tcl
	echo "ihwconnect   -instancename stk"$i"   -busslaveport sp1 -bus cpu"$i"Bus -loaddress 0xfff00000 -hiaddress 0xFFFFFFFF" >> module.op.tcl
	echo "" >> module.op.tcl
done

echo "########################### peripherals ###########################" >> module.op.tcl
echo "" >> module.op.tcl
echo "# From FreeRTOS hw_platform.h" >> module.op.tcl
echo "#define COREUARTAPB0_BASE_ADDR 0x70001000UL" >> module.op.tcl
echo "#define COREGPIO_IN_BASE_ADDR  0x70002000UL" >> module.op.tcl
echo "#define CORETIMER0_BASE_ADDR   0x70003000UL" >> module.op.tcl
echo "#define CORETIMER1_BASE_ADDR   0x70004000UL" >> module.op.tcl
echo "#define COREGPIO_OUT_BASE_ADDR 0x70005000UL" >> module.op.tcl
echo "#define CORE16550_BASE_ADDR    0x70007000UL" >> module.op.tcl
echo "#define PLIC_BASE_ADDR         0x40000000UL" >> module.op.tcl
echo "" >> module.op.tcl
echo "ihwaddperipheral -instancename tea -modelfile peripheral/tea/pse.pse" >> module.op.tcl
echo "ihwaddperipheral -instancename repository -modelfile peripheral/repository/pse.pse" >> module.op.tcl
echo "" >> module.op.tcl
for i in $(seq 0 $N);
do
	echo "ihwaddperipheral -instancename uart"$i" \\" >> module.op.tcl
	echo "                 -vendor microsemi.ovpworld.org -library peripheral -type CoreUARTapb -version 1.0" >> module.op.tcl
	echo "ihwconnect -instancename uart"$i" -bus cpu"$i"Bus -busslaveport port0 -loaddress 0x70001000 -hiaddress 0x70001017" >> module.op.tcl
	echo "" >> module.op.tcl
#	echo "ihwaddperipheral -instancename plic"$i" -vendor riscv.ovpworld.org -library peripheral -type PLIC -version 1.0" >> module.op.tcl
	echo "ihwaddperipheral -instancename plic"$i" -modelfile peripheral/plic/pse.pse" >> module.op.tcl
	echo "ihwconnect -instancename plic"$i" -bus cpu"$i"Bus -busslaveport port0 -loaddress 0x40000000 -hiaddress 0x43FFFFFF" >> module.op.tcl
	echo "ihwsetparameter -name num_sources -handle plic"$i" -type Uns32 -value 256" >> module.op.tcl
	echo "ihwsetparameter -name num_targets -handle plic"$i" -type Uns32 -value 1" >> module.op.tcl
	echo "" >> module.op.tcl
	#echo "ihwaddperipheral -instancename prci"$i" -vendor riscv.ovpworld.org -library peripheral -type CLINT -version 1.0" >> module.op.tcl
	echo "ihwaddperipheral -instancename prci"$i" -modelfile peripheral/CLINT/pse.pse" >> module.op.tcl
	echo "ihwconnect -instancename prci"$i" -bus cpu"$i"Bus -busslaveport port0 -loaddress 0x44000000 -hiaddress 0x4400BFFF" >> module.op.tcl
	echo "ihwsetparameter -name clockMHz -handle prci"$i" -type double -value 1000.0" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwaddperipheral -instancename router"$i" -modelfile peripheral/whnoc_dma/pse.pse" >> module.op.tcl
	echo "ihwconnect -instancename router"$i" -busslaveport localPort -bus cpu"$i"Bus -loaddress 0x50000000 -hiaddress 0x50000003" >> module.op.tcl
	echo "ihwconnect -instancename router"$i" -busmasterport RREAD  -bus cpu"$i"Bus" >> module.op.tcl
	echo "ihwconnect -instancename router"$i" -busmasterport RWRITE -bus cpu"$i"Bus" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwaddperipheral -instancename ni"$i" -modelfile peripheral/networkInterface/pse.pse" >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -busslaveport DMAC -bus cpu"$i"Bus -loaddress 0x50000004 -hiaddress 0x50000013" >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -busmasterport MREAD  -bus cpu"$i"Bus" >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -busmasterport MWRITE -bus cpu"$i"Bus" >> module.op.tcl
	echo "" >> module.op.tcl
	echo "ihwaddperipheral -instancename printer"$i" -modelfile peripheral/printer/pse.pse" >> module.op.tcl
	echo "ihwconnect -instancename printer"$i" -busslaveport PRINTREGS -bus cpu"$i"Bus -loaddress 0x50000020 -hiaddress 0x50000027" >> module.op.tcl
	echo "" >> module.op.tcl
done

echo "########################### interrupts ############################" >> module.op.tcl
echo "" >> module.op.tcl
echo "# External Interrupt Port" >> module.op.tcl
echo "" >> module.op.tcl
for i in $(seq 0 $N);
do
	echo "ihwaddnet -instancename eip"$i"" >> module.op.tcl
	echo "ihwconnect -net eip"$i" -instancename plic"$i" -netport irqT0" >> module.op.tcl
	echo "ihwconnect -net eip"$i" -instancename cpu"$i"  -netport MExternalInterrupt" >> module.op.tcl
	echo "" >> module.op.tcl
done
echo "# Local Interrupt Port" >> module.op.tcl
echo "" >> module.op.tcl
for i in $(seq 0 $N);
do
	echo "ihwaddnet -instancename MTimerInterrupt"$i >> module.op.tcl
	echo "ihwconnect -net MTimerInterrupt"$i" -instancename prci"$i" -netport MTimerInterrupt0" >> module.op.tcl
	echo "ihwconnect -net MTimerInterrupt"$i" -instancename cpu"$i"  -netport MTimerInterrupt" >> module.op.tcl
	echo "ihwaddnet -instancename MSWInterrupt"$i >> module.op.tcl
	echo "ihwconnect -net MSWInterrupt"$i" -instancename prci"$i" -netport MSWInterrupt0" >> module.op.tcl
	echo "ihwconnect -net MSWInterrupt"$i" -instancename cpu"$i"  -netport MSWInterrupt" >> module.op.tcl
	echo "ihwaddnet -instancename intNI_RX"$i >> module.op.tcl
	echo "ihwconnect -instancename plic"$i" -netport irqS2 -net intNI_RX"$i >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -netport INT_NI_RX  -net intNI_RX"$i >> module.op.tcl
	echo "ihwaddnet -instancename intNI_TX"$i >> module.op.tcl
	echo "ihwconnect -instancename plic"$i" -netport irqS3 -net intNI_TX"$i >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -netport INT_NI_TX  -net intNI_TX"$i >> module.op.tcl
	echo "ihwaddnet -instancename intNI_TMR"$i >> module.op.tcl
	echo "ihwconnect -instancename plic"$i" -netport irqS4 -net intNI_TMR"$i >> module.op.tcl
	echo "ihwconnect -instancename ni"$i" -netport INT_NI_TMR  -net intNI_TMR"$i >> module.op.tcl
	echo "" >> module.op.tcl
done

# Creates the wire to connect the TEA with one router
echo "ihwaddpacketnet -instancename data_0_0_TEA" >> module.op.tcl
echo "ihwaddpacketnet -instancename ctrl_0_0_TEA" >> module.op.tcl

# Creates the wire to connect the REPOSITORY with one router
echo "ihwaddpacketnet -instancename data_0_0_REPOSITORY" >> module.op.tcl
echo "ihwaddpacketnet -instancename ctrl_0_0_REPOSITORY" >> module.op.tcl

# Creates all ports to make the connection between routers (data and control)
for i in $(seq 0 $(($Y-1)));
	do
	for j in $(seq 0 $(($X-1)));
	do	
		echo "ihwaddpacketnet -instancename data_"$i"_"$j"_L" >> module.op.tcl
		echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_L" >> module.op.tcl
		if [ $(($j%2)) = 0 ];
		then 
			if [ $(($i%2)) = 0 ];
			then
				#echo $i$j
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_E" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_W" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_N" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_S" >> module.op.tcl
				
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_E" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_W" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_N" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_S" >> module.op.tcl
			fi
		else
			if [ $(($i%2)) != 0 ];
			then
				#echo $i$j
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_E" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_W" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_N" >> module.op.tcl
				echo "ihwaddpacketnet -instancename data_"$i"_"$j"_S" >> module.op.tcl

				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_E" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_W" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_N" >> module.op.tcl
				echo "ihwaddpacketnet -instancename ctrl_"$i"_"$j"_S" >> module.op.tcl 
			fi
		fi
	done
done 
echo "" >> module.op.tcl

# Connects the TEA peripheral to one given PE
echo "ihwconnect -instancename router0 -packetnetport portDataWest -packetnet data_0_0_TEA" >> module.op.tcl
echo "ihwconnect -instancename router0 -packetnetport portControlWest -packetnet ctrl_0_0_TEA" >> module.op.tcl
echo "ihwconnect -instancename tea -packetnetport portData -packetnet data_0_0_TEA" >> module.op.tcl
echo "ihwconnect -instancename tea -packetnetport portControl -packetnet ctrl_0_0_TEA" >> module.op.tcl

# Connects the REPOSITORY peripheral to one given PE
echo "ihwconnect -instancename router0 -packetnetport portDataSouth -packetnet data_0_0_REPOSITORY" >> module.op.tcl
echo "ihwconnect -instancename router0 -packetnetport portControlSouth -packetnet ctrl_0_0_REPOSITORY" >> module.op.tcl
echo "ihwconnect -instancename repository -packetnetport portData -packetnet data_0_0_REPOSITORY" >> module.op.tcl
echo "ihwconnect -instancename repository -packetnetport portControl -packetnet ctrl_0_0_REPOSITORY" >> module.op.tcl

# Connects each router to its neighbor
cont=0;
bordaX=$(($X-1))
bordaY=$(($Y-1))
for i in $(seq 0 $bordaY);
	do
	for j in $(seq 0 $bordaX);
	do
		echo "ihwconnect -instancename router"$cont" -packetnetport portDataLocal -packetnet data_"$i"_"$j"_L" >> module.op.tcl
		echo "ihwconnect -instancename ni"$cont" -packetnetport dataPort -packetnet data_"$i"_"$j"_L" >> module.op.tcl
		
		echo "ihwconnect -instancename router"$cont" -packetnetport portControlLocal -packetnet ctrl_"$i"_"$j"_L" >> module.op.tcl
		echo "ihwconnect -instancename ni"$cont" -packetnetport controlPort -packetnet ctrl_"$i"_"$j"_L" >> module.op.tcl
		if [ $(($j%2)) = 0 ];
		then
			if [ $(($i%2)) = 0 ];
			then 

				if [ $j -lt $bordaX ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataEast -packetnet data_"$i"_"$j"_E" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+1))" -packetnetport portDataWest -packetnet data_"$i"_"$j"_E" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlEast -packetnet ctrl_"$i"_"$j"_E" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+1))" -packetnetport portControlWest -packetnet ctrl_"$i"_"$j"_E" >> module.op.tcl
				fi


				if [ $j -gt 0 ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataWest -packetnet data_"$i"_"$j"_W" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-1))" -packetnetport portDataEast -packetnet data_"$i"_"$j"_W" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlWest -packetnet ctrl_"$i"_"$j"_W" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-1))" -packetnetport portControlEast -packetnet ctrl_"$i"_"$j"_W" >> module.op.tcl
				fi


				if [ $i -lt $bordaY ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataNorth -packetnet data_"$i"_"$j"_N" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+$X))" -packetnetport portDataSouth -packetnet data_"$i"_"$j"_N" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlNorth -packetnet ctrl_"$i"_"$j"_N" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+$X))" -packetnetport portControlSouth -packetnet ctrl_"$i"_"$j"_N" >> module.op.tcl
				fi


				if [ $i -gt 0 ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataSouth -packetnet data_"$i"_"$j"_S" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-$X))" -packetnetport portDataNorth -packetnet data_"$i"_"$j"_S" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlSouth -packetnet ctrl_"$i"_"$j"_S" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-$X))" -packetnetport portControlNorth -packetnet ctrl_"$i"_"$j"_S" >> module.op.tcl
				fi

			fi
	
		else
			if [ $(($i%2)) != 0 ];
			then

				if [ $j -lt $bordaX ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataEast -packetnet data_"$i"_"$j"_E" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+1))" -packetnetport portDataWest -packetnet data_"$i"_"$j"_E" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlEast -packetnet ctrl_"$i"_"$j"_E" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+1))" -packetnetport portControlWest -packetnet ctrl_"$i"_"$j"_E" >> module.op.tcl
				fi


				if [ $j -gt 0 ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataWest -packetnet data_"$i"_"$j"_W" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-1))" -packetnetport portDataEast -packetnet data_"$i"_"$j"_W" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlWest -packetnet ctrl_"$i"_"$j"_W" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-1))" -packetnetport portControlEast -packetnet ctrl_"$i"_"$j"_W" >> module.op.tcl
				fi


				if [ $i -lt $bordaY ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataNorth -packetnet data_"$i"_"$j"_N" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+$X))" -packetnetport portDataSouth -packetnet data_"$i"_"$j"_N" >> module.op.tcl
					
					echo "ihwconnect -instancename router"$cont" -packetnetport portControlNorth -packetnet ctrl_"$i"_"$j"_N" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont+$X))" -packetnetport portControlSouth -packetnet ctrl_"$i"_"$j"_N" >> module.op.tcl
				fi


				if [ $i -gt 0 ];
				then
					echo "ihwconnect -instancename router"$cont" -packetnetport portDataSouth -packetnet data_"$i"_"$j"_S" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-$X))" -packetnetport portDataNorth -packetnet data_"$i"_"$j"_S" >> module.op.tcl

					echo "ihwconnect -instancename router"$cont" -packetnetport portControlSouth -packetnet ctrl_"$i"_"$j"_S" >> module.op.tcl
					echo "ihwconnect -instancename router"$(($cont-$X))" -packetnetport portControlNorth -packetnet ctrl_"$i"_"$j"_S" >> module.op.tcl
				fi
			fi
		fi		
	cont=$(($cont+1))
	done
done 
echo "" >> module.op.tcl

# Connects every interruption signal from each router to the associated processor
#for i in $(seq 0 $N);
#do
	#echo "ihwconnect -instancename ni"$i" -netport INT_NI_TX  -net intNI_TX"$i >> module.op.tcl
	#echo "ihwconnect -instancename ni"$i" -netport INT_NI_RX  -net intNI_RX"$i >> module.op.tcl
#done

echo "ihwaddperipheral -instancename iterator -modelfile peripheral/iterator/pse.pse" >> module.op.tcl #iterator

echo "" >> module.op.tcl
for i in $(seq 0 $N);
	do
	echo "ihwaddpacketnet -instancename iteration_"$i"" >> module.op.tcl
	echo "ihwconnect -instancename router"$i" -packetnetport iterationsPort -packetnet iteration_"$i"" >> module.op.tcl
	echo "ihwconnect -instancename iterator -packetnetport iterationPort"$i" -packetnet iteration_"$i"" >> module.op.tcl
done 
echo "" >> module.op.tcl



