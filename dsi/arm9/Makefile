# ARM9 side: PROJECT NOISE's player logic, SD browser, and decoders.

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM is not set")
endif

include $(DEVKITARM)/ds_rules

TARGET   := project_noise_dsi_arm9
BUILD    := build
SOURCES  := source
DATA     := data
AUDIO    := audio

LIBNDS   := $(DEVKITPRO)/libnds

ARCH    := -mthumb -mthumb-interwork
CFLAGS  := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH) -DARM9 -DDSI \
           -Isource -I$(BUILD) -I$(LIBNDS)/include
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -L$(LIBNDS)/lib
LIBS    := -lfilesystem -lfat -lmm9 -lnds9

SRCS := $(wildcard $(SOURCES)/*.c)
OBJS := $(patsubst $(SOURCES)/%.c,$(BUILD)/%.o,$(SRCS))
DEPS := $(SOURCES)/minimp3.h

# --- maxmod soundbank ---------------------------------------------------
# maxmod's ARM9 side (mmInitDefault) always expects a soundbank built by
# mmutil, even when - as here - the app plays everything through the
# streaming API and never triggers a canned sample. `audio/silence.wav`
# exists only so mmutil has at least one entry to build a (otherwise
# empty-of-real-use) bank from; nothing in the code ever plays it.
$(BUILD)/soundbank.bin $(SOURCES)/soundbank.h: $(wildcard $(AUDIO)/*) | $(BUILD)
	mmutil $(AUDIO)/*.wav -d -o$(BUILD)/soundbank.bin -h$(SOURCES)/soundbank.h

# devkitPro's ds_rules provides the generic `data/%.bin -> build/%.bin.o`
# conversion (via bin2s) that exposes each file as `<name>_bin[]` /
# `<name>_bin_size` - point it at the soundbank mmutil just built.
$(DATA)/soundbank.bin: $(BUILD)/soundbank.bin | $(DATA)
	cp $< $@

.PHONY: all clean deps

all: $(TARGET).elf

deps: $(DEPS)

$(SOURCES)/minimp3.h:
	sh ../tools/get_deps.sh

$(BUILD) $(DATA):
	mkdir -p $@

$(BUILD)/%.o: $(SOURCES)/%.c $(DEPS) $(SOURCES)/soundbank.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) $(BUILD)/soundbank.bin.o
	$(CC) $(LDFLAGS) $(OBJS) $(BUILD)/soundbank.bin.o $(LIBS) -o $@

clean:
	rm -rf $(BUILD) $(DATA) $(TARGET).elf $(SOURCES)/soundbank.h
