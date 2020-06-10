#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A7__ -D__ARM32_NEON__

#export OUTPUT_I         := 1
#export OUTPUT_S         := 1
#export OUTPUT_OBJDUMP   := 1
#export OUTPUT_NM        := 1

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

xend:
	@echo Generate kernel7.img For Raspberry Pi 2
	@$(CP) $(X_NAME).bin $(X_OUT)/kernel7.img
