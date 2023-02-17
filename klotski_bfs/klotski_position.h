/*** 
 * @Author: Alex
 * @Date: 2023-02-15 09:43:10
 * @LastEditors: hikerpan
 * @LastEditTime: 2023-02-16 22:02:47
 * @FilePath: \klotski\klotski_bfs\klotski_position.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by hikerpan, All Rights Reserved. 
 */
#ifndef POSITION_H
#define POSITION_H
#include <stdio.h>
#include <stdlib.h>
#define BLOCKS_IN_PIECE 4
// #define PIECES_IN_POSITION 10
#define PIECES_IN_POSITION 16
#define MAX_POSSIBLE_NEIGHBOURS 16

#define MAX 65536

#define BOARD_X 5
#define BOARD_Y 4

#define MACHID_NOT_USED             0
#define MACHID_SOLIDER              1
#define MACHID_GENERAL_VERT         2
#define MACHID_GENERAL_HORI         3
#define MACHID_CAO                  4
#define MACHID_EMPTY                5

#define VISIT_MAP_SIZE_MAX          (15000)

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

/**
 * 该数据结构用于压缩华容道棋盘布局
 * 棋盘上曹+将+兵+空 最多 1+1+12+2 = 16个，共需要48bit，
 * 基本思路：
 * 1. 每三个bit代表一个piece;
 * 2. 从map_low低字节开始，每三个代表一个pieces
 * 3. 000 未使用，001 小兵，010，竖将 011 横将 100 曹操 101，空
 * 4. map_high 高16bit，表示其父VISIT_MAP结构的数组下标；
 * 5. 从map_low低位开始，到map_high的bit15，每三个bit表示一个pieces，
*/
typedef struct visited_map
{
    int map_low;
    int map_high;
}VISIT_MAP;


int is_visited_empty(void);
int is_good(POSITION *pos);
void print_position(POSITION *pos);
// void print_position(VISIT_TABLE *pos);

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

int visit_map_init(int size);
int visit_map_from_position(VISIT_MAP *map, POSITION *pos);
int position_from_visit_map(POSITION *pos, VISIT_MAP *map);
int visit_map_read(VISIT_MAP *map);
int visit_map_write(VISIT_MAP *map);
int visit_map_get_parent_index(void);
void print_next_step_map_solution(VISIT_MAP *map);
int is_symmetric(POSITION *pos);
#endif // POSITION_H
