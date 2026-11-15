#pragma once
#include <stdbool.h>

/* Thin dispatcher over wav_dec.c / mp3_dec.c so main.c and the maxmod
 * stream callback don't need to know which decoder is active. FLAC is
 * deliberately not wired in yet - see dsi/PORT_PLAN.md phase 4; dr_flac's
 * CPU cost on a 67 MHz ARM9 needs measuring on real hardware first. */

bool player_open(const char *path);   /* closes whatever was open first */
void player_close(void);
bool player_is_open(void);
int  player_sample_rate(void);        /* valid once player_open() succeeds */

/* Called from the maxmod stream callback (runs in an IRQ-ish context):
 * fills `out` with up to `want_frames` interleaved stereo s16 frames.
 * Returns frames written; 0 = end of track. */
int player_read_frames(short *out, int want_frames);
