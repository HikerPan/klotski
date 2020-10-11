#include "position.h"

int is_block_in_piece(BLOCK b, PIECE *p)
{
    for(int i = 0; i < p->n; i++)
    {
        BLOCK tmp = p->blocks[i];
        if(tmp.x == b.x && tmp.y == b.y)
            return 1;
    }
    return 0;
}

int is_block_in_any_piece(BLOCK b, POSITION *p)
{
    for(int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if(is_block_in_piece(b, &p->pieces[i]))
            return 1;
    }
    return 0;
}

int is_block_in_range(BLOCK b)
{
    if(b.x < 0 || b.y < 0 || b.x > BOARD_X - 1 || b.y > BOARD_Y - 1)
        return 0;

    return 1;
}

int can_move(POSITION *pos, PIECE *piece, BLOCK (*d) (BLOCK))
{
    for(int i = 0; i < piece->n; i++)
    {
        BLOCK b = piece->blocks[i];
        BLOCK next_block = d(b);
        if(!is_block_in_range(next_block) || (is_block_in_any_piece(next_block, pos) && !is_block_in_piece(next_block, piece)))
            return 0;
    }
    return 1;
}

void matrix_from_pieces(POSITION *pos)
{
    int mat[BOARD_X][BOARD_Y] = {};

    for(int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *p = &pos->pieces[i];
        for(int j = 0; j < p->n; j++)
        {
            BLOCK *b = &p->blocks[j];
            mat[b->x][b->y] = p->mat_id;
        }
    }

    for(int i = 0; i < BOARD_X; i++)
    {
        for(int j = 0; j < BOARD_Y; j++)
            pos->mat[i][j] = mat[i][j];
    }
}

POSITION move_piece(POSITION *pos, PIECE *p, BLOCK (*d) (BLOCK))
{
    POSITION new_pos = *pos;
    for(int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *tmp = &new_pos.pieces[i];
        if(tmp->id == p->id)
        {
            for(int j = 0; j < tmp->n; j++)
            {
                BLOCK b = d(tmp->blocks[j]);
                tmp->blocks[j] = b;
            }
            break;
        }
    }
    matrix_from_pieces(&new_pos);
    return new_pos;
}

BLOCK up (BLOCK b)
{
    BLOCK tmp = {b.x - 1, b. y};
    return tmp;
}

BLOCK down (BLOCK b)
{
    BLOCK tmp = {b.x + 1, b. y};
    return tmp;
}

BLOCK left (BLOCK b)
{
    BLOCK tmp = {b.x,  b.y - 1};
    return tmp;
}

BLOCK right (BLOCK b)
{
    BLOCK tmp = {b.x, b. y + 1};
    return tmp;
}

POSITION * all_neighbours(POSITION *p, int *n)
{
    POSITION * arr = malloc(MAX_POSSIBLE_NEIGHBOURS * sizeof(POSITION));
    *n = 0;
    for(int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *piece = &p->pieces[i];
        if (can_move(p, piece, up))
        {
            arr[*n] = move_piece(p, piece, up);
            *n = *n + 1;
        }
        if (can_move(p, piece, down))
        {
            arr[*n] = move_piece(p, piece, down);
            *n = *n + 1;
        }
        if (can_move(p, piece, left))
        {
            arr[*n] = move_piece(p, piece, left);
            *n = *n + 1;
        }
        if (can_move(p, piece, right))
        {
            arr[*n] = move_piece(p, piece, right);
            *n = *n + 1;
        }
    }
    return arr;
}

unsigned char visited[MAX][BOARD_X][BOARD_Y];
int n_visited = 0;

void add_to_visited(POSITION *pos)
{
    for(int i = 0; i < BOARD_X; i++)
        for(int j = 0; j < BOARD_Y; j++)
            visited[n_visited][i][j] = pos->mat[i][j];
    n_visited++;
}

int is_visited(POSITION *pos)
{
    int equal = 1;
    for(int i = 0; i < n_visited; i++)
    {
        equal = 1;
        for(int j = 0; j < BOARD_X && equal; j++)
            for(int k = 0; k < BOARD_Y && equal; k++)
                if(visited[i][j][k] != pos->mat[j][k])
                    equal = 0;
        if(equal)
            return 1;
    }
    return 0;
}

void print_position(POSITION *pos)
{
    for(int i = 0; i < BOARD_X; i++)
    {
        for(int j = 0; j < BOARD_Y; j++)
            printf("%d ", pos->mat[i][j]);
        printf("\n");
    }

    printf("\n");
}

int is_good(POSITION *pos)
{
    BLOCK b1 = {3, 1};
    BLOCK b2 = {4, 2};
    if(is_block_in_piece(b1, &pos->pieces[1]) && is_block_in_piece(b2, &pos->pieces[1]))
        return 1;

    return 0;
}

void print_solution(POSITION *p)
{
    while(p->parent)
    {
        print_position(p);
        p = p->parent;
    }
    print_position(p);
}
