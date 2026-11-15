#pragma once
/* DSi path layout - "sdmc:/" is the default-drive alias for the DS/DSi SD
 * slot (TWiLight Menu++ mounts the real SD card there), matching the 3DS
 * build's "sdmc:/" convention so the folder layout on the card can be
 * shared between both versions. */
#define APP_NAME       "PROJECT NOISE"
#define MUSIC_ROOT      "sdmc:/Music/"
#define DATA_ROOT       "sdmc:/_nds/PROJECT_NOISE"
#define SETTINGS_PATH   "sdmc:/_nds/PROJECT_NOISE/settings.cfg"
#define FAVORITES_PATH  "sdmc:/_nds/PROJECT_NOISE/favorites.txt"
#define RESUME_PATH     "sdmc:/_nds/PROJECT_NOISE/resume.cfg"
