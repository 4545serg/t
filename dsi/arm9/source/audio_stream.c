#include "audio_stream.h"
#include "player.h"
#include <nds.h>
#include <maxmod9.h>

static bool playing = false;

/* maxmod calls this whenever it needs more samples; `length` is in stereo
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
    /* Nothing persistent to set up beyond mmInstall(), already called from
     * main() before this; kept as a separate entry point so later phases
     * (e.g. a double-buffered visualizer tap) have somewhere to hook in. */
}

bool audio_stream_start(int sample_rate)
{
    mm_stream st;
    st.sampling_rate = (mm_word)sample_rate;
    st.buffer_length = 2048;                 /* frames per maxmod-side buffer */
    st.callback      = stream_fill;
    st.format        = MM_STREAM_16BIT_STEREO;
    st.timer         = MM_TIMER0;
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
