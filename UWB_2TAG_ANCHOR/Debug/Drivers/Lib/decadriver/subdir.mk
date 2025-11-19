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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/Lib/compiler -I../Drivers/Lib/decadriver -I../Drivers/Lib/platform -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Lib-2f-decadriver

clean-Drivers-2f-Lib-2f-decadriver:
	-$(RM) ./Drivers/Lib/decadriver/deca_device.cyclo ./Drivers/Lib/decadriver/deca_device.d ./Drivers/Lib/decadriver/deca_device.o ./Drivers/Lib/decadriver/deca_device.su ./Drivers/Lib/decadriver/deca_params_init.cyclo ./Drivers/Lib/decadriver/deca_params_init.d ./Drivers/Lib/decadriver/deca_params_init.o ./Drivers/Lib/decadriver/deca_params_init.su

.PHONY: clean-Drivers-2f-Lib-2f-decadriver

