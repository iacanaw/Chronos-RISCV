################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hal/hal_irq.c 

S_UPPER_SRCS += \
../hal/hw_reg_access.S 

OBJS += \
./hal/hal_irq.o \
./hal/hw_reg_access.o 

S_UPPER_DEPS += \
./hal/hw_reg_access.d 

C_DEPS += \
./hal/hal_irq.d 


# Each subdirectory must supply rules for building sources it contributes
hal/%.o: ../hal/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DMSCC_STDIO_THRU_CORE_UART_APB -I"../FreeRTOS" -I"../FreeRTOS/include" -I"../FreeRTOS/portable" -I"../FreeRTOS/portable/GCC" -I"../FreeRTOS/portable/MemMang" -I"../FreeRTOS/portable/GCC/RISCV" -I".." -I"../drivers/CoreGPIO" -I"../drivers/Core16550" -I"../drivers/CoreUARTapb" -I"../drivers/CoreTimer" -I"../hal" -I"../riscv_hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/%.o: ../hal/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


