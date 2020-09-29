#
# Makefile for xboot
#

#
# You can pass CROSS_COMPILE, PLATFORM and WBOXTEST variable.
#
#CROSS_COMPILE   ?= arm-linux-gnueabihf-
#PLATFORM       ?= arm32-realview
#PLATFORM       ?= arm32-rk3288
#PLATFORM       ?= arm32-h3

CROSS_COMPILE   ?= arm-eabi-
PLATFORM        ?= arm32-f1c100s
WBOXTEST        ?=

#
# Get platform information about ARCH and MACH from PLATFORM variable.
#
# 将PLATFORM中-替换为空格,然后判断单词数是否为2
ifeq ($(words $(subst -, , $(PLATFORM))), 2)
# ARCH为PLATFORM的-前一个单词，即arm32,所以ARCH=arm32
# MACH为PLATFORM的-后一个单词为后缀,即realview,所以MACH=mach-realview
ARCH            := $(word 1, $(subst -, , $(PLATFORM)))
MACH            := mach-$(word 2, $(subst -, , $(PLATFORM)))
else
ARCH            := x64
MACH            := mach-sandbox
endif

#
# System environment variable.
#
ifeq ($(OS), Windows_NT)
    HOSTOS      := windows
else
# 搜索系统名称是否为linux
    ifneq (,$(findstring Linux, $(shell uname -a)))
        HOSTOS  := linux
    endif
endif

#
# Load default variables.
#
ASFLAGS         := -g -ggdb -Wall -O0
CFLAGS          := -g -ggdb -Wall -O0
LDFLAGS         := -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS         :=

LIBDIRS         :=
LIBS            :=
INCDIRS         :=
SRCDIRS         :=

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
DEFINES     +=  -D__ARM32__
endif
ifeq ($(strip $(ARCH)), arm64)
DEFINES     +=  -D__ARM64__
endif
ifeq ($(strip $(ARCH)), riscv64)
DEFINES     +=  -D__RISCV64__
endif
ifeq ($(strip $(ARCH)), x64)
DEFINES     +=  -D__X64__
endif

# 追加ASFLAGS参数
ASFLAGS     +=  -ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
# 追加CFLAGS参数
CFLAGS      +=  -ffunction-sections -fdata-sections -ffreestanding -std=gnu99 $(DEFINES)
# 追加LDFLAGS参数
LDFLAGS     +=  -Wl,-gc-sections

#
# Add necessary directory for INCDIRS and SRCDIRS.
#
# 头文件路径
INCDIRS     +=  include \
                arch/$(ARCH)/include \
                arch/$(ARCH)/$(MACH)/include

# 源文件路径
SRCDIRS		+=	arch/$(ARCH)/lib/cpu \
				arch/$(ARCH)/lib \
				arch/$(ARCH)/$(MACH) \
				arch/$(ARCH)/$(MACH)/command \
				arch/$(ARCH)/$(MACH)/driver \
				lib/libc/charset \
				lib/libc/crypto \
				lib/libc/ctype \
				lib/libc/environ \
				lib/libc/errno \
				lib/libc/exit \
				lib/libc/filter \
				lib/libc/locale \
				lib/libc/malloc \
				lib/libc/path \
				lib/libc/stdio \
				lib/libc/stdlib \
				lib/libc/string \
				lib/libc/time \
				lib/libm/$(ARCH) \
				lib/libm \
				lib/libx \
				init \
				kernel/command \
				kernel/core \
				kernel/graphic \
				kernel/shell \
				kernel/time \
				kernel/vfs \
				kernel/vfs/cpio \
				kernel/vfs/ext4 \
				kernel/vfs/fat \
				kernel/vfs/ram \
				kernel/vfs/sys \
				kernel/vfs/tar \
				kernel/xfs \
				kernel/xui \
				driver/adc \
				driver/audio \
				driver/battery \
				driver/block \
				driver/block/partition \
				driver/buzzer \
				driver/camera \
				driver/clk \
				driver/clockevent \
				driver/clocksource \
				driver/compass \
				driver/console \
				driver/dac \
				driver/dma \
				driver/framebuffer \
				driver/gmeter \
				driver/gnss \
				driver/gpio \
				driver/gyroscope \
				driver/hygrometer \
				driver/i2c \
				driver/input \
				driver/input/rc \
				driver/interrupt \
				driver/led \
				driver/light \
				driver/motor \
				driver/nvmem \
				driver/pressure \
				driver/proximity \
				driver/pwm \
				driver/regulator \
				driver/reset \
				driver/rng \
				driver/rtc \
				driver/sd \
				driver/servo \
				driver/spi \
				driver/stepper \
				driver/thermometer \
				driver/uart \
				driver/vibrator \
				driver/watchdog \
				framework \
				framework/codec \
				framework/core \
				framework/hardware

