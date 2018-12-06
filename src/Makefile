#
# Makefile for xboot
#

#
# You must pass CROSS_COMPILE and PLATFORM variable.
#
CROSS_COMPILE	?= arm-linux-gnueabihf-
PLATFORM		?= arm32-realview
#PLATFORM		?= arm32-rk3288

#
# Get platform information about ARCH and MACH from PLATFORM variable.
#
# 将PLATFORM中-替换为空格,然后判断单词数是否为2
ifeq ($(words $(subst -, , $(PLATFORM))), 2)
# ARCH为PLATFORM的-前一个单词，即arm32,所以ARCH=arm32
ARCH		:= $(word 1, $(subst -, , $(PLATFORM)))
# MACH为PLATFORM的-后一个单词为后缀,即realview,所以MACH=mach-realview
MACH		:= mach-$(word 2, $(subst -, , $(PLATFORM)))
else
ARCH		:= x64
MACH		:= mach-sandbox
endif

#
# System environment variable.
#
# 搜索系统名称是否为linux
ifneq (,$(findstring Linux, $(shell uname -a)))
HOSTOS		:= linux
endif
ifneq (,$(findstring windows, $(shell uname -a)))
HOSTOS		:= windows
endif
ifneq (,$(findstring Darwin, $(shell uname -a)))
HOSTOS		:= macos
endif

#
# Load default variables.
#
ASFLAGS		:= -g -ggdb -Wall -O0
CFLAGS		:= -g -ggdb -Wall -O0
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

#
# Override default variables.
#
# 导入xboot配置参数覆盖默认值
sinclude arch/$(ARCH)/$(MACH)/xboot.mk

#
# Modify default variables.
#
# 追加宏定义
ifeq ($(strip $(ARCH)), arm32)
DEFINES		+=	-D__ARM32__
endif
ifeq ($(strip $(ARCH)), arm64)
DEFINES		+=	-D__ARM64__
endif
ifeq ($(strip $(ARCH)), x64)
DEFINES		+=	-D__X64__
endif

# 追加ASFLAGS参数
ASFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
# 追加CFLAGS参数
CFLAGS		+=	-ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
# 追加LDFLAGS参数
LDFLAGS		+=	-Wl,-gc-sections

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
# 头文件路径
INCDIRS		+=	include										\
				arch/$(ARCH)/include						\
				arch/$(ARCH)/$(MACH)/include

# 源文件路径
SRCDIRS		+=	arch/$(ARCH)/lib/cpu						\
				arch/$(ARCH)/lib							\
				arch/$(ARCH)/$(MACH)						\
				arch/$(ARCH)/$(MACH)/command				\
				arch/$(ARCH)/$(MACH)/driver					\
				lib/libc									\
				lib/libc/charset							\
				lib/libc/crypto								\
				lib/libc/ctype								\
				lib/libc/environ							\
				lib/libc/errno								\
				lib/libc/exit								\
				lib/libc/filter								\
				lib/libc/locale								\
				lib/libc/malloc								\
				lib/libc/path								\
				lib/libc/stdio								\
				lib/libc/stdlib								\
				lib/libc/string								\
				lib/libc/time								\
				lib/libm/$(ARCH)							\
				lib/libm									\
				lib/libx									\
				init										\
				kernel										\
				kernel/core									\
				kernel/shell								\
				kernel/command								\
				kernel/time									\
				kernel/fs									\
				kernel/fs/vfs								\
				kernel/fs/ramfs								\
				kernel/fs/sysfs								\
				kernel/fs/arfs								\
				kernel/fs/tarfs								\
				kernel/fs/cpiofs							\
				kernel/fs/fatfs								\
				kernel/xfs									\
				driver/adc									\
				driver/audio								\
				driver/battery								\
				driver/block								\
				driver/block/partition						\
				driver/block/romdisk						\
				driver/buzzer								\
				driver/clk									\
				driver/clockevent							\
				driver/clocksource							\
				driver/compass								\
				driver/console								\
				driver/dac									\
				driver/dma									\
				driver/framebuffer							\
				driver/gmeter								\
				driver/gpio									\
				driver/gyroscope							\
				driver/hygrometer							\
				driver/i2c									\
				driver/input								\
				driver/input/rc								\
				driver/interrupt							\
				driver/laserscan							\
				driver/led									\
				driver/light								\
				driver/motor								\
				driver/nvmem								\
				driver/pressure								\
				driver/proximity							\
				driver/pwm									\
				driver/regulator							\
				driver/reset								\
				driver/rng									\
				driver/rtc									\
				driver/sd									\
				driver/servo								\
				driver/spi									\
				driver/stepper								\
				driver/thermometer							\
				driver/uart									\
				driver/vibrator								\
				driver/watchdog								\
				framework									\
				framework/base64							\
				framework/display							\
				framework/event								\
				framework/hardware							\
				framework/lang								\
				framework/stopwatch

#
# Add external library
#
INCDIRS		+= 	external/libfdt-1.4.2
SRCDIRS		+=	external/libfdt-1.4.2

INCDIRS		+= 	external/lz4-1.8.2
SRCDIRS		+=	external/lz4-1.8.2

INCDIRS		+= 	external/zlib-1.2.11
SRCDIRS		+=	external/zlib-1.2.11

INCDIRS		+= 	external/libpng-1.6.32
SRCDIRS		+=	external/libpng-1.6.32

INCDIRS		+= 	external/lua-5.3.5
SRCDIRS		+=	external/lua-5.3.5

INCDIRS		+= 	external/lua-cjson-2.1.0
SRCDIRS		+=	external/lua-cjson-2.1.0

INCDIRS		+= 	external/pixman-0.34.0 \
				external/pixman-0.34.0/$(ARCH)
