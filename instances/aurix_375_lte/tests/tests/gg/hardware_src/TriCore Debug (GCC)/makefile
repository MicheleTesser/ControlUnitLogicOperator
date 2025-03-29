################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Libraries/iLLD/TC37A/Tricore/_PinMap/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/_Impl/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Src/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Scu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Port/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Pms/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Mtu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Gtm/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Trap/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Irq/subdir.mk
-include Libraries/Infra/Ssw/TC37A/Tricore/subdir.mk
-include Libraries/Infra/Platform/Tricore/Compilers/subdir.mk
-include Configurations/Debug/subdir.mk
-include Configurations/subdir.mk
-include subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := basic_aurix_template
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
basic_aurix_template.hex \

SECONDARY_LIST += \
basic_aurix_template.lst \

SECONDARY_SIZE += \
basic_aurix_template.siz \


# All Target
all: main-build

# Main-build Target
main-build: basic_aurix_template.elf secondary-outputs

# Tool invocations
basic_aurix_template.elf: $(OBJS) ../Lcf_Gnuc_Tricore_Tc.lsl makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: AURIX GCC Linker'
	tricore-elf-gcc -T"../Lcf_Gnuc_Tricore_Tc.lsl" -nocrt0 -Wl,--gc-sections -mtc162 -o "basic_aurix_template.elf" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

basic_aurix_template.hex: basic_aurix_template.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Object Copy'
	tricore-elf-objcopy -O ihex "basic_aurix_template.elf" "basic_aurix_template.hex"
	@echo 'Finished building: $@'
	@echo ' '

basic_aurix_template.lst: basic_aurix_template.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Create Listing'
	tricore-elf-objdump -h -S "basic_aurix_template.elf" > "basic_aurix_template.lst"
	@echo 'Finished building: $@'
	@echo ' '

basic_aurix_template.siz: basic_aurix_template.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Print Size'
	tricore-elf-size  --format=berkeley "basic_aurix_template.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) basic_aurix_template.elf basic_aurix_template.hex basic_aurix_template.lst basic_aurix_template.siz
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
