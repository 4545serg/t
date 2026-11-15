#include "mp3_dec.h"
#include <stdlib.h>
#include <string.h>

#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_SIMD   /* ARM9 has no NEON/SSE; avoid minimp3 reaching for it */
#include "minimp3.h"

#define READ_BUF_BYTES   (16 * 1024)
#define PCM_SCRATCH_FRAMES 1152

struct Mp3Dec {
    FILE *f;
    mp3dec_t dec;
    uint8_t  buf[READ_BUF_BYTES];
    int      buf_len;
    bool     eof;
    int      sample_rate;
    int      channels;
    int16_t  scratch[PCM_SCRATCH_FRAMES * 2];
    int      scratch_frames;
    int      scratch_pos;
};

static void refill(Mp3Dec *m)
{
    if (m->eof) return;
    size_t got = fread(m->buf + m->buf_len, 1, (size_t)(READ_BUF_BYTES - m->buf_len), m->f);
    m->buf_len += (int)got;
    if (got == 0) m->eof = true;
}

Mp3Dec *mp3_open(const char *path)
{
    Mp3Dec *m = calloc(1, sizeof *m);
    if (!m) return NULL;
    m->f = fopen(path, "rb");
    if (!m->f) { free(m); return NULL; }
    mp3dec_init(&m->dec);
    refill(m);

    mp3dec_frame_info_t info;
    int16_t probe[MINIMP3_MAX_SAMPLES_PER_FRAME];
    int n = mp3dec_decode_frame(&m->dec, m->buf, m->buf_len, probe, &info);
    if (info.frame_bytes <= 0 || info.hz <= 0) { fclose(m->f); free(m); return NULL; }
    m->sample_rate = info.hz;
    m->channels    = info.channels;
    if (n > 0) {
        memcpy(m->scratch, probe, (size_t)n * (size_t)m->channels * sizeof(int16_t));
        m->scratch_frames = n;
    }
    memmove(m->buf, m->buf + info.frame_bytes, (size_t)(m->buf_len - info.frame_bytes));
    m->buf_len -= info.frame_bytes;
    refill(m);
    return m;
}

void mp3_close(Mp3Dec *m)
{
    if (!m) return;
    if (m->f) fclose(m->f);
    free(m);
}

int mp3_sample_rate(const Mp3Dec *m) { return m ? m->sample_rate : 44100; }

static void push_stereo(int16_t *out, int *out_n, const int16_t *src, int frames, int channels)
{
    for (int i = 0; i < frames; i++) {
        int16_t l = src[i * channels + 0];
        int16_t r = (channels == 2) ? src[i * channels + 1] : l;
        out[(*out_n) * 2 + 0] = l;
        out[(*out_n) * 2 + 1] = r;
        (*out_n)++;
    }
}

int mp3_read_frames(Mp3Dec *m, int16_t *out, int want_frames)
{
    if (!m) return 0;
    int n = 0;

    if (m->scratch_pos < m->scratch_frames) {
        int avail = m->scratch_frames - m->scratch_pos;
        int take = avail < want_frames ? avail : want_frames;
        push_stereo(out, &n, m->scratch + m->scratch_pos * m->channels, take, m->channels);
        m->scratch_pos += take;
        if (n >= want_frames) return n;
    }

    while (n < want_frames) {
        if (m->buf_len <= 0) { refill(m); if (m->buf_len <= 0) break; }

        mp3dec_frame_info_t info;
        int16_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
        int fn = mp3dec_decode_frame(&m->dec, m->buf, m->buf_len, pcm, &info);

        if (info.frame_bytes <= 0) break;

        memmove(m->buf, m->buf + info.frame_bytes, (size_t)(m->buf_len - info.frame_bytes));
        m->buf_len -= info.frame_bytes;
        if (m->buf_len < READ_BUF_BYTES / 2) refill(m);

        if (fn <= 0) continue;

        int room = want_frames - n;
        int take = fn < room ? fn : room;
        push_stereo(out + n * 2, &n, pcm, take, info.channels);

        if (take < fn) {
            m->scratch_frames = fn - take;
            m->scratch_pos = 0;
            memcpy(m->scratch, pcm + take * info.channels,
                   (size_t)m->scratch_frames * (size_t)info.channels * sizeof(int16_t));
        }
    }
    return n;
}
