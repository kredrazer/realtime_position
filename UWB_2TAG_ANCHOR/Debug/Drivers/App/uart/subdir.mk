################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/App/uart/app_uart.c \
../Drivers/App/uart/app_uart_handler.c 

OBJS += \
./Drivers/App/uart/app_uart.o \
./Drivers/App/uart/app_uart_handler.o 

C_DEPS += \
./Drivers/App/uart/app_uart.d \
./Drivers/App/uart/app_uart_handler.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/App/uart/%.o Drivers/App/uart/%.su Drivers/App/uart/%.cyclo: ../Drivers/App/uart/%.c Drivers/App/uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/Lib/compiler -I../Drivers/Lib/decadriver -I../Drivers/Lib/platform -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/App/uart -I../Drivers/App/uwb -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-App-2f-uart

clean-Drivers-2f-App-2f-uart:
	-$(RM) ./Drivers/App/uart/app_uart.cyclo ./Drivers/App/uart/app_uart.d ./Drivers/App/uart/app_uart.o ./Drivers/App/uart/app_uart.su ./Drivers/App/uart/app_uart_handler.cyclo ./Drivers/App/uart/app_uart_handler.d ./Drivers/App/uart/app_uart_handler.o ./Drivers/App/uart/app_uart_handler.su

.PHONY: clean-Drivers-2f-App-2f-uart

