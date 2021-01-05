#
# Machine makefile
#

DEFINES     += -D__ARM32_ARCH__=7 -D__CORTEX_A9__ -D__ARM32_NEON__

#export OUTPUT_I         := 1
#export OUTPUT_S         := 1
#export OUTPUT_OBJDUMP   := 1
#export OUTPUT_NM        := 1

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a9 -mfpu=vfpv3-d16 -mfloat-abi=hard -marm -mno-thumb-interwork -mno-unaligned-access

LIBDIRS     :=
LIBS        :=
INCDIRS     :=
SRCDIRS     :=
