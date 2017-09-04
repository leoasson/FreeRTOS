################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/fifo.c \
../inc/serial.c 

OBJS += \
./inc/fifo.o \
./inc/serial.o 

C_DEPS += \
./inc/fifo.d \
./inc/serial.d 


# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo\trace\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\CMSISv1p30_LPC17xx\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\FreeRTOS_Library\include" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\FreeRTOS_Library\portable" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


