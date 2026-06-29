#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_dma.h>
#include <gba_types.h>
#include "image.h"

// Provides Maxmod functions for playing music and sound effects on the GBA.
#include <maxmod.h>

// Defines the identifiers generated for each track included in the soundbank,
// such as MOD_BLOODYTEARS.
#include "soundbank.h"

// References the binary soundbank embedded in the ROM by the build system.
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

    // Registers Maxmod's VBlank interrupt handler.
    // Maxmod uses it to keep audio playback synchronized.
    irqSet(IRQ_VBLANK, mmVBlank);

    // Enables the VBlank interrupt required by Maxmod.
    irqEnable(IRQ_VBLANK);

    SetMode(MODE_4 | BG2_ON);

    // Initializes Maxmod using the embedded soundbank and reserves
    // eight audio channels for music and sound effects.
    mmInitDefault((mm_addr)soundbank_bin, 8);

    setBlackPalette();

    dmaCopy(imageBitmap, (void*)VRAM, imageBitmapLen);

    fadeInPalette(FPS_FADE_IN);

    // Starts the Bloody Tears music module and plays it continuously.
    mmStart(MOD_BLOODYTEARS, MM_PLAY_LOOP);

    while (1) {
        VBlankIntrWait();

        // Updates Maxmod once per frame so the music continues playing.
        mmFrame();
    }
}
