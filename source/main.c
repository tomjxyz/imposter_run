#include <stdio.h>
#include <wiiuse/wpad.h>

#include<grrlib.h>


// Main entry point
int main(int argc, char **argv) {

	// Initialise graphics library
	GRRLIB_Init();

	// Initialise WiiMotes
	WPAD_Init();

	// Game loop
	while(1) {

		// Scan WiiMotes
		WPAD_ScanPads();
		// Get buttons pressed py p1
		u32 pressed = WPAD_ButtonsDown(0);

		// If home pressed, exit
		if ( pressed & WPAD_BUTTON_HOME ) break;

		// ------------ Place drawing code here -------------------


		GRRLIB_Render(); // Render frame buffer to tv
	}

	GRRLIB_Exit();	// Clear memory used by graphics lib
	return 0;		// Return no error
}
