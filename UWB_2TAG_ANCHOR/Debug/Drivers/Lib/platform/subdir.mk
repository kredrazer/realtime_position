################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Lib/platform/deca_mutex.c \
../Drivers/Lib/platform/deca_range_tables.c \
../Drivers/Lib/platform/deca_sleep.c \
../Drivers/Lib/platform/deca_spi.c \
../Drivers/Lib/platform/port.c 

OBJS += \
./Drivers/Lib/platform/deca_mutex.o \
./Drivers/Lib/platform/deca_range_tables.o \
./Drivers/Lib/platform/deca_sleep.o \
./Drivers/Lib/platform/deca_spi.o \
./Drivers/Lib/platform/port.o 

C_DEPS += \
./Drivers/Lib/platform/deca_mutex.d \
./Drivers/Lib/platform/deca_range_tables.d \
./Drivers/Lib/platform/deca_sleep.d \
./Drivers/Lib/platform/deca_spi.d \
./Drivers/Lib/platform/port.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Lib/platform/%.o Drivers/Lib/platform/%.su Drivers/Lib/platform/%.cyclo: ../Drivers/Lib/platform/%.c Drivers/Lib/platform/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/Lib/compiler -I../Drivers/Lib/decadriver -I../Drivers/Lib/platform -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Lib-2f-platform

clean-Drivers-2f-Lib-2f-platform:
	-$(RM) ./Drivers/Lib/platform/deca_mutex.cyclo ./Drivers/Lib/platform/deca_mutex.d ./Drivers/Lib/platform/deca_mutex.o ./Drivers/Lib/platform/deca_mutex.su ./Drivers/Lib/platform/deca_range_tables.cyclo ./Drivers/Lib/platform/deca_range_tables.d ./Drivers/Lib/platform/deca_range_tables.o ./Drivers/Lib/platform/deca_range_tables.su ./Drivers/Lib/platform/deca_sleep.cyclo ./Drivers/Lib/platform/deca_sleep.d ./Drivers/Lib/platform/deca_sleep.o ./Drivers/Lib/platform/deca_sleep.su ./Drivers/Lib/platform/deca_spi.cyclo ./Drivers/Lib/platform/deca_spi.d ./Drivers/Lib/platform/deca_spi.o ./Drivers/Lib/platform/deca_spi.su ./Drivers/Lib/platform/port.cyclo ./Drivers/Lib/platform/port.d ./Drivers/Lib/platform/port.o ./Drivers/Lib/platform/port.su

.PHONY: clean-Drivers-2f-Lib-2f-platform