#
# Add external library
#
INCDIRS     +=  external/libfdt-1.4.2
SRCDIRS     +=  external/libfdt-1.4.2

INCDIRS     +=  external/lz4-1.8.2
SRCDIRS     +=  external/lz4-1.8.2

INCDIRS     +=  external/zlib-1.2.11
SRCDIRS     +=  external/zlib-1.2.11

INCDIRS     +=  external/libpng-1.6.37
SRCDIRS     +=  external/libpng-1.6.37

INCDIRS		+= 	external/jpeg-9d
SRCDIRS		+=	external/jpeg-9d

INCDIRS		+= 	external/lua-5.4.0
SRCDIRS		+=	external/lua-5.4.0

INCDIRS     +=  external/lua-cjson-2.1.0
SRCDIRS     +=  external/lua-cjson-2.1.0

INCDIRS		+=	external/quirc-1.1.0
SRCDIRS		+=	external/quirc-1.1.0

INCDIRS		+= 	external/freetype-2.10.1/include \
				external/freetype-2.10.1/src/autofit \
				external/freetype-2.10.1/src/base \
				external/freetype-2.10.1/src/cache \
				external/freetype-2.10.1/src/cff \
				external/freetype-2.10.1/src/cid \
				external/freetype-2.10.1/src/gxvalid \
				external/freetype-2.10.1/src/otvalid \
				external/freetype-2.10.1/src/pfr \
				external/freetype-2.10.1/src/psaux \
				external/freetype-2.10.1/src/pshinter \
				external/freetype-2.10.1/src/psnames \
				external/freetype-2.10.1/src/raster \
				external/freetype-2.10.1/src/sfnt \
				external/freetype-2.10.1/src/smooth \
				external/freetype-2.10.1/src/truetype \
				external/freetype-2.10.1/src/type1 \
				external/freetype-2.10.1/src/type42 \
				external/freetype-2.10.1/src/winfonts
SRCDIRS		+=	external/freetype-2.10.1/src/autofit \
				external/freetype-2.10.1/src/base \
				external/freetype-2.10.1/src/cache \
				external/freetype-2.10.1/src/cff \
				external/freetype-2.10.1/src/cid \
				external/freetype-2.10.1/src/gxvalid \
				external/freetype-2.10.1/src/otvalid \
				external/freetype-2.10.1/src/pfr \
				external/freetype-2.10.1/src/psaux \
				external/freetype-2.10.1/src/pshinter \
				external/freetype-2.10.1/src/psnames \
				external/freetype-2.10.1/src/raster \
				external/freetype-2.10.1/src/sfnt \
				external/freetype-2.10.1/src/smooth \
				external/freetype-2.10.1/src/truetype \
				external/freetype-2.10.1/src/type1 \
				external/freetype-2.10.1/src/type42 \
				external/freetype-2.10.1/src/winfonts

INCDIRS		+= 	external/pixman-0.40.0 \
				external/pixman-0.40.0/$(ARCH)
SRCDIRS		+=	external/pixman-0.40.0 \
				external/pixman-0.40.0/$(ARCH)

INCDIRS     +=  external/cairo-1.17.2
SRCDIRS     +=  external/cairo-1.17.2

INCDIRS     +=  external/chipmunk-7.0.3/include
SRCDIRS     +=  external/chipmunk-7.0.3/src

#
# Add whitebox test
#
ifneq ($(strip $(WBOXTEST)),)
INCDIRS		+=	wboxtest
SRCDIRS		+=	wboxtest \
				wboxtest/benchmark \
				wboxtest/block \
				wboxtest/camera \
				wboxtest/crypto \
				wboxtest/dma \
				wboxtest/graphic \
				wboxtest/path \
				wboxtest/stdio
endif

#
# You shouldn't need to change anything below this point.
#
# 命令工具定义
AS          :=  $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC          :=  $(CROSS_COMPILE)gcc
CXX         :=  $(CROSS_COMPILE)g++
LD          :=  $(CROSS_COMPILE)ld
AR          :=  $(CROSS_COMPILE)ar
SZ          :=  $(CROSS_COMPILE)size
OC          :=  $(CROSS_COMPILE)objcopy
OD          :=  $(CROSS_COMPILE)objdump
SZ          :=  $(CROSS_COMPILE)size
NM          :=  $(CROSS_COMPILE)nm
MKDIR       :=  mkdir -p
CP          :=  cp -af
RM          :=  rm -fr
CD          :=  cd
FIND        :=  find
CPIO        :=  cpio -o -H newc --quiet

