#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


struct coords {
    int x;
    int y;
};


struct field {
    int height;
    int width;
    int* data;
};


struct field* create_field(int h, int w);
struct field* destroy_field(struct field* f);
struct coords* create_coords(int x, int y);
struct coords* destroy_coords(struct coords* c);
void print_field(struct field* f, char* skin, int t);
int get_percent(struct field* f);
int get_sum_column(struct field* f, int column);
int coords2idx(struct field* f, struct coords* c);
void idx2coords(struct field* f, int idx, struct coords* c);
int set_coord(struct field* f, struct coords* c, int v);
int set_ints(struct field* f, int x, int y, int v);
int set_coords(struct field* f, struct coords* c, int n, int v);
int get_coord(struct field* f, struct coords* c);
int get_ints(struct field* f, int x, int y);
int get_adjacent(struct field* f, struct coords* c, struct coords** ret);
void shuffle_field(struct field* f, int c_vals, char* skin, int t);
