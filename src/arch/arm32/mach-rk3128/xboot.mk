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
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKROCK		:= arch/$(ARCH)/$(MACH)/tools/linux/mkrock
endif
ifeq ($(strip $(HOSTOS)), windows)
MKROCK		:= arch/$(ARCH)/$(MACH)/tools/windows/mkrock
endif
INIFILE		:= arch/$(ARCH)/$(MACH)/tools/images/rk3128.ini

xend:
	@echo Packing rockchip binrary for irom booting
	@$(MKROCK) $(INIFILE)
