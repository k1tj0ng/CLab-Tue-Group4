################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/intergration.c \
../Src/main.c \
../Src/serial.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/uart_interrupt.c 

OBJS += \
./Src/intergration.o \
./Src/main.o \
./Src/serial.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/uart_interrupt.o 

C_DEPS += \
./Src/intergration.d \
./Src/main.d \
./Src/serial.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/uart_interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F303VCTx -DSTM32 -DSTM32F3 -DSTM32F3DISCOVERY -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/intergration.cyclo ./Src/intergration.d ./Src/intergration.o ./Src/intergration.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/serial.cyclo ./Src/serial.d ./Src/serial.o ./Src/serial.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/uart_interrupt.cyclo ./Src/uart_interrupt.d ./Src/uart_interrupt.o ./Src/uart_interrupt.su

.PHONY: clean-Src

