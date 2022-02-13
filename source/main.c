#include <dirent.h>
#include <fat.h>
#include <grrlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "font131.h"

#define BODY_COLOUR 0xC71012FF
#define EYES_COLOUR 0x95CADCFF
#define GRAVITY 1
#define JUMP_SPEED 15
#define SAVE_FN "sd:/imposter.sav"

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int width;
    int height;
    int xpos;
    int ypos;
    int speed;
} Block;

int scrWidth;
int scrHeight;

// Font
GRRLIB_bytemapFont *font;
int score = 0;
int hs = 0;
char scoreS[18];
char scoreHS[18];

// For blocks to jump over
Block blocks[10];
bool pastBlock;

// Properties for main imposter
Point imposterPos;
int velocity;
bool dead = false;

void updateScore(int score, int hs);
void setSave(uint score);

void resetBlocks(Block *blocks, int arrSize) {
    for (int i = 0; i < arrSize; i++) {
        blocks[i].width = scrHeight / 16;
        blocks[i].height = scrHeight / 8;
        blocks[i].speed = 5;
        blocks[i].xpos = scrWidth + blocks[i].width +
                         ((scrWidth + blocks[i].width) / arrSize - 1) * i;
        blocks[i].ypos = ((scrHeight / 6) * 5) - blocks[i].height;
    }
}

// Draw character
void drawCrewmate(Point pos, u32 colour, bool dead) {
    int bodyW = scrHeight / 12;
    int bodyH = scrHeight / 6;

    if (!dead) {
        // Body
        GRRLIB_Rectangle(pos.x, pos.y, bodyW, bodyH, colour, true);
        // Eye
        GRRLIB_Rectangle(pos.x + 5, pos.y + 8, bodyW, scrHeight / 16,
                         EYES_COLOUR, true);
        // Leg gap
        GRRLIB_Rectangle(pos.x + (bodyW / 2) - 2, pos.y + (bodyH - 20), 5, 20,
                         0x000000FF, true);
    } else {
        // Body
        GRRLIB_Rectangle(pos.x, pos.y + bodyH / 2, bodyW, bodyH / 2, colour,
                         true);
        // Bone
        GRRLIB_Rectangle(pos.x + (bodyW / 2) - 4, pos.y + (bodyH / 2) - 20, 8,
                         20, 0xFFFFFFFF, true);
        GRRLIB_Circle(pos.x + (bodyW / 2), pos.y + (bodyH / 2) - 20, 8,
                      0xFFFFFFFF, true);
        // Leg gap
        GRRLIB_Rectangle(pos.x + (bodyW / 2) - 2, pos.y + ((bodyH / 2) + 20), 5,
                         20, 0x000000FF, true);
    }
}

void drawBlocks() {
    // Blocks to jump over
    for (int i = 0; i < 4; i++) {
        GRRLIB_Rectangle(blocks[i].xpos, blocks[i].ypos, blocks[i].width,
                         blocks[i].height, 0x969696FF, true);

        // If player hits a block
        if (GRRLIB_RectOnRect(imposterPos.x, imposterPos.y, scrHeight / 12,
                              scrHeight / 6, blocks[i].xpos, blocks[i].ypos,
                              blocks[i].width, blocks[i].height)) {
            dead = true;
        }

        if (!dead) {
            if (blocks[i].xpos < 0 - blocks[i].width) {

                blocks[i].xpos = scrWidth;
                pastBlock = true;
            } else
                blocks[i].xpos -= blocks[i].speed;
        }
        //    printf("pastBlock: %d", pastBlock);

        if (blocks[i].xpos < imposterPos.x && pastBlock) {
            updateScore(++score, hs);
            printf("Score: %d", score);
            pastBlock = false;
        }
    }
}

void gameplay() {
    GRRLIB_FillScreen(0x000000FF); // Clear the screen

    GRRLIB_PrintBMF(10, 10, font, scoreS);
    GRRLIB_PrintBMF(10, 32, font, scoreHS);

    // If touching floor
    if (imposterPos.y + scrHeight / 6 >= (scrHeight / 6) * 5) {
        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A && !dead)
            velocity = -JUMP_SPEED; // Add upwards velocity when A pressed and
                                    // still alive
        else
            velocity = 0;
    } else { // Add gravity to pull back down
        velocity += GRAVITY;
    }

    // Move main character if jumping
    imposterPos.y += velocity;
    // Draw main imposter
    drawCrewmate(imposterPos, BODY_COLOUR, dead);

    drawBlocks();

    // Floor
    GRRLIB_Rectangle(0, (scrHeight / 6) * 5, scrWidth, (scrHeight / 6) * 5,
                     0xBABABAFF, true);
}

void initSave() {
    // Check if save exists if not create it
    if (access(SAVE_FN, F_OK) < 0) {
        printf("Could not open / create save file, creating it.");
        setSave(0);
        updateScore(0, 0);
    } else {
        // Load score from file
        FILE *saveFile = fopen(SAVE_FN, "rb");
        printf("Opened save file at %p", saveFile);
        uint *hs_temp = malloc(sizeof(uint));
        fread(hs_temp, sizeof(uint), 1, saveFile);
        updateScore(0, *hs_temp);
        printf("Loaded HS: %d", hs);
        free(hs_temp);
        fclose(saveFile);
    }
}

void setSave(uint score) {
    FILE *saveFile = fopen(SAVE_FN, "wb");
    if (saveFile != NULL)
        printf("Opened save file for writing at %p", saveFile);
    if (fwrite(&score, sizeof(uint), 1, saveFile) == 0)
        printf("Error writing to save file.");
    else
        printf("Wrote score %d to save file", score);

    fclose(saveFile);
}

void updateScore(int new_score, int new_hs) {
    score = new_score;
    sprintf(scoreS, "SCORE %d", score);

    hs = new_hs;
    sprintf(scoreHS, "BEST  %d", hs);
}

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

    scrWidth = rmode->fbWidth;
    scrHeight = rmode->efbHeight;
    // TODO: Find out why assigning this on global declaration dosent work
    pastBlock = true;

    // Initialise WiiMotes
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    imposterPos.x = scrWidth / 5;
    imposterPos.y = (scrHeight / 5) * 3;
    // Initial blocks
    resetBlocks(blocks, 4);

    // Load font
    font = GRRLIB_LoadBMF(font131);

    // Game loop
    while (1) {
        // Scan WiiMotes
        WPAD_ScanPads();

        // Get buttons pressed py p1
        u32 pressed = WPAD_ButtonsDown(0);
        // If home pressed, exit
        if (pressed & WPAD_BUTTON_HOME)
            break;
        // Reset game after death
        if (pressed & WPAD_BUTTON_B && dead) {
            printf("HS pre death: %d", hs);
            // Make player alive again
            dead = !dead;

            resetBlocks(blocks, 4);
            // Set hs
            if (score > hs) {
                updateScore(score, score);
                setSave(hs);
            }
            printf("HS: %d", hs);
            // Reset score
            updateScore(0, hs);
            // Reset blocks
            pastBlock = true;
        }

        // ------------ Place drawing code here -------------------
        gameplay();

        GRRLIB_Render(); // Render frame buffer to tv
    }

    fatUnmount(0);
    GRRLIB_Exit(); // Clear memory used by graphics lib
    return 0;      // Return no error
}
