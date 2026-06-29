// Enables simple text output on the GBA screen.
#include <gba_console.h>

// Provides interrupt functions such as irqInit() and irqEnable().
#include <gba_interrupt.h>

// Provides system/BIOS calls such as VBlankIntrWait().
#include <gba_systemcalls.h>

// Provides standard input/output functions.
// iprintf() is used to print text to the GBA console.
#include <stdio.h>

int main(void) {

   // Initialize the interrupt system.
   // This must be done before enabling interrupts.
   irqInit();

   // Enable the VBlank interrupt.
   // This is required for VBlankIntrWait() to work correctly.
   irqEnable(IRQ_VBLANK);

   // Initialize a basic text console on the GBA screen.
   // After this, iprintf() can print text.
   consoleDemoInit();

   // Move the cursor to row 10, column 10 and print text.
   iprintf("\x1b[10;10HHello World!\n");

   // Main loop.
   // GBA programs usually run forever instead of ending.
   while (1) {

      // Wait for the next vertical blank period.
      // This keeps the program synchronized with the screen refresh.
      VBlankIntrWait();
   }
}
