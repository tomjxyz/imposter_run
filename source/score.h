// Save file location
#include "gcbool.h"
#define SAVE_FN "sd:/imposter.sav"

// String representations of score and hs
extern char scoreS[18];
extern char scoreHS[18];

// Creates save file if it does not exist
void initSave();

// Saves a new high score to the file
void saveHS(unsigned int hs);

// Updates the global variables that keep track of score and hs
void updateScore(int score, int hs);

// Increments score by 1
void incrementScore();

// Resets score
void resetScore();

// Checks if score is > high score and updates relevant info if so
void checkHS();
