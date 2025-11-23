################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c 

OBJS += \
./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.o Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.su Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.cyclo: ../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.c Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Core/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device/ST" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Device" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/CMSIS/Include" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Inc" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/STM32F4xx_HAL_Driver/Src" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/USB_DEVICE/Target" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/App" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/decadriver" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/compiler" -I"E:/project/realtime_positioning/UWB_2TAG_ANCHOR/Drivers/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Class-2f-CDC-2f-Src

clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Class-2f-CDC-2f-Src:
	-$(RM) ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.cyclo ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.d ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.o ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.su

.PHONY: clean-Middlewares-2f-ST-2f-STM32_USB_Device_Library-2f-Class-2f-CDC-2f-Src

