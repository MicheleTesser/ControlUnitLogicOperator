################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/lib/rigen_fun_simulink22_ert_rtw/subdir.mk
-include src/lib/board_dbc/dbc/out_lib/cansbg/subdir.mk
-include src/lib/board_dbc/dbc/out_lib/can3/subdir.mk
-include src/lib/board_dbc/dbc/out_lib/can2/subdir.mk
-include src/lib/board_dbc/dbc/out_lib/can1/subdir.mk
-include src/lib/TV/subdir.mk
-include src/lib/DPS/lib/c_vector/subdir.mk
-include src/lib/DPS/subdir.mk
-include src/lib/Cmemory/subdir.mk
-include src/cores/core_utility/running_status/subdir.mk
-include src/cores/core_utility/rtd_assi_sound/subdir.mk
-include src/cores/core_utility/mission_reader/mission_locker/subdir.mk
-include src/cores/core_utility/mission_reader/subdir.mk
-include src/cores/core_utility/imu/subdir.mk
-include src/cores/core_utility/emergency_module/subdir.mk
-include src/cores/core_utility/driver_input_reader/subdir.mk
-include src/cores/core_utility/core_status/subdir.mk
-include src/cores/core_utility/as_node/subdir.mk
-include src/cores/core_utility/alive_blink/subdir.mk
-include src/cores/core_2/feature/DV/steering_wheel_alg/subdir.mk
-include src/cores/core_2/feature/DV/res/subdir.mk
-include src/cores/core_2/feature/DV/ebs/subdir.mk
-include src/cores/core_2/feature/DV/subdir.mk
-include src/cores/core_2/feature/subdir.mk
-include src/cores/core_2/subdir.mk
-include src/cores/core_1/feature/suspensions/subdir.mk
-include src/cores/core_1/feature/log/telemetry/json_builder/subdir.mk
-include src/cores/core_1/feature/log/telemetry/subdir.mk
-include src/cores/core_1/feature/log/sd/subdir.mk
-include src/cores/core_1/feature/log/external_log_variables/subdir.mk
-include src/cores/core_1/feature/log/subdir.mk
-include src/cores/core_1/feature/core_1_imu/subdir.mk
-include src/cores/core_1/feature/core_1_driver_input/subdir.mk
-include src/cores/core_1/feature/cooling/subdir.mk
-include src/cores/core_1/feature/batteries/bms/subdir.mk
-include src/cores/core_1/feature/batteries/subdir.mk
-include src/cores/core_1/feature/subdir.mk
-include src/cores/core_1/subdir.mk
-include src/cores/core_0/feature/math_saturated/subdir.mk
-include src/cores/core_0/feature/maps/subdir.mk
-include src/cores/core_0/feature/giei/giei_components/torque_vec_alg/subdir.mk
-include src/cores/core_0/feature/giei/giei_components/speed_alg/subdir.mk
-include src/cores/core_0/feature/giei/giei_components/regen_alg/subdir.mk
-include src/cores/core_0/feature/giei/giei_components/power_control/subdir.mk
-include src/cores/core_0/feature/giei/giei_components/hv/subdir.mk
-include src/cores/core_0/feature/giei/subdir.mk
-include src/cores/core_0/feature/engines/amk/subdir.mk
-include src/cores/core_0/feature/driver_input/subdir.mk
-include src/cores/core_0/feature/subdir.mk
-include src/cores/core_0/subdir.mk
-include driver_imp/timer/subdir.mk
-include driver_imp/serial/subdir.mk
-include driver_imp/gpio/subdir.mk
-include driver_imp/ethernet/subdir.mk
-include driver_imp/can/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/_PinMap/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/_Lib/DataHandling/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/_Impl/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Stm/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Src/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Scu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Port/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Pms/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Mtu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Gtm/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Trap/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Cpu/Irq/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Can/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Can/Can/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Asclin/Std/subdir.mk
-include Libraries/iLLD/TC37A/Tricore/Asclin/Asc/subdir.mk
-include Libraries/Service/CpuGeneric/SysSe/Bsp/subdir.mk
-include Libraries/Service/CpuGeneric/StdIf/subdir.mk
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


BUILD_ARTIFACT_NAME := aurix_mcu_src
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
aurix_mcu_src.hex \

SECONDARY_LIST += \
aurix_mcu_src.lst \

SECONDARY_SIZE += \
aurix_mcu_src.siz \


# All Target
all: main-build

# Main-build Target
main-build: aurix_mcu_src.elf secondary-outputs

# Tool invocations
aurix_mcu_src.elf: $(OBJS) ../Lcf_Gnuc_Tricore_Tc.lsl makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: AURIX GCC Linker'
	tricore-elf-gcc -T"../Lcf_Gnuc_Tricore_Tc.lsl" -nocrt0 -Wl,--gc-sections -mtc162 -o "aurix_mcu_src.elf" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

aurix_mcu_src.hex: aurix_mcu_src.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Object Copy'
	tricore-elf-objcopy -O ihex "aurix_mcu_src.elf" "aurix_mcu_src.hex"
	@echo 'Finished building: $@'
	@echo ' '

aurix_mcu_src.lst: aurix_mcu_src.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Create Listing'
	tricore-elf-objdump -h -S "aurix_mcu_src.elf" > "aurix_mcu_src.lst"
	@echo 'Finished building: $@'
	@echo ' '

aurix_mcu_src.siz: aurix_mcu_src.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: AURIX Print Size'
	tricore-elf-size  --format=berkeley "aurix_mcu_src.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) aurix_mcu_src.elf aurix_mcu_src.hex aurix_mcu_src.lst aurix_mcu_src.siz
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
