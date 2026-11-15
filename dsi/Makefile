# PROJECT NOISE - Nintendo DSi port, built with BlocksDS.
#
# BlocksDS (https://blocksds.skylyrac.net/) replaces devkitARM+libnds for
# this project: it ships a prebuilt default ARM7 core (Maxmod + libnds,
# which is exactly what this player needs - no custom ARM7 binary
# required), and - importantly for this project - devkitPro's own
# maintainer has said pacman-based installs aren't meant to run in CI
# (https://devkitpro.org/viewtopic.php?p=16862). BlocksDS instead ships
# an official Docker image with everything preinstalled; see
# ../.github/workflows/dsi-build.yml.
#
#   make        -> project_noise_dsi.nds
#   make clean
#
# Local build: install BlocksDS (https://blocksds.skylyrac.net/docs/setup/)
# and set BLOCKSDS (e.g. `export BLOCKSDS=/opt/blocksds/core`).

NAME          := project_noise_dsi

GAME_TITLE    := PROJECT NOISE
GAME_SUBTITLE := Music Player
GAME_AUTHOR   := GRIMZZZZ
GAME_ICON     := resources/icon.bmp

SOURCEDIRS    := source
INCLUDEDIRS   :=
GFXDIRS       :=
BINDIRS       :=
AUDIODIRS     :=
NITROFSDIR    :=

# No custom ARM7 core needed - BlocksDS's default core already includes
# Maxmod + libnds, which is all this player uses (no DSWiFi).
LIBS    := -lmm9 -lnds9
LIBDIRS := $(BLOCKSDS)/libs/maxmod \
           $(BLOCKSDS)/libs/libnds

# Which exact path BlocksDS uses for its "ARM9-only, default ARM7 core"
# template has been inconsistent across their own docs ("rom_arm9_only" in
# one page, "rom_arm9" in another) - rather than hardcode a guess a third
# time, try the plausible candidates and use whichever actually exists in
# this install. If none match, print the real directory listing so the
# correct path is visible directly in the build log instead of needing
# another round of guessing.
_TEMPLATE_CANDIDATES := \
    $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile.arm9 \
    $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile \
    $(BLOCKSDS)/sys/default_makefiles/rom_arm9_only/Makefile.arm9 \
    $(BLOCKSDS)/sys/default_makefiles/rom_arm9_only/Makefile
_TEMPLATE := $(firstword $(wildcard $(_TEMPLATE_CANDIDATES)))

ifeq ($(_TEMPLATE),)
$(info Looked for an ARM9-only BlocksDS template at:)
$(foreach p,$(_TEMPLATE_CANDIDATES),$(info   $(p)))
$(info None of those exist. Here is what IS under $(BLOCKSDS)/sys/default_makefiles/:)
$(info $(shell ls -la $(BLOCKSDS)/sys/default_makefiles/ 2>&1))
$(info And here is what's one level deeper, in case the right file is a template folder away:)
$(info $(shell find $(BLOCKSDS)/sys/default_makefiles/ -maxdepth 2 2>&1))
$(error Could not find a matching BlocksDS template Makefile - see the directory listing above for the real path, then update _TEMPLATE_CANDIDATES in dsi/Makefile to match it)
endif

include $(_TEMPLATE)
