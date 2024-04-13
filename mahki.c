#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "field.h"
#include "mahki.h"



// timer 7m



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

int get_adjacent_same_val(struct field* f, struct coords* c, struct coords** ret)
{
    int count = 0;
    
    struct coords* tmp;
    if (ret != NULL) 
        tmp = *ret;
    else 
        tmp = calloc(4, sizeof(struct coords));

    struct coords* adjacent = calloc(4, sizeof(struct coords));
    int adjacent_num = get_adjacent(f, c, &adjacent);


    for (int i = 0; i < adjacent_num; i++) {
        if (get_coord(f, &adjacent[i]) == get_coord(f, c)) {
            tmp[count].x = adjacent[i].x;
            tmp[count].y = adjacent[i].y;
            count++;
        }
    }


    free(adjacent);
    if (ret == NULL)
        free(tmp);
    return count;
}

int possible_moves(struct field* f)
{
    int idx = 0;

    while (idx < (f->height * f->width)) {
        struct coords* current = malloc(sizeof(struct coords));
        idx2coords(f, idx, current);
        if (get_coord(f, current) > 0) {
            int temp = get_adjacent_same_val(f, current, NULL);
            if (temp > 0)
                return idx;
            if (current != NULL)
                destroy_coords(current);
        }
        idx++;
    }

    return -1;
}

int get_group(struct field* f, struct coords* c, struct coords** ret)
{
    struct coords* tmp = *ret;
    int count = 1;
    int len = 1;

    tmp[0].x = c->x;
    tmp[0].y = c->y;

    for (int i = 1; i < (f->height * f->width); i++) {
        tmp[i].x = -1;
        tmp[i].y = -1;
    }
    
    while (tmp[count - 1].x != -1 && tmp[count - 1].y != -1) {
        struct coords* neighbours = calloc(4, sizeof(struct coords));;
        int num = get_adjacent_same_val(f, &tmp[count - 1], &neighbours);


        /* printf("%d neighbours of (%d %d): ", num, tmp[count - 1].x, tmp[count - 1].y);
        for (int i = 0; i < num; i++)
            printf("    (%d %d)", neighbours[i].x, neighbours[i].y);
        printf("\n"); */
        


        for (int i = 0; i < num; i++) {
            int new = 0;

            while ((tmp[new].x != neighbours[i].x || tmp[new].y != neighbours[i].y) && new < len)
                new++;

            if (new == len) {
                tmp[len].x = neighbours[i].x;
                tmp[len].y = neighbours[i].y;
                len++;
            }
        }
        count++;
    }

    return len;
}

