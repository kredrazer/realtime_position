################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/decadriver/deca_device.c \
../Core/Lib/decadriver/deca_params_init.c 

OBJS += \
./Core/Lib/decadriver/deca_device.o \
./Core/Lib/decadriver/deca_params_init.o 

C_DEPS += \
./Core/Lib/decadriver/deca_device.d \
./Core/Lib/decadriver/deca_params_init.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/decadriver/%.o Core/Lib/decadriver/%.su Core/Lib/decadriver/%.cyclo: ../Core/Lib/decadriver/%.c Core/Lib/decadriver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/App" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/decadriver" -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-decadriver

clean-Core-2f-Lib-2f-decadriver:
	-$(RM) ./Core/Lib/decadriver/deca_device.cyclo ./Core/Lib/decadriver/deca_device.d ./Core/Lib/decadriver/deca_device.o ./Core/Lib/decadriver/deca_device.su ./Core/Lib/decadriver/deca_params_init.cyclo ./Core/Lib/decadriver/deca_params_init.d ./Core/Lib/decadriver/deca_params_init.o ./Core/Lib/decadriver/deca_params_init.su

.PHONY: clean-Core-2f-Lib-2f-decadriver

