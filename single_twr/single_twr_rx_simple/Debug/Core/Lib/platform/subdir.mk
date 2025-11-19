################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/platform/deca_mutex.c \
../Core/Lib/platform/deca_range_tables.c \
../Core/Lib/platform/deca_sleep.c \
../Core/Lib/platform/deca_spi.c \
../Core/Lib/platform/port.c 

OBJS += \
./Core/Lib/platform/deca_mutex.o \
./Core/Lib/platform/deca_range_tables.o \
./Core/Lib/platform/deca_sleep.o \
./Core/Lib/platform/deca_spi.o \
./Core/Lib/platform/port.o 

C_DEPS += \
./Core/Lib/platform/deca_mutex.d \
./Core/Lib/platform/deca_range_tables.d \
./Core/Lib/platform/deca_sleep.d \
./Core/Lib/platform/deca_spi.d \
./Core/Lib/platform/port.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/platform/%.o Core/Lib/platform/%.su Core/Lib/platform/%.cyclo: ../Core/Lib/platform/%.c Core/Lib/platform/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/App" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/decadriver" -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-platform

clean-Core-2f-Lib-2f-platform:
	-$(RM) ./Core/Lib/platform/deca_mutex.cyclo ./Core/Lib/platform/deca_mutex.d ./Core/Lib/platform/deca_mutex.o ./Core/Lib/platform/deca_mutex.su ./Core/Lib/platform/deca_range_tables.cyclo ./Core/Lib/platform/deca_range_tables.d ./Core/Lib/platform/deca_range_tables.o ./Core/Lib/platform/deca_range_tables.su ./Core/Lib/platform/deca_sleep.cyclo ./Core/Lib/platform/deca_sleep.d ./Core/Lib/platform/deca_sleep.o ./Core/Lib/platform/deca_sleep.su ./Core/Lib/platform/deca_spi.cyclo ./Core/Lib/platform/deca_spi.d ./Core/Lib/platform/deca_spi.o ./Core/Lib/platform/deca_spi.su ./Core/Lib/platform/port.cyclo ./Core/Lib/platform/port.d ./Core/Lib/platform/port.o ./Core/Lib/platform/port.su

.PHONY: clean-Core-2f-Lib-2f-platform

