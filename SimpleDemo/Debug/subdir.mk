################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Parte_2.c \
../Parte_3.c \
../cr_startup_lpc17.c 

OBJS += \
./Parte_2.o \
./Parte_3.o \
./cr_startup_lpc17.o 

C_DEPS += \
./Parte_2.d \
./Parte_3.d \
./cr_startup_lpc17.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\SimpleDemo\trace\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\CMSISv1p30_LPC17xx\inc" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\FreeRTOS_Library\include" -I"C:\Users\leo\Documents\LPCXpresso_8.2.2_650\workspace\FreeRTOS_Library\portable" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


