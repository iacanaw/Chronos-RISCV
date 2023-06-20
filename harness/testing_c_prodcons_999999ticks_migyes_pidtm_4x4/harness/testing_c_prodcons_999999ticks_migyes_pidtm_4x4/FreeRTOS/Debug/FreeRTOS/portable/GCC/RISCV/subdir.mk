################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/portable/GCC/RISCV/port.c 

S_UPPER_SRCS += \
../FreeRTOS/portable/GCC/RISCV/portasm.S 

OBJS += \
./FreeRTOS/portable/GCC/RISCV/port.o \
./FreeRTOS/portable/GCC/RISCV/portasm.o 

S_UPPER_DEPS += \
./FreeRTOS/portable/GCC/RISCV/portasm.d 

C_DEPS += \
./FreeRTOS/portable/GCC/RISCV/port.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/GCC/RISCV/%.o: ../FreeRTOS/portable/GCC/RISCV/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DMSCC_STDIO_THRU_CORE_UART_APB -I"../FreeRTOS" -I"../FreeRTOS/include" -I"../FreeRTOS/portable" -I"../FreeRTOS/portable/GCC" -I"../FreeRTOS/portable/MemMang" -I"../FreeRTOS/portable/GCC/RISCV" -I".." -I"../drivers/CoreGPIO" -I"../drivers/Core16550" -I"../drivers/CoreUARTapb" -I"../drivers/CoreTimer" -I"../hal" -I"../riscv_hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FreeRTOS/portable/GCC/RISCV/%.o: ../FreeRTOS/portable/GCC/RISCV/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


