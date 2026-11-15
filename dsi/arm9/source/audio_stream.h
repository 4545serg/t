#pragma once
#include <stdbool.h>

/* Wraps maxmod9's streaming (MM_STREAM) API. The ARM9 side decodes PCM
 * (see player.c) and this module's callback hands it to maxmod, which
 * forwards it to the maxmod7 mixer running on ARM7 (see arm7/source/main.c)
 * over the FIFO. This is the standard devkitPro way to play a format
 * libnds doesn't know about natively (MP3/FLAC), same approach used by
 * DS homebrew players like Moonshell.
 *
 * NEEDS HARDWARE VERIFICATION: the exact mm_stream field names/signature
 * can differ slightly between maxmod versions - check against the
 * maxmod9.h installed by your devkitPro before building. */

void audio_stream_init(void);          /* call once, after mmInstall is up */
bool audio_stream_start(int sample_rate);
void audio_stream_stop(void);
bool audio_stream_is_playing(void);
