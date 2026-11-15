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

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9_only/Makefile.arm9
