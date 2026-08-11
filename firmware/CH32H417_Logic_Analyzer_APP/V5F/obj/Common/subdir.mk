################################################################################
# MRS Version: 2.5.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_logic.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_uhsif.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbhs_device.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbss_device.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbss_it.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/hardware.c \
/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/usb_desc.c 

C_DEPS += \
./Common/ch32h417_logic.d \
./Common/ch32h417_uhsif.d \
./Common/ch32h417_usbhs_device.d \
./Common/ch32h417_usbss_device.d \
./Common/ch32h417_usbss_it.d \
./Common/hardware.d \
./Common/usb_desc.d 

OBJS += \
./Common/ch32h417_logic.o \
./Common/ch32h417_uhsif.o \
./Common/ch32h417_usbhs_device.o \
./Common/ch32h417_usbss_device.o \
./Common/ch32h417_usbss_it.o \
./Common/hardware.o \
./Common/usb_desc.o 

DIR_OBJS += \
./Common/*.o \

DIR_DEPS += \
./Common/*.d \

DIR_EXPANDS += \
./Common/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
Common/ch32h417_logic.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_logic.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/ch32h417_uhsif.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_uhsif.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/ch32h417_usbhs_device.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbhs_device.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/ch32h417_usbss_device.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbss_device.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/ch32h417_usbss_it.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/ch32h417_usbss_it.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/hardware.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/hardware.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
Common/usb_desc.o: /Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/usb_desc.c
	@	riscv-wch-elf-gcc -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DCore_V5F -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common/Debug" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Core" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/V5F/User" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/SRC/Peripheral/inc" -I"/Users/wangmian/Downloads/CH32H417_Logic_Analyzer_1v0/CH32H417_Logic_Analyzer_APP/Common" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

