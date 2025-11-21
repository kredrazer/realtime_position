################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_DEVICE/Target/usbd_conf.c 

OBJS += \
./USB_DEVICE/Target/usbd_conf.o 

C_DEPS += \
./USB_DEVICE/Target/usbd_conf.d 


# Each subdirectory must supply rules for building sources it contributes
USB_DEVICE/Target/%.o USB_DEVICE/Target/%.su USB_DEVICE/Target/%.cyclo: ../USB_DEVICE/Target/%.c USB_DEVICE/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Core/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/Target" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/decadriver" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/compiler" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-USB_DEVICE-2f-Target

clean-USB_DEVICE-2f-Target:
	-$(RM) ./USB_DEVICE/Target/usbd_conf.cyclo ./USB_DEVICE/Target/usbd_conf.d ./USB_DEVICE/Target/usbd_conf.o ./USB_DEVICE/Target/usbd_conf.su

.PHONY: clean-USB_DEVICE-2f-Target

