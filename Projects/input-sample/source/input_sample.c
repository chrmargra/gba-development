#include <gba_console.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_input.h>
#include <maxmod.h>
#include <stdio.h>

#include "soundbank.h"

extern const unsigned char soundbank_bin[];

void printPressedButton(const char* buttonName) {
    iprintf("\x1b[8;3H                    ");
    iprintf("\x1b[8;3HPressed: %s", buttonName);
}

int main(void) {
    irqInit();
    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    mmInitDefault((mm_addr)soundbank_bin, 8);
    mmStart(MOD_BLOODYTEARS, MM_PLAY_LOOP);

    iprintf("\x1b[2;7HButton test");
    iprintf("\x1b[5;3HPress any GBA button");

    while (1) {
        VBlankIntrWait();
        mmFrame();

        scanKeys();

        u16 keys = keysDown();

        switch (keys) {
            case KEY_A:
                printPressedButton("A");
                break;
            case KEY_B:
                printPressedButton("B");
                break;
            case KEY_START:
                printPressedButton("START");
                break;
            case KEY_SELECT:
                printPressedButton("SELECT");
                break;
            case KEY_UP:
                printPressedButton("UP");
                break;
            case KEY_DOWN:
                printPressedButton("DOWN");
                break;
            case KEY_LEFT:
                printPressedButton("LEFT");
                break;
            case KEY_RIGHT:
                printPressedButton("RIGHT");
                break;
            case KEY_L:
                printPressedButton("L");
                break;
            case KEY_R:
                printPressedButton("R");
                break;
        }
    }
}
