################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period" -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period/Debug" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/DSP/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source" -DARM_MATH_CM0 -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1928323289: ../timx_timer_mode_capture_duty_and_period.syscfg
	@echo 'SysConfig - building file: "$<"'
	"D:/ti/ccs2051/sysconfig_1.26.2/sysconfig_cli.bat" -s "D:/ti/ccs2051/mspm0_sdk_2_10_00_04/.metadata/product.json" --script "D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period/timx_timer_mode_capture_duty_and_period.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1928323289 ../timx_timer_mode_capture_duty_and_period.syscfg
device.opt: build-1928323289
device.cmd.genlibs: build-1928323289
ti_msp_dl_config.c: build-1928323289
ti_msp_dl_config.h: build-1928323289
Event.dot: build-1928323289

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period" -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period/Debug" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/DSP/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source" -DARM_MATH_CM0 -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g351x_ticlang.o: D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g351x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/ti/ccs2051/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period" -I"D:/My_Git/TI-Cup-Training-1/MSPM0G3519/timx_timer_mode_capture_duty_and_period/Debug" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/DSP/Include" -I"D:/ti/ccs2051/mspm0_sdk_2_10_00_04/source" -DARM_MATH_CM0 -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


