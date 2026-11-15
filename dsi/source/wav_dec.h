#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE *f;
    uint32_t data_start, data_end, pos; /* byte offsets within the file */
    int channels;        /* 1 or 2 */
    int sample_rate;
    int bits_per_sample;  /* 8 or 16 - PROJECT NOISE only writes/reads PCM WAVs */
} WavDec;

/* Opens a PCM WAV file and parses its header. Returns false (and leaves *f
 * open for the caller to close) on anything that isn't a plain PCM WAV. */
bool wav_open(WavDec *w, const char *path);
void wav_close(WavDec *w);

/* Fills `out` with up to `want_frames` stereo s16 frames (2 int16 per frame,
 * mono source is duplicated to both channels). Returns frames actually
 * written; 0 means end of file. */
int wav_read_frames(WavDec *w, int16_t *out, int want_frames);
