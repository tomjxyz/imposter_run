#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "score.h"

// Global score and hs
int score = 0;
int hs = 0;

void initSave() {
    // Check if save exists if not create it
    if (access(SAVE_FN, F_OK) < 0) {
        printf("Could not open / create save file, creating it.");
        saveHS(0);
        updateScore(0, 0);
    } else {
        // Load score from file
        FILE *saveFile = fopen(SAVE_FN, "rb");
        printf("Opened save file at %p", saveFile);

        unsigned int *hs_temp = malloc(sizeof(unsigned int));
        fread(hs_temp, sizeof(unsigned int), 1, saveFile);
        updateScore(0, *hs_temp);
        printf("Loaded HS: %d", hs);

        free(hs_temp);
        fclose(saveFile);
    }
}

void saveHS(unsigned int hs) {
    FILE *saveFile = fopen(SAVE_FN, "wb");
    if (saveFile != NULL)
        printf("Opened save file for writing at %p", saveFile);
    if (fwrite(&score, sizeof(unsigned int), 1, saveFile) == 0)
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

void incrementScore() { updateScore(++score, hs); }

void resetScore() { updateScore(0, hs); }

void checkHS() {
    if (score > hs) {
        updateScore(score, score);
        saveHS(hs);
    }
}