void gravity(struct field* f)
{
    for (int i = 0; i < f->width; i++) {
        if (get_sum_column(f, i) > 0) {
            for (int j = 0; j < f->height; j++) {
                struct coords* current = create_coords(i, j);

                if (get_coord(f, current) == 0) {
                    struct coords* next = create_coords(i, j + 1);

                    while (next->y < f->height && get_coord(f, next) == 0) {
                        next->y++;
                    }

                    if (next->y == (f->height)) {
                        j = f->height;
                    } else {
                        set_coord(f, current, get_coord(f, next));
                        set_coord(f, next, 0);
                    }
                }
            }        
        } else {
            for (int j = i; j > 0; j--) {
                for (int k = 0; k < f->height; k++) {
                    struct coords* current = create_coords(j - 1, k);
                    int tmp = get_coord(f, current);

                    if (tmp > 0) {
                        set_ints(f, j, k, tmp);
                        set_ints(f, j - 1, k, 0);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int score_total = 0;
    int score = 0;
    int shuffled;
    int difficulty_level = 4;
    int threshhold = 90;
    int ff_go_next = 1;
    srand(time(NULL));
    struct field* field = create_field(20, 10);

    char* strings[4] = {"\033[0;31m", "\033[0;33m", "\033[0;35m", "\033[0;36m"};
    char* string = "CONGRATULATIONS!";

    char* skins[3] = {"\u25A0 ", "\u2588\u2588", "\u263B "};


    for (int i = 0; i < 3; i++)
        printf("%d: %s\n", i + 1, skins[i]);


    int tmp = -1;
    printf("Choose wisely: ");
    scanf("%d", &tmp);
    tmp--;

    

    init_field(field, difficulty_level);
    print_field(field, 1, skins[tmp], threshhold);


    while (ff_go_next > 0) {
        ff_go_next = 0;

        init_field(field, difficulty_level);
        shuffled = 0;

        while (possible_moves(field) > -1) {
            int input_x = -1;
            int input_y = -1;
            int error = 0;
            int group_size;
            struct coords* group;
            struct coords* selected;
            

            do {
                print_field(field, 1, skins[tmp], threshhold);
                printf("\n\tSCORE:\t\033[0;32m%d\033[0;37m / %d\n\n", score, score_total);


                switch (error)
                {
                case 1:
                    printf("ERROR: [%d, %d] invalid for field of size %d * %d\n", input_x + 1, input_y + 1, field->width, field->height);
                    break;

                case 2:
                    printf("ERROR: [%d, %d] has no adjacent blocks of same colour\n", input_x + 1, input_y + 1);
                
                default:
                    break;
                }


                printf("Enter valid  [X, Y] coordinates: \n");
                printf("X: ");
                scanf("%d", &input_x);
                input_x--;
                printf("Y: ");
                scanf("%d", &input_y);
                input_y--;


                if (input_x < 0 || input_y < 0 || input_x >= field->width || input_y >= field->height || get_ints(field, input_x, input_y) == 0) {
                    error = 1;
                } else {
                    error = 0;
                }
                
                if (error == 0) {
                    group_size;

                    group = calloc(field->height * field->width, sizeof(struct coords));
                    selected = malloc(sizeof(struct coords));
                    
                    
                    selected->x = input_x;
                    selected->y = input_y;

                    group_size = get_group(field, selected, &group);

                    if (group_size < 2) {
                        error = 2;
                    }
                }
            } while (error != 0);


            /*
                This is a debug feature. 
                The selected group is printed in white 
                    to be able to verify the correctness of the group
            */
            int temp = (((group_size / 4) + 1) * group_size) + ((group_size / 4) * (group_size % 4));


            set_coords(field, group, group_size, -1);
            print_field(field, 1, skins[tmp], threshhold);
            
            printf("\n\tSCORE:\t\033[0;32m%d\033[0;37m / %d\n\n", score, score_total);
            sleep(1);

            score += temp;


            /*
                explosion == set_coords(...)
                gravity pulls the zeros to the top left
            */
            set_coords(field, group, group_size, 0);
            gravity(field);
            print_field(field, 1, skins[tmp], threshhold);
            printf("\n\tSCORE:\t\033[0;32m%d\033[0;37m / %d\n\n", score, score_total);


            if (shuffled == 0 && possible_moves(field) == -1 && get_sum_column(field, field->width - 1) >= 0) {
                printf("\tshUfFeLliNg\n");
                shuffle_field(field, difficulty_level, skins[tmp], threshhold);
                shuffled = 1;
            }
        }
        
        for (int i = 0; i < strlen(string); i++) 
            printf("%s%c", strings[i % 4], string[i]);
        printf("\n");
        printf("\033[0;37mYou have cleared \033[0;32m%d\033[0;37m\% with a score of \033[0;32m%d\033[0;37m!\n", get_percent(field), score);
        score_total += score;
        printf("Your total score sums up to: \033[0;32m%d\033[0;37m\n", score_total);
        score = 0;

        if (get_percent(field) > threshhold) {
            printf("You won a \033[0;33m*\033[0;35m*\033[0;37mBonus\033[0;31m*\033[0;36m*\033[0;37m round, have fun!\n");
            for (int i = 0; i < 5; i++) {
                printf("\r\t\t%d", 5 - i);
                fflush(stdout);
                sleep(1);
            }
            printf("\n");
            ff_go_next = 1;
            
            if (get_percent(field) == 100)
                threshhold -= 1;
            else if (get_percent(field) < 99)
                threshhold++;
        }
    }
    


    destroy_field(field);
    exit(EXIT_SUCCESS);
}
