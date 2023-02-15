#include "string.h"
#include "klotski_position.h"
#include "klotski_list_queue.h"

VISIT_TABLE visited_head = {0};
int n_visited = 0;

int is_block_in_piece(BLOCK b, PIECE *p)
{
    for (int i = 0; i < p->n; i++)
    {
        BLOCK tmp = p->blocks[i];
        if (tmp.x == b.x && tmp.y == b.y)
            return 1;
    }
    return 0;
}

int is_block_in_any_piece(BLOCK b, POSITION *p)
{
    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if (is_block_in_piece(b, &p->pieces[i]))
            return 1;
    }
    return 0;
}
// block��������Чֵ��Χ��
int is_block_in_range(BLOCK b)
{
    if (b.x < 0 || b.y < 0 || b.x > BOARD_X - 1 || b.y > BOARD_Y - 1)
        return 0;

    return 1;
}

int can_move(POSITION *pos, PIECE *piece, BLOCK (*d)(BLOCK))
{
    for (int i = 0; i < piece->n; i++)
    {
        BLOCK b = piece->blocks[i];
        BLOCK next_block = d(b);
        if (!is_block_in_range(next_block) || (is_block_in_any_piece(next_block, pos) && !is_block_in_piece(next_block, piece)))
            return 0;
    }
    return 1;
}

int visit_table_from_position(VISIT_TABLE *vis, POSITION *pos)
{
    if ((NULL == vis) || (NULL == pos))
    {
        printf("paramter is null.\r\n");
        return 0;
    }
    
    memcpy(vis->visited, pos->mat, BOARD_X * BOARD_Y * sizeof(unsigned char));
    vis->parent = vis->next = NULL;

    return 1;
}

