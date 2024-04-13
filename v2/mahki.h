#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


const int num_skins = 3;

const char *colors[4] = {"\033[0;31m", "\033[0;33m", "\033[0;35m", "\033[0;36m"};


const char *skins[3] = {"\u25A0 ", "\u2588\u2588", "\u263B "};


struct game {
	struct field *field;
	int score;
	int round_count;
	int score_round;
	int num_blocks;
	int threshhold;
	int shuffled;
	char *skin;
	char **colors;
};


struct game * init_game(int block_count, int skin_option);
void print_game(struct game *g);
