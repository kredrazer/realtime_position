################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/App/app_uart.c \
../Drivers/App/app_uart_handler.c \
../Drivers/App/app_uwb.c 

OBJS += \
./Drivers/App/app_uart.o \
./Drivers/App/app_uart_handler.o \
./Drivers/App/app_uwb.o 

C_DEPS += \
./Drivers/App/app_uart.d \
./Drivers/App/app_uart_handler.d \
./Drivers/App/app_uwb.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/App/%.o Drivers/App/%.su Drivers/App/%.cyclo: ../Drivers/App/%.c Drivers/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/Lib/compiler -I../Drivers/Lib/decadriver -I../Drivers/Lib/platform -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-App

clean-Drivers-2f-App:
	-$(RM) ./Drivers/App/app_uart.cyclo ./Drivers/App/app_uart.d ./Drivers/App/app_uart.o ./Drivers/App/app_uart.su ./Drivers/App/app_uart_handler.cyclo ./Drivers/App/app_uart_handler.d ./Drivers/App/app_uart_handler.o ./Drivers/App/app_uart_handler.su ./Drivers/App/app_uwb.cyclo ./Drivers/App/app_uwb.d ./Drivers/App/app_uwb.o ./Drivers/App/app_uwb.su

.PHONY: clean-Drivers-2f-App

