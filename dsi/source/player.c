#include "player.h"
#include "wav_dec.h"
#include "mp3_dec.h"
#include <string.h>
#include <strings.h>
#include <stddef.h>

typedef enum { TRACK_NONE, TRACK_WAV, TRACK_MP3 } TrackKind;

static TrackKind cur_kind = TRACK_NONE;
static WavDec    cur_wav;
static Mp3Dec   *cur_mp3 = NULL;

static bool ends_with_ci(const char *s, const char *suffix)
{
    size_t ls = strlen(s), lx = strlen(suffix);
    return ls >= lx && strcasecmp(s + (ls - lx), suffix) == 0;
}

void player_close(void)
{
    if (cur_kind == TRACK_WAV) wav_close(&cur_wav);
    if (cur_kind == TRACK_MP3) mp3_close(cur_mp3);
    cur_mp3 = NULL;
    cur_kind = TRACK_NONE;
}

bool player_open(const char *path)
{
    player_close();

    if (ends_with_ci(path, ".wav")) {
        if (wav_open(&cur_wav, path)) { cur_kind = TRACK_WAV; return true; }
        wav_close(&cur_wav);
        return false;
    }
    if (ends_with_ci(path, ".mp3")) {
        cur_mp3 = mp3_open(path);
        if (cur_mp3) { cur_kind = TRACK_MP3; return true; }
        return false;
    }
    /* .flac falls through to here and is rejected - see player.h. */
    return false;
}

bool player_is_open(void) { return cur_kind != TRACK_NONE; }

int player_sample_rate(void)
{
    if (cur_kind == TRACK_WAV) return cur_wav.sample_rate;
    if (cur_kind == TRACK_MP3) return mp3_sample_rate(cur_mp3);
    return 44100;
}

int player_read_frames(short *out, int want_frames)
{
    if (cur_kind == TRACK_WAV) return wav_read_frames(&cur_wav, out, want_frames);
    if (cur_kind == TRACK_MP3) return mp3_read_frames(cur_mp3, out, want_frames);
    return 0;
}
