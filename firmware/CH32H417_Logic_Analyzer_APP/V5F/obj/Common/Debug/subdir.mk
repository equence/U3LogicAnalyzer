################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug/debug.c 

C_DEPS += \
./Common/Debug/debug.d 

OBJS += \
./Common/Debug/debug.o 

DIR_OBJS += \
./Common/Debug/*.o \

DIR_DEPS += \
./Common/Debug/*.d \

DIR_EXPANDS += \
./Common/Debug/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Common/Debug/debug.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug/debug.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

