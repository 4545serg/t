/* PROJECT NOISE - ARM7 side.
 *
 * This CPU does not know about songs, folders, or the UI. It starts the
 * maxmod7 mixer core (which the ARM9 side feeds PCM into over FIFO for
 * streamed MP3/WAV playback) and answers the standard libnds system
 * requests (touch screen, power, real-time clock). All player logic
 * lives on ARM9 in ../../arm9/source/.
 */
#include <nds.h>
#include <maxmod7.h>

void VblankHandler(void) {
    Touch touch;
    touchRead(&touch);
}

int main(void) {
    readUserSettings();
    irqInit();
    irqSet(IRQ_VBLANK, VblankHandler);
    fifoInit();

    mmInstall(FIFO_MAXMOD);

    irqEnable(IRQ_VBLANK);
    keysSetRepeat(25, 5);

    while (1) {
        swiWaitForVBlank();
    }
    return 0;
}
