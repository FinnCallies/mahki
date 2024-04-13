#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "field.h"
#include "mahki.h"



// timer 7m

struct game * init_game(int block_count, int skin_option)
{
	struct game *ret = malloc(sizeof(struct game));

	if (skin_option >= num_skins){
		printf("ERROR: not a valid skin option. Default skin set.\n");
		ret->skin = skins[1];
	} else {
		ret->skin = skins[skin_option];
	}
	
	ret->colors = calloc(block_count, sizeof(char *));
	for (int i = 0; i < block_count; i++) 
		ret->colors[i] = colors[i];
	
	ret->score = 0;
	ret->score_round = 0;
	ret->shuffled = 0;
	ret->threshhold = 0;
	ret->round_count = 0;
	ret->num_blocks = block_count;
	
	ret->field = create_field(20, 10);
	if (ret->field == NULL) {
		printf("ERROR: (init_game(...)): Unable to create field\n");
		free(NULL);
		return NULL;
	}
	
	
	
	
	
	return ret;
}


void print_game(struct game *g)
{
	print_field(g->field, g->skin, g->threshhold);
}


void init_field(struct field* f, int num)
{
    int* blocks = malloc(num);

    int block_per_num;

    if ((f->height * f->width) % num == 0)
        block_per_num = (f->height * f->width) / num;
    else
        block_per_num = (f->height * f->width + (num - (f->height * f->width % num))) / num;
    
    for (int i = 0; i < num; i++)
        blocks[i] = block_per_num;
    
    for (int i = 0; i < (f->height * f->width); i++)
    {
        int r = rand() % num;
        while (blocks[r] == 0)
            r = rand() % num;

        f->data[i] = r + 1;
        blocks[r]--;
    }
}


int main(int argc, char *argv[])
{
	int skin = 1;
	if (argc > 1) 
		skin = atoi(argv[1]);
	struct game *game = init_game(4, skin);

	init_field(game->field, game->num_blocks);

	print_game(game);
}
