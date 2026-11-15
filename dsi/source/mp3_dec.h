#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* Streaming MP3 decode on top of minimp3 (vendored by get_deps.sh, same
 * header the 3DS build uses - it's plain C99, no devkitARM/BlocksDS
 * dependency). minimp3 decodes one frame (up to 1152 samples/channel) per
 * call from an in-memory buffer, so this wrapper keeps a read-ahead
 * buffer from the file and refills it as frames are consumed. */

typedef struct Mp3Dec Mp3Dec;

Mp3Dec *mp3_open(const char *path);
void mp3_close(Mp3Dec *m);

/* Decodes into `out` (interleaved s16, stereo) up to `want_frames` frames.
 * Returns frames written; 0 means end of file or an unrecoverable error. */
int mp3_read_frames(Mp3Dec *m, int16_t *out, int want_frames);

int mp3_sample_rate(const Mp3Dec *m);
