#include "audio_stream.h"
#include "player.h"
#include <nds.h>
#include <maxmod9.h>

static bool playing = false;

/* Maxmod calls this whenever it needs more samples; `length` is in stereo
 * sample frames, `dest` is where to write interleaved s16 L/R. We pull
 * straight from whichever decoder player.c has open. On end-of-track we
 * hand back silence and flip `playing` off; main.c polls that to advance
 * to the next song. */
static mm_word stream_fill(mm_word length, mm_addr dest, mm_stream_formats format)
{
    (void)format; /* always MM_STREAM_16BIT_STEREO here */
    short *out = (short *)dest;
    int got = player_read_frames(out, (int)length);
    if (got < (int)length) {
        for (int i = got; i < (int)length; i++) { out[i * 2] = 0; out[i * 2 + 1] = 0; }
        if (got == 0) playing = false;
    }
    return length;
}

void audio_stream_init(void)
{
    /* Sound hardware (ARM7 side) needs enabling once before any audio -
     * see BlocksDS's own audio tutorial. Safe to call even if the default
     * core already enables it; soundEnable()/soundDisable() are
     * idempotent on libnds. */
    soundEnable();

    /* PROJECT NOISE never plays a canned Maxmod sample/module - everything
     * goes through the streaming API with our own MP3/WAV decoders - so
     * there's no soundbank to build or load. mmInitNoSoundbank() is
     * BlocksDS Maxmod's documented call for exactly this case (see
     * maxmod9.h: "You need to initialize Maxmod even if you don't plan on
     * using any soundbank. If that's your situation, use
     * mmInitNoSoundbank()."). This replaces the mmInitDefault()+soundbank
     * approach devkitARM/libctru-style maxmod would need. */
    mmInitNoSoundbank();
}

bool audio_stream_start(int sample_rate)
{
    mm_stream st;
    st.sampling_rate = (mm_word)sample_rate;
    st.buffer_length = 2048;                 /* frames per buffer; must be a multiple of 16 */
    st.callback      = stream_fill;
    st.format        = MM_STREAM_16BIT_STEREO;
    st.timer         = MM_TIMER3;            /* matches BlocksDS's own streaming docs example */
    st.manual        = false;

    mmStreamOpen(&st);
    playing = true;
    return true;
}

void audio_stream_stop(void)
{
    mmStreamClose();
    playing = false;
}

bool audio_stream_is_playing(void) { return playing; }
