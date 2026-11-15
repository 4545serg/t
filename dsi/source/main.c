/* PROJECT NOISE - DSi port, phase 1 (BlocksDS build).
 *
 * Bare-bones but functional: browse SD:/Music on the bottom screen with
 * the d-pad, play the selected MP3/WAV with A, stop with B, exit with
 * START. This intentionally does NOT yet carry over the cover-flow top
 * screen, ambient glow, queue, favorites, or search from the 3DS UI -
 * those are phase 2+ (see PORT_PLAN.md). The goal of this phase is a
 * stable playback core to build the real interface on top of.
 *
 * NEEDS HARDWARE VERIFICATION - written and reasoned through against
 * BlocksDS's published docs; no devkitARM/BlocksDS toolchain was
 * available to compile or run it in this environment.
 */
#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>

#include "noise.h"
#include "browser.h"
#include "player.h"
#include "audio_stream.h"

static DirList dir;
static int sel = 0;
static int playing_index = -1;

static void draw_list(void)
{
    consoleClear();
    /* BlocksDS's picolibc doesn't provide iprintf() - plain printf() is
     * both the integer and float version depending on a build-time
     * libc choice; either way it's the right call here, not iprintf(). */
    printf("%s\n", APP_NAME);
    printf("%s\n\n", dir.dir);

    int rows = 20;
    int top = sel - rows / 2;
    if (top < 0) top = 0;
    if (top > dir.n - rows) top = dir.n - rows;
    if (top < 0) top = 0;

    for (int i = top; i < dir.n && i < top + rows; i++) {
        Entry *e = &dir.e[i];
        char mark = (i == sel) ? '>' : ' ';
        char note = (i == playing_index) ? '*' : (e->is_dir ? '/' : ' ');
        printf("%c%c%-27.27s\n", mark, note, e->name);
    }
}

static void play_selected(void)
{
    Entry *e = &dir.e[sel];
    if (e->is_dir) return;

    char path[768];
    snprintf(path, sizeof path, "%s%s", dir.dir, e->name);

    audio_stream_stop();
    if (player_open(path)) {
        audio_stream_start(player_sample_rate());
        playing_index = sel;
    } else {
        playing_index = -1;
        printf("\n  Could not open (unsupported or FLAC - not yet ported)\n");
    }
}

static void open_selected(void)
{
    Entry *e = &dir.e[sel];
    if (!e->is_dir) { play_selected(); return; }

    char path[768];
    if (strcmp(e->name, "..") == 0) {
        char tmp[768];
        snprintf(tmp, sizeof tmp, "%s", dir.dir);
        size_t len = strlen(tmp);
        if (len > 1 && tmp[len - 1] == '/') tmp[len - 1] = 0;
        char *slash = strrchr(tmp, '/');
        if (slash) slash[1] = 0; else tmp[0] = 0;
        snprintf(path, sizeof path, "%s", tmp);
    } else {
        snprintf(path, sizeof path, "%s%s/", dir.dir, e->name);
    }

    if (dir_load(&dir, path)) { sel = 0; playing_index = -1; }
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleDemoInit(); /* bottom-screen text console for this phase's browser UI */

    if (!fatInitDefault()) {
        printf("SD card / FAT init failed.\n");
        while (1) swiWaitForVBlank();
    }

    audio_stream_init(); /* soundEnable() + mmInitNoSoundbank() - see audio_stream.c */

    memset(&dir, 0, sizeof dir);
    if (!dir_load(&dir, MUSIC_ROOT)) {
        printf("Could not open %s\n", MUSIC_ROOT);
        printf("Create that folder on the SD card and restart.\n");
        while (1) swiWaitForVBlank();
    }

    draw_list();

    while (1) {
        swiWaitForVBlank();
        scanKeys();
        int down = keysDown();

        if (down & KEY_UP)    { if (sel > 0) sel--; draw_list(); }
        if (down & KEY_DOWN)  { if (sel < dir.n - 1) sel++; draw_list(); }
        if (down & KEY_A)     { open_selected(); draw_list(); }
        if (down & KEY_B)     { audio_stream_stop(); playing_index = -1; draw_list(); }
        if (down & KEY_START) break;

        if (playing_index >= 0 && !audio_stream_is_playing()) {
            int next = playing_index;
            do { next++; } while (next < dir.n && dir.e[next].is_dir);
            if (next < dir.n) { sel = next; play_selected(); draw_list(); }
            else { playing_index = -1; draw_list(); }
        }
    }

    audio_stream_stop();
    return 0;
}
