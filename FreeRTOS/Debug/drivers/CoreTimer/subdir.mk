################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/CoreTimer/core_timer.c 

OBJS += \
./drivers/CoreTimer/core_timer.o 

C_DEPS += \
./drivers/CoreTimer/core_timer.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/CoreTimer/%.o: ../drivers/CoreTimer/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DMSCC_STDIO_THRU_CORE_UART_APB -I"../FreeRTOS" -I"../FreeRTOS/include" -I"../FreeRTOS/portable" -I"../FreeRTOS/portable/GCC" -I"../FreeRTOS/portable/MemMang" -I"../FreeRTOS/portable/GCC/RISCV" -I".." -I"../drivers/CoreGPIO" -I"../drivers/Core16550" -I"../drivers/CoreUARTapb" -I"../drivers/CoreTimer" -I"../hal" -I"../riscv_hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


