################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include riscv_hal/subdir.mk
-include hal/subdir.mk
-include drivers/CoreUARTapb/subdir.mk
-include drivers/CoreTimer/subdir.mk
-include drivers/CoreI2C/subdir.mk
-include drivers/CoreGPIO/subdir.mk
-include drivers/Core16550/subdir.mk
-include FreeRTOS/portable/MemMang/subdir.mk
-include FreeRTOS/portable/GCC/RISCV/subdir.mk
-include FreeRTOS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
miv-rv32im-freertos-port-test.hex \

SECONDARY_LIST += \
miv-rv32im-freertos-port-test.lst \

SECONDARY_SIZE += \
miv-rv32im-freertos-port-test.siz \


# All Target
all: miv-rv32im-freertos-port-test.elf secondary-outputs

# Tool invocations
miv-rv32im-freertos-port-test.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	$(RISCV_PREFIX)gcc -march=rv32im -mabi=ilp32 -msmall-data-limit=8 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "../riscv_hal/microsemi-riscv-ram.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"miv-rv32im-freertos-port-test.map" --specs=nano.specs -o "miv-rv32im-freertos-port-test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

miv-rv32im-freertos-port-test.hex: miv-rv32im-freertos-port-test.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	$(RISCV_PREFIX)objcopy -O ihex "miv-rv32im-freertos-port-test.elf"  "miv-rv32im-freertos-port-test.hex"
	@echo 'Finished building: $@'
	@echo ' '

miv-rv32im-freertos-port-test.lst: miv-rv32im-freertos-port-test.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	$(RISCV_PREFIX)objdump --source --all-headers --demangle --line-numbers --wide "miv-rv32im-freertos-port-test.elf" > "miv-rv32im-freertos-port-test.lst"
	@echo 'Finished building: $@'
	@echo ' '

miv-rv32im-freertos-port-test.siz: miv-rv32im-freertos-port-test.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	$(RISCV_PREFIX)size --format=berkeley "miv-rv32im-freertos-port-test.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) miv-rv32im-freertos-port-test.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
