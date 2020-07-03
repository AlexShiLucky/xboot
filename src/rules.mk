# 获取SRCDIR路径下所有的*.S文件
SFILES  := $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))
# 获取SRCDIR路径下所有的*.c文件
CFILES  := $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))

# .S文件的依赖文件在.obj/*.o.d
SDEPS   := $(patsubst %, .obj/%, $(SFILES:.S=.o.d))
# .c文件的依赖文件在.obj/*.o.d
CDEPS   := $(patsubst %, .obj/%, $(CFILES:.c=.o.d))
# 所有的依赖文件在.obj/*.o.d
DEPS    := $(SDEPS) $(CDEPS)

# .S文件的目标文件在.obj/*.o
SOBJS   := $(patsubst %, .obj/%, $(SFILES:.S=.o))
# .c文件的目标文件在.obj/*.o
COBJS   := $(patsubst %, .obj/%, $(CFILES:.c=.o))
# 所有的目标文件在.obj/*.o
OBJS    := $(SOBJS) $(COBJS)

ifneq ($(strip $(NAME)),)
all : $(NAME)
else
all : $(OBJS)
endif

$(NAME) : $(OBJS)
ifneq ($(strip $(OBJS)),)
	@echo "[LD] $@ ($^)"
	$(LD) -r -o $@ $^
else
	@echo "[AR] $@"
	@$(AR) -rcs $@
endif

# 汇编*.S文件规则
$(SOBJS) : .obj/%.o : %.S
	@echo "[AS] $<"
	@$(AS) $(X_ASFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

# 编译*.c文件规则
$(COBJS) : .obj/%.o : %.c
ifneq ($(OUTPUT_I),)
	@echo "[CC -E] $(<:.c=.i)"
	@$(CC) $(X_CFLAGS) $(X_INCDIRS) -E -C $< -o $(@:.o=.i)
endif
ifneq ($(OUTPUT_S),)
	@echo "[CC -S] $(<:.c=.s)"
	@$(CC) $(X_CFLAGS) $(X_INCDIRS) -S $< -o $(@:.o=.s)
endif
	@echo "[CC] $<"
	@$(CC) $(X_CFLAGS) -MD -MP -MF $@.d $(X_INCDIRS) -c $< -o $@

# 包含进依赖文件
sinclude $(DEPS)
