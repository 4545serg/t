#pragma once
#include <stdbool.h>

/* Wraps Maxmod's streaming (mm_stream) API, BlocksDS build. The ARM9 side
 * decodes PCM (see player.c) and this module's callback hands it to
 * Maxmod, which forwards it to the default ARM7 core BlocksDS ships
 * (BlocksDS's standard "default core includes Maxmod and libnds only" -
 * see docs/guides/devkitarm_porting_guide, section 2). This is the
 * documented way to play a format Maxmod doesn't decode natively
 * (MP3/FLAC): https://blocksds.skylyrac.net/maxmod/md_documentation_2audio__streaming.html
 * lists "You can stream raw audio with Maxmod" as a supported pattern.
 *
 * NEEDS HARDWARE VERIFICATION: written from BlocksDS's published API
 * docs, not compiled here (no toolchain available in this environment).
 */

void audio_stream_init(void);          /* call once before playback starts */
bool audio_stream_start(int sample_rate);
void audio_stream_stop(void);
bool audio_stream_is_playing(void);
