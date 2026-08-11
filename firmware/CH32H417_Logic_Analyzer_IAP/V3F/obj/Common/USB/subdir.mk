################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/ch32h417_usbfs_device.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/ch32h417_usbhs_device.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/usb_desc.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/usb_inf.c 

C_DEPS += \
./Common/USB/ch32h417_usbfs_device.d \
./Common/USB/ch32h417_usbhs_device.d \
./Common/USB/usb_desc.d \
./Common/USB/usb_inf.d 

OBJS += \
./Common/USB/ch32h417_usbfs_device.o \
./Common/USB/ch32h417_usbhs_device.o \
./Common/USB/usb_desc.o \
./Common/USB/usb_inf.o 

DIR_OBJS += \
./Common/USB/*.o \

DIR_DEPS += \
./Common/USB/*.d \

DIR_EXPANDS += \
./Common/USB/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Common/USB/ch32h417_usbfs_device.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/ch32h417_usbfs_device.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V3F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V3F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/USB/ch32h417_usbhs_device.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/ch32h417_usbhs_device.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V3F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V3F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/USB/usb_desc.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/usb_desc.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V3F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V3F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/USB/usb_inf.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB/usb_inf.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V3F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/V3F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_IAP/Common/USB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