SRCDIRS		+=	external/pixman-0.34.0 \
				external/pixman-0.34.0/$(ARCH)

INCDIRS		+= 	external/cairo-1.14.10
SRCDIRS		+=	external/cairo-1.14.10

INCDIRS		+= 	external/freetype-2.8/include \
				external/freetype-2.8/src/autofit \
				external/freetype-2.8/src/base \
				external/freetype-2.8/src/cache \
				external/freetype-2.8/src/cff \
				external/freetype-2.8/src/cid \
				external/freetype-2.8/src/gxvalid \
				external/freetype-2.8/src/otvalid \
				external/freetype-2.8/src/pfr \
				external/freetype-2.8/src/psaux \
				external/freetype-2.8/src/pshinter \
				external/freetype-2.8/src/psnames \
				external/freetype-2.8/src/raster \
				external/freetype-2.8/src/sfnt \
				external/freetype-2.8/src/smooth \
				external/freetype-2.8/src/truetype \
				external/freetype-2.8/src/type1 \
				external/freetype-2.8/src/type42 \
				external/freetype-2.8/src/winfonts
SRCDIRS		+=	external/freetype-2.8/src/autofit \
				external/freetype-2.8/src/base \
				external/freetype-2.8/src/cache \
				external/freetype-2.8/src/cff \
				external/freetype-2.8/src/cid \
				external/freetype-2.8/src/gxvalid \
				external/freetype-2.8/src/otvalid \
				external/freetype-2.8/src/pfr \
				external/freetype-2.8/src/psaux \
				external/freetype-2.8/src/pshinter \
				external/freetype-2.8/src/psnames \
				external/freetype-2.8/src/raster \
				external/freetype-2.8/src/sfnt \
				external/freetype-2.8/src/smooth \
				external/freetype-2.8/src/truetype \
				external/freetype-2.8/src/type1 \
				external/freetype-2.8/src/type42 \
				external/freetype-2.8/src/winfonts

INCDIRS		+=	external/chipmunk-7.0.2/include
SRCDIRS		+=	external/chipmunk-7.0.2/src

#
# You shouldn't need to change anything below this point.
#
# 命令工具定义
AS			:=	$(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:=	$(CROSS_COMPILE)gcc
CXX			:=	$(CROSS_COMPILE)g++
LD			:=	$(CROSS_COMPILE)ld
AR			:=	$(CROSS_COMPILE)ar
OC			:=	$(CROSS_COMPILE)objcopy
OD			:=	$(CROSS_COMPILE)objdump
SZ			:=	$(CROSS_COMPILE)size
MKDIR		:=	mkdir -p
CP			:=	cp -af
RM			:=	rm -fr
CD			:=	cd
FIND		:=	find
ifeq ($(strip $(HOSTOS)), macos)
CPIO		:=	gcpio -o -H newc --quiet
else
CPIO		:=	cpio -o -H newc --quiet
endif

#
# Xboot variables
#
# XBoot 命令参数定义
X_ASFLAGS	:=	$(MCFLAGS) $(ASFLAGS)
X_CFLAGS	:=	$(MCFLAGS) $(CFLAGS)
X_LDFLAGS	:=	$(LDFLAGS)
X_LIBS		:=	$(LIBS) -lgcc

# XBoot 输出路径定义
X_OUT		:=	../output
# XBoot 输出文件名定义
X_NAME		:=	$(patsubst %, $(X_OUT)/%, xboot)
# 头文件路径添加前缀 -I
X_INCDIRS	:=	$(patsubst %, -I %, $(INCDIRS))
# 库路径添前缀 -L
X_LIBDIRS	:=	$(patsubst %, -L %, $(LIBDIRS))
X_SRCDIRS	:=	$(patsubst %, %, $(SRCDIRS))
# 目标文件路径
X_OBJDIRS	:=	$(patsubst %, .obj/%, $(X_SRCDIRS))

# 搜索源文件路径下的.S文件
X_SFILES	:=	$(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.S))
# 搜索源文件路径下的.c文件
X_CFILES	:=	$(foreach dir, $(X_SRCDIRS), $(wildcard $(dir)/*.c))

X_SDEPS		:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o.d))
X_CDEPS		:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o.d))
X_DEPS		:=	$(X_SDEPS) $(X_CDEPS)

# .S生成的目标文件
X_SOBJS		:=	$(patsubst %, .obj/%, $(X_SFILES:.S=.o))
# .c生成的目标文件
X_COBJS		:=	$(patsubst %, .obj/%, $(X_CFILES:.c=.o))
# 全部目标文件
X_OBJS		:=	$(X_SOBJS) $(X_COBJS)

VPATH		:=	$(X_OBJDIRS)
.PHONY:	all clean xbegin xend xclean

#
# Xboot rules
#
all : xbegin $(X_NAME) xend

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@ $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) -v -O binary $@ $@.bin
	@echo    | tee -a $@.map
	@$(SZ) $@ | tee -a $@.map
	@echo    | tee -a $@.map

$(X_SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

$(X_COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

clean : xclean
	@$(RM) .obj $(X_OUT)

#
# Include the dependency files, should be place the last of makefile
#
sinclude $(shell $(MKDIR) $(X_OBJDIRS) $(X_OUT)						\
			&& $(RM) .obj/romdisk									\
			&& $(RM) .obj/romdisk.cpio								\
			&& $(RM) .obj/init/version.o							\
			&& $(RM) .obj/driver/block/romdisk/data.o				\
			&& $(CP) romdisk .obj									\
			&& $(CP) arch/$(ARCH)/$(MACH)/romdisk .obj				\
			&& $(CD) .obj/romdisk									\
			&& $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio	\
			&& $(CD) ../..)											\
			$(X_DEPS)
