#include <gba_video.h>
#include <gba_dma.h>
#include "soundbank.h"
#include <gba_console.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <maxmod.h>
#include <stdio.h>
#include "image.h"

#define PALETTE_COLORS 256
#define FPS_FADE_IN 15

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
    {"Contra Ending", MOD_CONTRA_ED}
};

const int songCount = sizeof(songs) / sizeof(SongEntry);

int selected = 0;
int currentPlaying = -1;

typedef enum {
    SCREEN_TITLE,
    SCREEN_MUSIC_MENU
} ScreenState;

ScreenState screenState = SCREEN_TITLE;

u16 fadePalette[PALETTE_COLORS];

void setBlackPalette(void) {
    for (int i = 0; i < PALETTE_COLORS; i++) {
        fadePalette[i] = 0;
    }
    dmaCopy(fadePalette, BG_PALETTE, PALETTE_COLORS * sizeof(u16));
}

void fadeInPalette(int frames) {
    for (int step = 0; step <= frames; step++) {
        for (int i = 0; i < PALETTE_COLORS; i++) {
            u16 color = imagePal[i];
            int red = color & 0x1F;
            int green = (color >> 5) & 0x1F;
            int blue = (color >> 10) & 0x1F;
            red = (red * step) / frames;
            green = (green * step) / frames;
            blue = (blue * step) / frames;
            fadePalette[i] = red | (green << 5) | (blue << 10);
        }

        VBlankIntrWait();

        dmaCopy(fadePalette, BG_PALETTE, PALETTE_COLORS * sizeof(u16));
    }
}

void showTitleScreen(void) {
    SetMode(MODE_4 | BG2_ON);
    setBlackPalette();
    dmaCopy(imageBitmap, (void*)VRAM, imageBitmapLen);
    fadeInPalette(FPS_FADE_IN);
    mmStart(MOD_CONTRA_ED, MM_PLAY_LOOP);
}

void drawMenu(void) {
    iprintf("\x1b[2J\x1b[H");
    iprintf("\x1b[2;1HSelect a song:");

    for (int i = 0; i < songCount; i++) {
        char marker = (i == selected) ? '>' : ' ';
        iprintf("\x1b[%d;1H%c %-20.20s", 4 + i, marker, songs[i].name);
    }

    if (currentPlaying >= 0) {
        iprintf("\x1b[%d;1HNow playing: %-20.20s", 5 + songCount, songs[currentPlaying].name);
    } else {
        iprintf("\x1b[%d;1HNow playing: (none)      ", 5 + songCount);
    }
}

void enterMusicMenu(void) {
    consoleDemoInit();
    drawMenu();
    screenState = SCREEN_MUSIC_MENU;
}

int main(void) {
    irqInit();
    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);

    mmInitDefault((mm_addr)soundbank_bin, 16);

    showTitleScreen();

    while (1) {
        VBlankIntrWait();
        mmFrame();
        scanKeys();
        u16 keys = keysDown();

        if (screenState == SCREEN_TITLE) {
            if (keys & KEY_START) {
                enterMusicMenu();
                mmStop();
            }

        } else if (screenState == SCREEN_MUSIC_MENU) {
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
}