void pieces_from_matrix(POSITION *pos)
{
    unsigned char mat_lable[BOARD_X][BOARD_Y] = {0};

    for (int i = 0; i < BOARD_X; i++)
    {
        for (int j = 0; j < BOARD_Y; j++)
        {
            // �жϸ�λ���Ƿ��Ѿ�������
            if(0 == mat_lable[i][j])
            {
                if (MACHID_SOLIDER == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if(0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k+1;
                            pos->pieces[k].mat_id = MACHID_SOLIDER;
                            pos->pieces[k].n = 1;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            mat_lable[i][j] = 1;
                            break;
                        }

                    }
                }

                if(MACHID_GENERAL_VERT == pos->mat[i][j])
                {
                    // ��ô������������Ǹ�����������һ��PIECES
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if(0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k+1;
                            pos->pieces[k].mat_id = MACHID_GENERAL_VERT;
                            pos->pieces[k].n = 2;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i+1;
                            pos->pieces[k].blocks[1].y = j;
                            mat_lable[i][j] = 1;
                            mat_lable[i+1][j] = 1;
                            break;
                        }
                    }
                }

                if(MACHID_GENERAL_HORI == pos->mat[i][j])
                {
                    // ��ô�������������������������һ��PIECES
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if(0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k+1;
                            pos->pieces[k].mat_id = MACHID_GENERAL_HORI;
                            pos->pieces[k].n = 2;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i;
                            pos->pieces[k].blocks[1].y = j+1;
                            mat_lable[i][j] = 1;
                            mat_lable[i][j+1] = 1;
                            break;
                        }
                    }
                }

                if(MACHID_CAO == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if(0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k+1;
                            pos->pieces[k].mat_id = MACHID_CAO;
                            pos->pieces[k].n = 4;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i;
                            pos->pieces[k].blocks[1].y = j+1;
                            pos->pieces[k].blocks[2].x = i+1;
                            pos->pieces[k].blocks[2].y = j;
                            pos->pieces[k].blocks[3].x = i+1;
                            pos->pieces[k].blocks[3].y = j+1;
                            mat_lable[i][j] = 1;
                            mat_lable[i][j+1] = 1;
                            mat_lable[i+1][j] = 1;
                            mat_lable[i+1][j+1] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}

int position_from_visit_table(POSITION *pos, VISIT_TABLE *vis)
{
    if ((NULL == vis) || (NULL == pos))
    {
        printf("paramter is null.\r\n");
        return 0;
    }

    memcpy(pos->mat, vis->visited, BOARD_X * BOARD_Y * sizeof(unsigned char));
    pieces_from_matrix(pos);

    return 1;
}

// ��picesת��Ϊmatrix
void matrix_from_pieces(POSITION *pos)
{
    int mat[BOARD_X][BOARD_Y] = {};

    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *p = &pos->pieces[i];
        for (int j = 0; j < p->n; j++)
        {
            BLOCK *b = &p->blocks[j];
            mat[b->x][b->y] = p->mat_id;
        }
    }

    for (int i = 0; i < BOARD_X; i++)
    {
        for (int j = 0; j < BOARD_Y; j++)
            pos->mat[i][j] = mat[i][j];
    }
}

POSITION move_piece(POSITION *pos, PIECE *p, BLOCK (*d)(BLOCK))
{
    POSITION new_pos = *pos;
    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *tmp = &new_pos.pieces[i];
        if (tmp->id == p->id)
        {
            for (int j = 0; j < tmp->n; j++)
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

BLOCK up(BLOCK b)
{
    BLOCK tmp = {b.x - 1, b.y};
    return tmp;
}

BLOCK down(BLOCK b)
{
    BLOCK tmp = {b.x + 1, b.y};
    return tmp;
}

BLOCK left(BLOCK b)
{
    BLOCK tmp = {b.x, b.y - 1};
    return tmp;
}

BLOCK right(BLOCK b)
{
    BLOCK tmp = {b.x, b.y + 1};
    return tmp;
}

POSITION *all_neighbours(POSITION *p, int *n)
{
    POSITION *arr = malloc(MAX_POSSIBLE_NEIGHBOURS * sizeof(POSITION));
    *n = 0;
    for (int i = 0; i < PIECES_IN_POSITION; i++)
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

int is_visited_empty(void)
{
    return visited_head.next == NULL;
}

void del_visited(void)
{
    VISIT_TABLE *p = &visited_head.next;
    VISIT_TABLE *del_p = NULL;
    while (p)
    {
        del_p = p;
        p = p->next;
        free(p);
    }
    visited_head.next = NULL;
}

void add_to_visited(POSITION *pos, POSITION *parent)
{
    VISIT_TABLE *p = &visited_head;
    VISIT_TABLE *visited = NULL;
    visited = (VISIT_TABLE *)malloc(sizeof(VISIT_TABLE));
    if (NULL == visited)
    {
        printf("not enouth memery.\r\n");
        return;
    }

    for (int i = 0; i < BOARD_X; i++)
        for (int j = 0; j < BOARD_Y; j++)
            visited->visited[i][j] = pos->mat[i][j];

    // �ҵ�����β��
    while (p->next != NULL)
        p = p->next;

    p->next = visited;
    visited->next = NULL;
    n_visited++;

    if (parent == pos)
    {
        // �б��е�һ��
        visited->parent = NULL;
    }
    else
    {
        // ���������ҵ���parent��visite��parentָ��ָ����
        p = &visited_head;
        int i = 0;
        int j = 0;
        while (p->next != NULL)
        {
            p = p->next;

            // �ҵ���ȫ��ͬ�ģ���Ϊ��ָ��
            if (0 == memcmp(p->visited, parent->mat, BOARD_X * BOARD_Y))
            {
                visited->parent = p;
            }
        }
    }
}

int is_visited(POSITION *pos)
{
    VISIT_TABLE *p = g_lq.head;
    int equal = 1;

    while (p != NULL)
    {
        if (0 != memcmp(p->visited, pos->mat, BOARD_X * BOARD_Y))
            equal = 0;
        else
            equal = 1;

        if (equal)
            return 1;

        p = p->next;
    }
    return 0;
}

void print_position(VISIT_TABLE *pos)
{
    for (int i = 0; i < BOARD_X; i++)
    {
        for (int j = 0; j < BOARD_Y; j++)
            printf("%d ", pos->visited[i][j]);
        printf("\n");
    }

    printf("\n");
}

int is_good(POSITION *pos)
{
    BLOCK b1 = {3, 1};
    BLOCK b2 = {4, 2};
    int pieces_index = 0;

    for(int i = 0;i<MAX_POSSIBLE_NEIGHBOURS;i++)
    {
        if(MACHID_CAO == pos->pieces[i].mat_id)
        {
            pieces_index = i;
            break;
        }
    }
    if (is_block_in_piece(b1, &pos->pieces[pieces_index]) && is_block_in_piece(b2, &pos->pieces[pieces_index]))
        return 1;

    return 0;
}

void print_next_step_solution(VISIT_TABLE *p_vis)
{
    if (NULL == p_vis)
    {
        printf("can not find p in table.\r\n");
        return;
    }

    while (p_vis->parent->parent)
    {
        print_position(p_vis);
        p_vis = p_vis->parent;
    }
    print_position(p_vis);
}

void print_solution(POSITION *p)
{
    VISIT_TABLE *p_vis = &visited_head;

    p_vis = p_vis->next;
    while (p_vis != NULL)
    {
        if (0 == memcmp(p_vis->visited, p->mat, BOARD_X * BOARD_Y * sizeof(unsigned char)))
        {
            break;
        }
        p_vis = p_vis->next;
    }

    while (p_vis->parent)
    {
        print_position(p_vis);
        p_vis = p_vis->parent;
    }
    print_position(p_vis);
}

int get_visited_num(void)
{
    return n_visited;
}
