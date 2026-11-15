#include "wav_dec.h"
#include <string.h>

/* Minimal RIFF/WAVE PCM reader - just enough to stream 8/16-bit mono or
 * stereo PCM, which is the only WAV flavour PROJECT NOISE's own ripper
 * (and most DSi-side music tools) produce. No float/ADPCM/extensible
 * format support - those are rejected rather than mis-decoded. */

static uint32_t rd_u32(FILE *f) {
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4) return 0;
    return (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
}
static uint16_t rd_u16(FILE *f) {
    uint8_t b[2];
    if (fread(b, 1, 2, f) != 2) return 0;
    return (uint16_t)(b[0] | b[1] << 8);
}

bool wav_open(WavDec *w, const char *path)
{
    memset(w, 0, sizeof *w);
    w->f = fopen(path, "rb");
    if (!w->f) return false;

    uint8_t tag[4];
    if (fread(tag, 1, 4, w->f) != 4 || memcmp(tag, "RIFF", 4) != 0) return false;
    rd_u32(w->f); /* chunk size, unused */
    if (fread(tag, 1, 4, w->f) != 4 || memcmp(tag, "WAVE", 4) != 0) return false;

    bool have_fmt = false;
    uint16_t fmt_tag = 0;
    for (;;) {
        uint8_t id[4];
        if (fread(id, 1, 4, w->f) != 4) break;
        uint32_t sz = rd_u32(w->f);
        long chunk_pos = ftell(w->f);

        if (memcmp(id, "fmt ", 4) == 0) {
            fmt_tag          = rd_u16(w->f);
            w->channels      = rd_u16(w->f);
            w->sample_rate   = (int)rd_u32(w->f);
            rd_u32(w->f);              /* byte rate */
            rd_u16(w->f);              /* block align */
            w->bits_per_sample = rd_u16(w->f);
            have_fmt = true;
        } else if (memcmp(id, "data", 4) == 0) {
            w->data_start = (uint32_t)chunk_pos;
            w->data_end   = w->data_start + sz;
            w->pos        = w->data_start;
            /* Keep scanning only if fmt hasn't been seen yet; normally fmt
             * comes first, so once we have both we're done. */
            if (have_fmt) break;
        }
        fseek(w->f, chunk_pos + (long)sz + (sz & 1), SEEK_SET); /* chunks are word-aligned */
    }

    if (!have_fmt || w->data_end == 0) return false;
    if (fmt_tag != 1 /* PCM */ || (w->bits_per_sample != 16 && w->bits_per_sample != 8))
        return false;
    if (w->channels != 1 && w->channels != 2) return false;

    fseek(w->f, (long)w->data_start, SEEK_SET);
    return true;
}

void wav_close(WavDec *w)
{
    if (w->f) fclose(w->f);
    w->f = NULL;
}

int wav_read_frames(WavDec *w, int16_t *out, int want_frames)
{
    if (!w->f || w->pos >= w->data_end) return 0;

    int bytes_per_src_frame = w->channels * (w->bits_per_sample / 8);
    uint32_t avail_bytes = w->data_end - w->pos;
    int avail_frames = (int)(avail_bytes / (uint32_t)bytes_per_src_frame);
    int n = want_frames < avail_frames ? want_frames : avail_frames;
    if (n <= 0) return 0;

    for (int i = 0; i < n; i++) {
        int16_t l, r;
        if (w->bits_per_sample == 16) {
            int16_t s[2];
            size_t got = fread(s, 2, (size_t)w->channels, w->f);
            if (got != (size_t)w->channels) { n = i; break; }
            l = s[0];
            r = (w->channels == 2) ? s[1] : s[0];
        } else { /* 8-bit PCM is unsigned, 0..255 centred on 128 */
            uint8_t s[2];
            size_t got = fread(s, 1, (size_t)w->channels, w->f);
            if (got != (size_t)w->channels) { n = i; break; }
            l = (int16_t)(((int)s[0] - 128) << 8);
            r = (w->channels == 2) ? (int16_t)(((int)s[1] - 128) << 8) : l;
        }
        out[i * 2 + 0] = l;
        out[i * 2 + 1] = r;
    }
    w->pos = (uint32_t)ftell(w->f);
    return n;
}
