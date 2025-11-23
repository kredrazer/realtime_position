################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Lib/decadriver/deca_device.c \
../Drivers/Lib/decadriver/deca_params_init.c 

OBJS += \
./Drivers/Lib/decadriver/deca_device.o \
./Drivers/Lib/decadriver/deca_params_init.o 

C_DEPS += \
./Drivers/Lib/decadriver/deca_device.d \
./Drivers/Lib/decadriver/deca_params_init.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Lib/decadriver/%.o Drivers/Lib/decadriver/%.su Drivers/Lib/decadriver/%.cyclo: ../Drivers/Lib/decadriver/%.c Drivers/Lib/decadriver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Core/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/Target" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/decadriver" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/compiler" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Lib-2f-decadriver

clean-Drivers-2f-Lib-2f-decadriver:
	-$(RM) ./Drivers/Lib/decadriver/deca_device.cyclo ./Drivers/Lib/decadriver/deca_device.d ./Drivers/Lib/decadriver/deca_device.o ./Drivers/Lib/decadriver/deca_device.su ./Drivers/Lib/decadriver/deca_params_init.cyclo ./Drivers/Lib/decadriver/deca_params_init.d ./Drivers/Lib/decadriver/deca_params_init.o ./Drivers/Lib/decadriver/deca_params_init.su

.PHONY: clean-Drivers-2f-Lib-2f-decadriver