#
# Xboot variables
#
# XBoot 命令参数定义
X_ASFLAGS   :=  $(MCFLAGS) $(ASFLAGS)
X_CFLAGS    :=  $(MCFLAGS) $(CFLAGS)
X_LDFLAGS   :=  $(LDFLAGS)
X_LIBS      :=  $(LIBS) -lgcc

# XBoot 输出路径定义
X_OUT       :=  ../output
# XBoot 输出文件名定义
X_NAME      :=  $(patsubst %, $(X_OUT)/%, xboot)
# 头文件路径添加前缀 -I
X_INCDIRS   :=  $(patsubst %, -I %, $(INCDIRS))
# 库路径添前缀 -L
X_LIBDIRS   :=  $(patsubst %, -L %, $(LIBDIRS))
X_SRCDIRS   :=  $(patsubst %, %, $(SRCDIRS))
# 目标文件路径
X_OBJDIRS   :=  $(patsubst %, .obj/%, $(X_SRCDIRS))

# 带arch/前缀的目录
X_ARCHS     :=  $(filter arch/%, $(X_SRCDIRS))
# 其他不带arch/前缀的目录
X_OTHERS    :=  $(filter-out arch/%, $(X_SRCDIRS))

# 获取arch/目录下所有的*.S文件
X_SFILES    :=  $(foreach dir, $(X_ARCHS), $(wildcard $(dir)/*.S))
# 获取arch/目录下所有的*.c文件
X_CFILES    :=  $(foreach dir, $(X_ARCHS), $(wildcard $(dir)/*.c))

# arch/目录下.S生成的目标文件
X_SOBJS     :=  $(patsubst %, .obj/%, $(X_SFILES:.S=.o))
# arch/目录下.c生成的目标文件
X_COBJS     :=  $(patsubst %, .obj/%, $(X_CFILES:.c=.o))
# 全部目标文件
X_OBJS      :=  $(X_SOBJS) $(X_COBJS) $(foreach dir, $(X_OTHERS), .obj/$(dir)/built-in.o)

.PHONY: all clean romdisk xbegin xend xclean $(X_NAME) $(X_ARCHS) $(X_OTHERS)
export AS CC LD X_ASFLAGS X_CFLAGS X_INCDIRS

#
# Xboot rules
#
all : xend

# 最终目标
xend : $(X_NAME)

$(X_NAME) : $(X_OBJS)
	@echo [LD] Linking $@
	@$(CC) $(X_LDFLAGS) $(X_LIBDIRS) -Wl,--cref,-Map=$@.map $^ -o $@ $(X_LIBS)
	@echo [OC] Objcopying $@.bin
	@$(OC) -v -O binary $@ $@.bin
	@echo     | tee -a $@.map
	@$(SZ) $@ | tee -a $@.map
	@echo     | tee -a $@.map
ifneq ($(OUTPUT_OBJDUMP),)
	@echo [OD] Objdumping $@.objdump
	@$(OD) -D -S $@ > $@.objdump
endif
ifneq ($(OUTPUT_NM),)
	@echo [NM] Nm-ing $@.nm
	@$(NM) $@ | sort > $@.nm
endif

$(X_OBJS) : $(X_OTHERS)

# 其他目标对象
$(X_OTHERS) : $(X_ARCHS)
	@$(MAKE) -s -f rules.mk SRCDIR=$@ NAME=.obj/$@/built-in.o

# arch目标对象
$(X_ARCHS) : xbegin
	@$(MAKE) -s -f rules.mk SRCDIR=$@

xbegin : romdisk

# 创建romdisk
romdisk :
	@echo [ROMDISK] Packing romdisk
	# 创建所有的.obj目录下的目录和../output目录
	# 然后将romdisk目录和arch/.../romdisk拷贝到.obj目录下合成,并打包成romdisk.cpio
	@$(MKDIR) $(X_OBJDIRS) $(X_OUT) \
		&& $(RM) .obj/init/version.o \
		&& $(RM) .obj/driver/block/romdisk.o \
		&& $(RM) .obj/romdisk \
		&& $(RM) .obj/romdisk.cpio \
		&& $(CP) romdisk .obj \
		&& $(CP) arch/$(ARCH)/$(MACH)/romdisk .obj \
		&& $(CD) .obj/romdisk \
		&& $(FIND) . -not -name . | $(CPIO) > ../romdisk.cpio \
		&& $(CD) ../..

# 清除目标
clean : xclean
	@$(RM) .obj $(X_OUT)
