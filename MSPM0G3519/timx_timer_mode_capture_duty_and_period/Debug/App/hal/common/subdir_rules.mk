################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
App/hal/common/%.o: ../App/hal/common/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/My_Git/MSPM0G3519/MSPM0G3519/timx_timer_mode_capture_duty_and_period" -I"D:/My_Git/MSPM0G3519/MSPM0G3519/timx_timer_mode_capture_duty_and_period/Debug" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/DSP/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source" -DARM_MATH_CM0 -gdwarf-3 -MMD -MP -MF"App/hal/common/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


