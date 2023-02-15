#ifndef POSITION_H
#define POSITION_H
#include <stdio.h>
#include <stdlib.h>
#define BLOCKS_IN_PIECE 4
#define PIECES_IN_POSITION 10
#define MAX_POSSIBLE_NEIGHBOURS 10

#define MAX 65536

#define BOARD_X 5
#define BOARD_Y 4

#define MACHID_SOLIDER              1
#define MACHID_GENERAL_VERT         2
#define MACHID_GENERAL_HORI         3
#define MACHID_CAO                  4
#define MACHID_EMPTY                0


typedef struct block
{
    signed char x, y;
} BLOCK;

typedef struct piece
{
    unsigned char id; //unique id (each piece has its own id)
    unsigned char mat_id; //size id (pieces of the same size have the same mat_id)
    unsigned char n; //number of blocks in piece
    BLOCK blocks[BLOCKS_IN_PIECE];
} PIECE;

typedef struct position
{
    unsigned char mat[BOARD_X][BOARD_Y];
    PIECE pieces[PIECES_IN_POSITION];
    struct position *parent;
} POSITION;

typedef struct visited_table
{
    unsigned char visited[BOARD_X][BOARD_Y];
    struct visited_table *parent;
    struct visited_table *next;
}VISIT_TABLE;


int is_visited_empty(void);
int is_good(POSITION *pos);
// void print_position(POSITION *pos);
void print_position(VISIT_TABLE *pos);

int is_visited(POSITION *pos);
// void add_to_visited(POSITION *pos);
void add_to_visited(POSITION *pos, POSITION *parent);
POSITION * all_neighbours(POSITION *p, int *n);
BLOCK right (BLOCK b);
BLOCK left (BLOCK b);
BLOCK down (BLOCK b);
BLOCK up (BLOCK b);
POSITION move_piece(POSITION *pos, PIECE *p, BLOCK (*d) (BLOCK));
void matrix_from_pieces(POSITION *pos);
int can_move(POSITION *pos, PIECE *piece, BLOCK (*d) (BLOCK));
int is_block_in_range(BLOCK b);
int is_block_in_any_piece(BLOCK b, POSITION *p);
int is_block_in_piece(BLOCK b, PIECE *p);
void print_solution(POSITION *p);
int get_visited_num(void);



int visit_table_from_position(VISIT_TABLE *vis, POSITION *pos);
void pieces_from_matrix(POSITION *pos);
int position_from_visit_table(POSITION *pos, VISIT_TABLE *vis);
void print_next_step_solution(VISIT_TABLE *p_vis);
#endif // POSITION_H
