INC-y+=./include/arch/arm
ARCH:=arm
CROSS_COMPILE:=arm-none-eabi-
CFLAG+= -mcpu=$(SUB_ARCH) -target arm-none-eabi
