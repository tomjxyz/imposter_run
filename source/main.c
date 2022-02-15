#include <dirent.h>
#include <fat.h>
#include <grrlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "font131.h"
#include "gameplay.h"
#include "score.h"

// Main entry point
int main() {
    printf("game start");
    if (!fatInitDefault()) {
        printf("Could not init FAT fs!");
    } else {
        printf("Initialised FAT fs");
    }
    initSave();

    // Initialise graphics library
    GRRLIB_Init();
    // Init gameplay loop
    initGameplay();

    // Initialise WiiMotes
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    // Game loop
    while (1) {
        // Scan WiiMotes
        WPAD_ScanPads();

        // Get buttons pressed py p1
        u32 pressed = WPAD_ButtonsDown(0);
        // If home pressed, exit
        if (pressed & WPAD_BUTTON_HOME)
            break;
        if (pressed & WPAD_BUTTON_PLUS) {
            shiftColour(true);
        }
        if (pressed & WPAD_BUTTON_MINUS) {
            shiftColour(false);
        }

        // Reset game after death
        if (pressed & WPAD_BUTTON_B && dead) {
            resetGame();
        }

        // ------------ Place drawing code here -------------------
        gameplay();

        GRRLIB_Render(); // Render frame buffer to tv
    }

    fatUnmount(0);
    GRRLIB_Exit(); // Clear memory used by graphics lib
    return 0;      // Return no error
}
