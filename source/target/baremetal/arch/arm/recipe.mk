ARCH:=arm
CROSS_COMPILE:=arm-none-eabi-
CFLAG+= -mtune=$(SUB_ARCH) 

