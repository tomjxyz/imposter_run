#include "gameplay.h"
#include "font131.h"
#include "score.h"
// Globals

GRRLIB_bytemapFont *font;
int scrHeight;
int scrWidth;

// For blocks to jump over
Block blocks[10];
bool pastBlock = true;

// Properties for main imposter
Point imposterPos;
int velocity;
bool dead;
int colours[] = {0xC71111FF, 0x132FD2FF, 0x10802EFF,
                 0xEE55BAFF, 0xF17D0EFF, 0xF6F757FF};
int currentColour = 0;

// To display score
char scoreS[18];
char scoreHS[18];

// Functions
void initGameplay() {
    dead = false;
    scrWidth = rmode->fbWidth;
    scrHeight = rmode->efbHeight;

    font = GRRLIB_LoadBMF(font131);

    imposterPos.x = scrWidth / 5;
    imposterPos.y = (scrHeight / 5) * 3;

    resetBlocks(blocks, 4);
}

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

void shiftColour(bool forward) {
    if (forward) {
        ++currentColour;
        currentColour %= (sizeof(colours) / sizeof(colours[0]));
    } else {
        --currentColour;
        currentColour %= (sizeof(colours) / sizeof(colours[0]));
    }
}

void resetGame() {
    dead = !dead;
    resetBlocks(blocks, 4);
    checkHS();
    resetScore();
    pastBlock = true;
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
            incrementScore();
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
    drawCrewmate(imposterPos, colours[currentColour], dead);

    drawBlocks();

    // Floor
    GRRLIB_Rectangle(0, (scrHeight / 6) * 5, scrWidth, (scrHeight / 6) * 5,
                     0xBABABAFF, true);
}
