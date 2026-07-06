#include "soundbank.h"
#include <gba_console.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <maxmod.h>
#include <stdio.h>

extern const unsigned char soundbank_bin[];

typedef struct {
  const char *name;
  int mod_id;
} SongEntry;

SongEntry songs[] = {
    {"Bloody Tears", MOD_BLOODYTEARS},
    {"Castlevania 1 STG-1", MOD_CAVE06},
    {"Castlevania 1 STG-6", MOD_CASTLE01},
    {"Castlevania Cave", MOD_CASTLEVA},
    {"FF6 OP Theme", MOD_FF3OPENINGTHEME},
    {"FF8 Breez", MOD_FF8BREEZ},
    {"Ninja Gaiden", MOD_NINJAGAIDENSHADOWTUNE},
};

const int songCount = sizeof(songs) / sizeof(SongEntry);

int selected = 0;
int currentPlaying = -1;

void drawMenu(void) {
    iprintf("\x1b[2J\x1b[H");
    iprintf("\x1b[2;3HSelect a song:");

    for (int i = 0; i < songCount; i++) {
        char marker = (i == selected) ? '>' : ' ';
        iprintf("\x1b[%d;3H%c %-20.20s", 4 + i, marker, songs[i].name);
    }

    if (currentPlaying >= 0) {
        iprintf("\x1b[%d;3HNow playing: %-20.20s", 5 + songCount, songs[currentPlaying].name);
    } else {
        iprintf("\x1b[%d;3HNow playing: (none)      ", 5 + songCount);
    }
}

int main(void) {
    irqInit();
    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    mmInitDefault((mm_addr)soundbank_bin, 8);
    drawMenu();

    while (1) {
        VBlankIntrWait();
        mmFrame();

        scanKeys();
        u16 keys = keysDown();

        if (keys & KEY_UP) {
            selected = (selected + songCount - 1) % songCount;
            drawMenu();
        }

        if (keys & KEY_DOWN) {
            selected = (selected + 1) % songCount;
            drawMenu();
        }

        if (keys & KEY_A) {
            mmStop();
            mmStart(songs[selected].mod_id, MM_PLAY_LOOP);
            currentPlaying = selected;
            drawMenu();
        }

        if (keys & KEY_B) {
            mmStop();
            currentPlaying = -1;
            drawMenu();
        }
    }
}
