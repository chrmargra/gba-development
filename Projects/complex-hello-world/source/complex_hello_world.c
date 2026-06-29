#include <gba_console.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <stdio.h>

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
   // \x1b is the ESC character used for ANSI escape sequences.
   // [%d;%dH means: move cursor to row;column.
   // %s means: print the text string.
   iprintf("\x1b[%d;%dH%s", row, column, text);
}

int main(void) {

   irqInit();

   irqEnable(IRQ_VBLANK);

   consoleDemoInit();

   // Print three centered lines of text.
   // The first parameter is the row number.
   // The second parameter is the text to print.
   printCentered(9, "Hello World!");
   printCentered(10, "First ROM compiled");
   printCentered(11, "using devkitARM!");

   while (1) {
      VBlankIntrWait();
   }
}
