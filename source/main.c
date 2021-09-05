#include <stdio.h>
#include <wiiuse/wpad.h>
#include <grrlib.h>

#define BODY_COLOUR 0xC71012FF
#define EYES_COLOUR 0x95CADCFF

int scrWidth;
int scrHeight;

typedef struct {
	int x;
	int y;
} Point;

void gameplay() {
	
    GRRLIB_FillScreen(0x000000FF);    // Clear the screen
	// Test character
	Point sus = {scrWidth/5, (scrHeight/5)*3};
	if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
		sus.y = (scrHeight/5)*3-40;

	GRRLIB_Rectangle(sus.x, sus.y, scrHeight/12, scrHeight/6, BODY_COLOUR, true);
	GRRLIB_Rectangle(sus.x+5, sus.y+8, scrHeight/12, scrHeight/16, EYES_COLOUR, true);




	// Floor

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
