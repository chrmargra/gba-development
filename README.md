# GBA Development

Personal repository for learning and experimenting with **Game Boy Advance** development using **devkitPro**, **devkitARM**, **libgba**, and **mGBA**.

Some of the initial learning notes and examples in this repository are based on the tutorial [Tutorial de programación para Game Boy Advance](https://www.migsantiago.com/index.php/tutoriales/tutorial-de-programacion-para-game-boy-advance) by Mig Santiago, adapted to a modern devkitPro/devkitARM setup.


This repository contains setup notes, installation references, BIOS-related files for local emulator usage, and small GBA example projects created while learning how to build `.gba` ROMs from C source code.

## Table of contents

- [About](#about)
- [Repository structure](#repository-structure)
- [Tools used](#tools-used)
- [Requirements](#requirements)
- [Environment variables](#environment-variables)
- [Creating a new GBA project](#creating-a-new-gba-project)
- [Basic Hello World example](#basic-hello-world-example)
- [Compiling a project](#compiling-a-project)
- [Running the ROM](#running-the-rom)
- [Changing the ROM name](#changing-the-rom-name)
- [Common issues](#common-issues)
  - [`undefined reference to 'main'`](#undefined-reference-to-main)
  - [`strlen` implicit declaration error](#strlen-implicit-declaration-error)
  - [`shell-init: error retrieving current directory`](#shell-init-error-retrieving-current-directory)
  - [Editor shows red errors on GBA includes](#editor-shows-red-errors-on-gba-includes)
- [Useful commands](#useful-commands)
- [Notes](#notes)
- [License](#license)

## About

The main goal of this repository is to document and organize my Game Boy Advance development environment.

It is intended as a personal learning workspace for:

- Setting up devkitPro on macOS.
- Creating new GBA projects from the official devkitPro template.
- Compiling `.gba` ROM files with `make`.
- Running and testing ROMs using mGBA.
- Keeping small examples and experiments organized.

## Repository structure

```text
gba-development/
├── Bios/
│   └── Local BIOS-related files for emulator usage
│
├── GBAProjects/
│   └── GBA projects and example ROMs
│
├── Installation/
│   └── Notes and documentation about the setup process
│
└── README.md
```

## Tools used

This development environment uses:

- **devkitPro**: homebrew development toolchain manager.
- **devkitARM**: ARM compiler/toolchain used for GBA development.
- **libgba**: GBA development library.
- **make**: build system used by the devkitPro templates.
- **mGBA**: emulator used to test compiled `.gba` ROM files.

## Requirements


This setup is currently based on macOS.

For a more detailed step-by-step installation guide, check the `.docx` document included in the `Installation/` folder. It contains additional notes about installing devkitPro, configuring the terminal environment, creating the first project, compiling the ROM, and running it with mGBA.

Required tools:

```bash
xcode-select --install
```

devkitPro pacman must also be installed from the official devkitPro installer.

After installing devkitPro pacman, the GBA development packages can be installed with:

```bash
sudo dkp-pacman -Syu
sudo dkp-pacman -S gba-dev gba-examples
```

## Environment variables

The following environment variables should be available in the terminal:

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=${DEVKITPRO}/devkitARM
export PATH=${DEVKITPRO}/pacman/bin:${DEVKITPRO}/tools/bin:${DEVKITARM}/bin:$PATH
```

On macOS using `zsh`, these can be added to:

```bash
~/.zshrc
```

After editing the file, reload the terminal configuration:

```bash
source ~/.zshrc
```

To verify the setup:

```bash
echo $DEVKITPRO
echo $DEVKITARM
which dkp-pacman
which arm-none-eabi-gcc
arm-none-eabi-gcc --version
```

Expected paths:

```text
/opt/devkitpro
/opt/devkitpro/devkitARM
```

## Creating a new GBA project

The easiest way to start a new project is to copy the official devkitPro GBA template.

```bash
cd GBAProjects
cp -r /opt/devkitpro/examples/gba/template hello-world
cd hello-world
```

The typical project structure is:

```text
hello-world/
├── Makefile
└── source/
    └── main.c
```

Important notes:

- The `Makefile` must stay in the root of the project.
- Source files should be placed inside the `source/` folder.
- The project must contain a C file with an `int main(void)` function.
- The `make` command should be executed from the project root, not from inside `source/`.

## Basic Hello World example

Example `source/main.c`:

```c
// Enables simple text output on the GBA screen.
#include <gba_console.h>

// Provides interrupt functions such as irqInit() and irqEnable().
#include <gba_interrupt.h>

// Provides system/BIOS calls such as VBlankIntrWait().
#include <gba_systemcalls.h>

// Provides standard input/output functions.
// iprintf() is used to print text to the GBA console.
#include <stdio.h>

// Program entry point.
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

    // Move the cursor to row 9, column 6 and print text.
    iprintf("\x1b[9;6HHello World!");

    // Move the cursor to row 10, column 6 and print text.
    iprintf("\x1b[10;6HFirst ROM compiled");

    // Move the cursor to row 11, column 6 and print text.
    iprintf("\x1b[11;6Husing devkitARM!");

    // Main loop.
    // GBA programs usually run forever instead of ending.
    while (1) {

        // Wait for the next vertical blank period.
        // This keeps the program synchronized with the screen refresh.
        VBlankIntrWait();
    }
}
```

## Compiling a project

From the project root, run:

```bash
make clean
make
```

If the build succeeds, a `.gba` ROM file will be generated in the project folder.

Example:

```text
hello-world.gba
```

## Running the ROM

To open the generated ROM with mGBA:

```bash
open -a mGBA ./*.gba
```

Or open the `.gba` file manually from Finder.

## Changing the ROM name

By default, the devkitPro template usually names the ROM after the project folder.

The output name is controlled from the `Makefile` using the `TARGET` variable.

Example:

```makefile
TARGET := hello-world
```

This generates:

```text
hello-world.gba
```

To use a different ROM name, change `TARGET`:

```makefile
TARGET := my-first-gba-rom
```

Then rebuild:

```bash
make clean
make
```

The generated ROM will be:

```text
my-first-gba-rom.gba
```

Do not include `.gba` in the `TARGET` value.

Correct:

```makefile
TARGET := my-first-gba-rom
```

Incorrect:

```makefile
TARGET := my-first-gba-rom.gba
```

## Common issues

### `undefined reference to 'main'`

This usually means the compiler did not find the `main()` function.

Common causes:

- The `.c` file is not inside the `source/` folder.
- The file does not contain `int main(void)`.
- The project is being compiled from the wrong folder.

Correct structure:

```text
project-name/
├── Makefile
└── source/
    └── main.c
```

Compile from:

```bash
project-name/
```

not from:

```bash
project-name/source/
```

### `strlen` implicit declaration error

If using `strlen()`, include:

```c
#include <string.h>
```

Example:

```c
#include <string.h>

int length = strlen("Hello World!");
```

### `shell-init: error retrieving current directory`

Example error:

```text
shell-init: error retrieving current directory: getcwd: cannot access parent directories: Bad file descriptor
```

This is usually related to the terminal session or current folder state, not directly to the GBA compiler.

Possible fixes:

```bash
cd ~
cd path/to/project
make clean
make
```

If it continues, close the terminal window and open a new one.

### Editor shows red errors on GBA includes

Some editors may underline these includes:

```c
#include <gba_console.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
```

This does not always mean the project is broken.

If `make` works, the project is compiling correctly. The editor simply does not know where the devkitPro headers are located.

## Useful commands

Check devkitPro installation:

```bash
ls /opt/devkitpro
```

Check devkitARM:

```bash
arm-none-eabi-gcc --version
```

List GBA examples:

```bash
ls /opt/devkitpro/examples/gba
```

Create a new project from the template:

```bash
cp -r /opt/devkitpro/examples/gba/template my-project
```

Compile:

```bash
make clean
make
```

Find generated ROMs:

```bash
find . -name "*.gba"
```

Run with mGBA:

```bash
open -a mGBA ./*.gba
```

## Notes

This repository is mainly intended for personal learning and experimentation.

The examples here are simple and focused on understanding the basics of GBA development before moving on to more advanced topics such as:

- Button input.
- Sprites.
- Backgrounds.
- Tiles.
- Palettes.
- Sound.
- Game loops.
- Real hardware testing.

## License

This repository is for personal learning purposes.

If third-party templates, examples, or assets are added, their original licenses should be respected.
