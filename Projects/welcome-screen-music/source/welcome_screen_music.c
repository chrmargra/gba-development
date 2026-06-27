#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_dma.h>
#include <gba_types.h>
#include "image.h"
#include <maxmod.h>
#include "soundbank.h"

extern const unsigned char soundbank_bin[];

#define PALETTE_COLORS 256
#define FPS_FADE_IN 15

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

int main(void) {
    irqInit();
    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);

    SetMode(MODE_4 | BG2_ON);

    mmInitDefault((mm_addr)soundbank_bin, 8);

    setBlackPalette();

    dmaCopy(imageBitmap, (void*)VRAM, imageBitmapLen);

    fadeInPalette(FPS_FADE_IN);

    mmStart(MOD_BLOODYTEARS, MM_PLAY_LOOP);

    while (1) {
        VBlankIntrWait();
        mmFrame();
    }
}
