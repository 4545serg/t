# Building the DSi port

This port is built with **BlocksDS** (https://blocksds.skylyrac.net/), not
devkitARM+libnds. Two reasons: BlocksDS ships a prebuilt default ARM7 core
that already includes Maxmod + libnds - exactly what this player needs, so
no custom ARM7 binary has to be built - and devkitPro's own maintainer has
said plainly that pacman-based installs aren't meant to run in CI
(https://devkitpro.org/viewtopic.php?p=16862), which is what an earlier
version of this port tried to do and why its first CI run failed.

## Building via GitHub Actions (recommended, no local install needed)

`.github/workflows/dsi-build.yml` (at the repo root) builds
`project_noise_dsi.nds` automatically on every push that touches `dsi/`,
using BlocksDS's own official Docker image (`skylyrac/blocksds:slim-latest`),
which has the whole toolchain preinstalled - nothing is installed at
build time, so there's no pacman/network-mirror step to fail.

1. Push this repo to GitHub.
2. Open the repo's **Actions** tab - a "Build DSi .nds" run should start
   (or trigger it with **Run workflow**).
3. Download the **project_noise_dsi** artifact from the finished run.
4. Copy the `.nds` to the SD card and launch it with TWiLight Menu++.

## Building locally

1. Install BlocksDS: https://blocksds.skylyrac.net/docs/setup/ (or use
   their Docker image locally - same page, "Docker" section).
2. Set `BLOCKSDS` (e.g. `export BLOCKSDS=/opt/blocksds/core`).
3. `cd dsi && sh tools/get_deps.sh && make`

## Status - phase 1

What's written (**not yet compiled or run on real hardware or an
emulator - I have no BlocksDS toolchain available to build or test this
myself**, so treat this as a strong starting point to compile, run, and
report bugs against, not a finished build):

- SD card browser (`SD:/Music/`, sub-folders, `..` to go up) - ported
  almost unchanged from the 3DS build's `browser.c` (plain POSIX
  dirent/qsort, no platform dependency).
- MP3 playback via minimp3, decoded on ARM9 and streamed to Maxmod's
  software-streaming API (`mmStreamOpen`), which BlocksDS's default ARM7
  core mixes and plays - the documented way to play a format Maxmod
  doesn't decode natively (BlocksDS's own audio-streaming docs list this
  as a supported pattern).
- PCM WAV playback (8 or 16-bit, mono or stereo).
- D-pad to move the selection, A to open a folder / play a song, B to
  stop, START to quit. Auto-advance to the next song in the folder.
- Correct Maxmod init for a player with no canned samples:
  `mmInitNoSoundbank()` (an earlier version of this port used
  `mmInitDefault()` with a dummy soundbank, which was unnecessary
  complexity working around the wrong function).

What's deliberately not here yet (see `PORT_PLAN.md`):

- The actual PROJECT NOISE interface - this phase draws a plain text list
  on the bottom-screen console, not the cover flow / Spotify-style
  browser. The real UI is phase 2, built on top of this playback core.
- FLAC. dr_flac's CPU cost needs benchmarking on real DSi hardware before
  committing to it; `player.c` already has the dispatch point to add it.
- Favorites, queue, search, settings, lyrics, visualizer, custom keyboard.

## Known risk points to check first on hardware

- `mm_stream` field values (`timer: MM_TIMER3`, `buffer_length: 2048`) in
  `source/audio_stream.c` match BlocksDS's published streaming docs, but
  haven't been exercised on hardware. If audio is corrupted or silent,
  check whether another part of the program is also using MM_TIMER3.
- The icon at `resources/icon.bmp` is an auto-generated placeholder
  (8-bit indexed); `ndstool`, which BlocksDS uses under the hood, wants a
  4-bit/16-color 32x32 BMP for the icon. If the build fails on the icon
  step, re-save it as 4bpp/16-color (`convert icon.bmp -colors 16 -depth 4
  icon.bmp` with ImageMagick, or any image editor's "16 colors" BMP
  export).
- minimp3 on ARM9 thumb with no FPU: decode speed and memory use (its
  internal tables plus the 16 KB read buffer in `source/mp3_dec.c`) have
  not been measured on hardware. If MP3s stutter, that buffer and the
  `MINIMP3_NO_SIMD` path are the first places to look.
- `soundEnable()` in `audio_stream_init()` is defensive - BlocksDS's docs
  don't explicitly show it before `mmInitNoSoundbank()` in their Maxmod
  example, only in their LibXM7 and raw libnds-sound examples. It should
  be harmless either way, but worth knowing if debugging silence.
