################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32h417_it.c \
../User/main.c \
../User/system_ch32h417.c 

C_DEPS += \
./User/ch32h417_it.d \
./User/main.d \
./User/system_ch32h417.d 

OBJS += \
./User/ch32h417_it.o \
./User/main.o \
./User/system_ch32h417.o 

DIR_OBJS += \
./User/*.o \

DIR_DEPS += \
./User/*.d \

DIR_EXPANDS += \
./User/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V3F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V3F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

