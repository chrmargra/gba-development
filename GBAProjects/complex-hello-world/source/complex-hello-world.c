// Enables text output on screen using libgba console functions like iprintf().
#include <gba_console.h>

// Provides video-related definitions and functions for the GBA.
#include <gba_video.h>

// Provides interrupt-related functions such as irqInit() and irqEnable().
#include <gba_interrupt.h>

// Provides BIOS/system calls such as VBlankIntrWait().
#include <gba_systemcalls.h>

// Provides input/key handling functions for reading GBA buttons.
// Not used yet in this example, but useful later.
#include <gba_input.h>

// Provides standard input/output functions.
// iprintf() depends on this.
#include <stdio.h>

// Provides general-purpose standard library functions.
// Not used yet in this example, but commonly included in templates.
#include <stdlib.h>

// Provides string utility functions such as strlen().
#include <string.h>

// Number of text columns available in the GBA console mode used by consoleDemoInit().
#define SCREEN_COLUMNS 30

// Prints text centered horizontally on a specific console row.
void printCentered(int row, const char* text) {

   // Calculate the starting column so the text appears centered.
   // strlen(text) gets the number of characters in the text.
   int column = (SCREEN_COLUMNS - strlen(text)) / 2;

   // If the text is longer than the screen width, avoid using a negative column.
   if (column < 0) {
      column = 0;
   }

   // Move the console cursor to the selected row and calculated column,
   // then print the text.
   //
   // \x1b is the ESC character used for ANSI escape sequences.
   // [%d;%dH means: move cursor to row;column.
   // %s means: print the text string.
   iprintf("\x1b[%d;%dH%s", row, column, text);
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {

   // Initialize the interrupt system.
   // This must be called before enabling or using interrupts.
   irqInit();

   // Enable the VBlank interrupt.
   // This is required so VBlankIntrWait() can work correctly.
   irqEnable(IRQ_VBLANK);

   // Initialize a simple text console on the GBA screen.
   // After this, iprintf() can print text to the screen.
   consoleDemoInit();

   // Print three centered lines of text.
   // The first parameter is the row number.
   // The second parameter is the text to print.
   printCentered(9, "Hello World!");
   printCentered(10, "First ROM compiled");
   printCentered(11, "using devkitARM!");

   // Main loop.
   // GBA programs should not finish immediately; they usually run forever.
   while (1) {

      // Wait for the next vertical blank period.
      // This keeps the program synchronized with the screen refresh.
      VBlankIntrWait();
   }
}
