# ARM7 side: default libnds system services (touch, sound hardware init,
# power) plus the maxmod7 audio mixer core. This CPU does not run any
# PROJECT NOISE logic - it just answers FIFO requests from the ARM9 side.

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM is not set")
endif

include $(DEVKITARM)/ds_rules

TARGET   := project_noise_dsi_arm7
BUILD    := build
SOURCES  := source
INCLUDES := source

ARCH    := -mthumb -mthumb-interwork
CFLAGS  := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH) -DARM7 -DDSI \
           -I$(LIBNDS)/include -Isource
LDFLAGS := -specs=ds_arm7.specs -g $(ARCH) -L$(LIBNDS)/lib
LIBS    := -lmm7 -lnds7

LIBNDS  := $(DEVKITPRO)/libnds

SRCS := $(wildcard $(SOURCES)/*.c)
OBJS := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET).elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	rm -rf $(BUILD) $(TARGET).elf
