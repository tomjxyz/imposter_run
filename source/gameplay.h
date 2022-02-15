#include <grrlib.h>
#include <time.h>
#include <wiiuse/wpad.h>

#define EYES_COLOUR 0x95CADCFF
#define GRAVITY 1
#define JUMP_SPEED 15
#define MAX_OFFSET 30

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
    int offset;
} Block;

void initGameplay();

void gameplay();

void shiftColour();

void drawBlocks();

void drawCrewmate(Point pos, u32 colour, bool dead);

void resetBlocks(Block *blocks, int arrSize);

void randomOffset(Block *b);

void resetGame();
