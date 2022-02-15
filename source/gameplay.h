#include <grrlib.h>
#include <wiiuse/wpad.h>

#define EYES_COLOUR 0x95CADCFF
#define GRAVITY 1
#define JUMP_SPEED 15

extern int scrHeight;
extern int scrWidth;
extern bool dead;

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

void initGameplay();

void gameplay();

void shiftColour(bool forward);

void drawBlocks();

void drawCrewmate(Point pos, u32 colour, bool dead);

void resetBlocks(Block *blocks, int arrSize);

void resetGame();
