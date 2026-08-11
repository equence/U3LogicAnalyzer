################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/flash.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/hardware.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/iap.c 

C_DEPS += \
./Common/flash.d \
./Common/hardware.d \
./Common/iap.d 

OBJS += \
./Common/flash.o \
./Common/hardware.o \
./Common/iap.o 

DIR_OBJS += \
./Common/*.o \

DIR_DEPS += \
./Common/*.d \

DIR_EXPANDS += \
./Common/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Common/flash.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/flash.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/hardware.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/hardware.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/iap.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/iap.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

