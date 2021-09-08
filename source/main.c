#include <stdio.h>
#include <wiiuse/wpad.h>
#include <grrlib.h>

#define BODY_COLOUR 0xC71012FF
#define EYES_COLOUR 0x95CADCFF
#define GRAVITY 1
#define JUMP_SPEED 15

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	int width;
	int height;
	int xpos;
	int speed;
} Block; 

int scrWidth;
int scrHeight;

// For blocks to jump over
Block blocks[10];

// For character position and jump
Point imposterPos;
int velocity;

void resetBlocks(Block *blocks, int arrSize) {
	for (int i = 0; i < arrSize; i++) {
		blocks[i].width = scrHeight/16;
		blocks[i].height = scrHeight/8;
		blocks[i].speed = 5;
		blocks[i].xpos = scrWidth+blocks[i].width + ((scrWidth+blocks[i].width)/arrSize-1)*i;
	}
}

// Draw character
void drawCrewmate(Point pos, u32 colour, bool dead){
	int bodyW = scrHeight/12;
	int bodyH = scrHeight/6;

	if (!dead) {
		// Body
		GRRLIB_Rectangle(pos.x, pos.y, bodyW, bodyH, colour, true);
		// Eye
		GRRLIB_Rectangle(pos.x+5, pos.y+8, bodyW, scrHeight/16, EYES_COLOUR, true);
		// Leg gap
		GRRLIB_Rectangle(pos.x+(bodyW/2)-2, pos.y+(bodyH-20), 5, 20, 0x000000FF, true);
	} else {
		// Body
		GRRLIB_Rectangle(pos.x, pos.y+bodyH/2, bodyW, bodyH/2, colour, true);
		// Bone
		GRRLIB_Rectangle(pos.x+(bodyW/2)-4, pos.y+(bodyH/2)-20, 8, 20, 0xFFFFFFFF, true);
		GRRLIB_Circle(pos.x+(bodyW/2), pos.y+(bodyH/2)-20, 8, 0xFFFFFFFF, true);
		// Leg gap
		GRRLIB_Rectangle(pos.x+(bodyW/2)-2, pos.y+((bodyH/2)+20), 5, 20, 0x000000FF, true);
	}
}

void gameplay() {
    GRRLIB_FillScreen(0x000000FF);    // Clear the screen

	// Test character

	// If touching floor
	if (imposterPos.y+scrHeight/6 >= (scrHeight/6)*5) {
		if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
			velocity = -JUMP_SPEED; // Add upwards velocity when A pressed
		else
			velocity = 0;
	}
	else { // Add gravity to pull back down
		velocity += GRAVITY;
	}

	// Move main character if jumping
	imposterPos.y += velocity;
	// Draw main imposter
	drawCrewmate(imposterPos, BODY_COLOUR, false);

	// Floor
	GRRLIB_Rectangle(0, (scrHeight/6)*5, scrWidth, (scrHeight/6)*5, 0xBABABAFF, true);

	// Blocks to jump over
	for (int i = 0; i < 4; i++) {
		GRRLIB_Rectangle(blocks[i].xpos, ((scrHeight/6)*5)-blocks[i].height, blocks[i].width, blocks[i].height, 0xBABABAFF, true);
		if (blocks[i].xpos < 0-blocks[i].width)
			blocks[i].xpos = scrWidth;
		else
			blocks[i].xpos -= blocks[i].speed;
	}

}

// Main entry point
int main() {
	// Initialise graphics library
	GRRLIB_Init();

	scrWidth = rmode->fbWidth;
	scrHeight = rmode->efbHeight;

	// Initialise WiiMotes
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	
	imposterPos.x = scrWidth/5;
	imposterPos.y = (scrHeight/5)*3;
	// Initial blocks
	resetBlocks(blocks, 4);

	// Game loop
	while(1) {
		// Scan WiiMotes
		WPAD_ScanPads();

		// Get buttons pressed py p1
		u32 pressed = WPAD_ButtonsDown(0);
		// If home pressed, exit
		if (pressed & WPAD_BUTTON_HOME)
			break;

		// ------------ Place drawing code here -------------------
		gameplay();

		GRRLIB_Render(); // Render frame buffer to tv
	}

	GRRLIB_Exit();	// Clear memory used by graphics lib
	return 0;		// Return no error
}
