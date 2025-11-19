################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/App/app_init.c \
../Core/App/uwb.c 

OBJS += \
./Core/App/app_init.o \
./Core/App/uwb.o 

C_DEPS += \
./Core/App/app_init.d \
./Core/App/uwb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/App/%.o Core/App/%.su Core/App/%.cyclo: ../Core/App/%.c Core/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/App" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/decadriver" -I"E:/project/realtime_positioning/single_twr/single_twr2/Core/Lib/platform" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-App

clean-Core-2f-App:
	-$(RM) ./Core/App/app_init.cyclo ./Core/App/app_init.d ./Core/App/app_init.o ./Core/App/app_init.su ./Core/App/uwb.cyclo ./Core/App/uwb.d ./Core/App/uwb.o ./Core/App/uwb.su

.PHONY: clean-Core-2f-App

