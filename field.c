#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "field.h"



struct field* create_field(int w, int h)
{
    struct field* f = malloc(sizeof(struct field));
    int data_len = h * w;


    f->height = h;
    f->width = w;


    f->data = calloc(data_len, sizeof(int));
    for (int i = 0; i < data_len; i++) 
        f->data[i] = 0;

    return f;
}

struct field* destroy_field(struct field* f)
{
    free(f->data);
    free(f);

    return NULL;
}

struct coords* create_coords(int x, int y)
{
    struct coords* c = malloc(sizeof(struct field));
    
    c->x = x;
    c->y = y;

    return c;
}

struct coords* destroy_coords(struct coords* c)
{
    free(c);
    return NULL;
}


void print_field(struct field* f, int color, char* skin, int t)
{
    system("clear");

    for (int i = 0; i < (f->height * f->width); i++) {
        if (i % f->width == 0) {
            printf("\033[0;37m");
            if ((f->height - (i / f->width)) > 9)
                printf(" ");
            else 
                printf("  ");
            printf("%d ", (f->height - (i / f->width)));
        } 
        

        if (color != 0) {
            switch (f->data[i])
            {
            case 1:
                printf("\033[0;31m");
                break;
            case 2:
                printf("\033[0;36m");
                break;
            case 3:
                printf("\033[0;33m");
                break;
            case 4:
                printf("\033[0;35m");
                break;
            case -1: 
                printf("\033[0;37m");
                break;
            
            default:
                printf("\033[0;30m");
                break;
            }
        }

        // printf("%d ", f->data[i]);
        // printf("\u25A0 ");
        // printf("\u2588\u2588");
        printf("%s", skin);




        if ((i + 1) % f->width == 0) {
            printf("\033[0;37m");
            if ((f->height - (i / f->width)) < 10)
                printf(" ");
            printf("%d ", (f->height - (i / f->width)));

            if (i / f->width == 0)
                printf("\t\033[0;32m%d\%\033[0;37m / %d\%", get_percent(f), t);
        }

        if ((i + 1) % f->width == 0)
            printf("\n");
    }
    printf("\033[0;37m");


    printf("    ");
    for (int i = 0; i < f->width; i++)
        printf("%d ", (i + 1) % 10);
    printf("\n");
}

int get_percent(struct field* f)
{
    int zero = 0;

    for (int i = 0; i < f->height * f->width; i++) {
        if (f->data[i] == 0) 
            zero++;
    }
    
    return ((zero * 100) / (f->height * f->width));
}

int get_sum_column(struct field* f, int column)
{
    int sum = 0;

    for (int i = 0; i < f->height; i++) {
        struct coords* c = create_coords(column, i);

        sum += get_coord(f, c);
    }
    
    return sum;
}

int coords2idx(struct field* f, struct coords* c)
{
    int idx = -1;

    if (c->x < 0 || c->x > (f->width - 1) || c->y < 0 || c->y > (f->height - 1)) {
        printf("ERROR: Invalid coordinates (%d %d) in field with size %d*%d\n", c->x, c->y, f->width, f->height);
        return idx;
    }

    idx = (f->height - c->y - 1) * f->width + c->x;

    return idx;
}

void idx2coords(struct field* f, int idx, struct coords* c)
{
    if (c == NULL)
        return;  
    if (idx < 0 || idx > (f->height * f->width - 1)) {
        printf("ERROR: Index %d out of bound for field of size %d\n", idx, f->width * f->height);
        return;
    }
    
    c->x = idx % f->width;
    c->y = (f->height - 1) - (idx / f->width);
}

int set_coord(struct field* f, struct coords* c, int v)
{
    int idx = -1;

    if ((idx = coords2idx(f, c)) == -1)
        return idx;

    f->data[idx] = v;

    return 0;
}

int set_ints(struct field* f, int x, int y, int v)
{
    struct coords* tmp = create_coords(x, y);

    set_coord(f, tmp, v);

    free(tmp);
}

int set_coords(struct field* f, struct coords* c, int n, int v)
{
    int idx;

    for (int i = 0; i < n; i++)
    {
        idx = -1;

        if ((idx = coords2idx(f, &c[i])) == -1)
            return idx;

        f->data[idx] = v;
    }
    
    return 0;
}

int get_coord(struct field* f, struct coords* c)
{
    int idx = 0;

    if ((idx = coords2idx(f, c)) == -1)
        return -1;
    
    return f->data[idx];
}

int get_ints(struct field* f, int x, int y)
{
    struct coords* tmp = create_coords(x, y);
    int ret = get_coord(f, tmp);

    free(tmp);
    return ret;
}

int get_adjacent(struct field* f, struct coords* c, struct coords** ret)
{
    struct coords* tmp = *ret;
    int count = 0;

    for (int i = 0; i < 4; i++) {
        tmp[i].x = -1;
        tmp[i].y = -1;
    }


    if (c->y < (f->height - 1)) {
        tmp[count].x = c->x;
        tmp[count].y = c->y + 1;
        count++;
    }
    
    if (c->x < (f->width - 1)) {
        tmp[count].x = c->x + 1;
        tmp[count].y = c->y;
        count++;
    }

    if (c->y > 0) {
        tmp[count].x = c->x;
        tmp[count].y = c->y - 1;
        count++;
    }
    
    if (c->x > 0) {
        tmp[count].x = c->x - 1;
        tmp[count].y = c->y;
        count++;
    }

    return count;
}

void shuffle_field(struct field* f, int c_vals, char* skin, int t)
{
    int* c = calloc(f->height * f->width, sizeof(int));
    int* vals = calloc(c_vals, sizeof(int));
    int count = 0;
    int ret = 0;

    srand(time(NULL));

    for (int i = 0; i < c_vals; i++)
        vals[i] = 0;
    

    for (int i = 0; i < f->height * f->width; i++) {
        if (f->data[i] > 0) {
            c[count] = i;
            vals[f->data[i] - 1]++;
            count++;
        }
    }


    for (int i = 0; i < c_vals; i++) {
        if (vals[i] < 2) 
            ret++;
    }
    if (ret == c_vals)
        return;
    
    
    for (int i = 0; i < count; i++) {
        f->data[c[i]] = -1;
    }
    print_field(f, 1, skin, t);
    sleep(1);
    


    for (int i = 0; i < count; i++) {
        int r = rand() % c_vals;
        while (vals[r] == 0)
            r = rand() % c_vals;

        f->data[c[i]] = r + 1;
        vals[r]--;
    }
    
    
}
