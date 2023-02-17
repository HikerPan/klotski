#include "string.h"
#include "klotski_position.h"
#include "klotski_list_queue.h"

VISIT_MAP *g_visited_map = NULL;
VISIT_TABLE visited_head = {0};
int n_visited = 0;

int is_block_in_piece(BLOCK b, PIECE *p)
{
    if(MACHID_EMPTY == p->mat_id)
    {
        return 0;
    }
    for (int i = 0; i < p->n; i++)
    {
        BLOCK tmp = p->blocks[i];
        if (tmp.x == b.x && tmp.y == b.y)
            return 1;
    }
    return 0;
}

int is_block_in_empty_piece(BLOCK b, POSITION *p)
{
    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if((MACHID_EMPTY == p->pieces[i].mat_id)
            &&(p->pieces->blocks[0].x == b.x)
            &&(p->pieces->blocks[0].y == b.y))
        {
            return 1;
        }
    }

    return 0;
}

int is_block_in_any_piece(BLOCK b, POSITION *p)
{
    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if(MACHID_EMPTY == p->pieces[i].mat_id)
        {
            continue;
        }

        if (is_block_in_piece(b, &p->pieces[i]))
            return 1;
    }
    return 0;
}

int is_block_in_range(BLOCK b)
{
    if (b.x < 0 || b.y < 0 || b.x > BOARD_X - 1 || b.y > BOARD_Y - 1)
        return 0;

    return 1;
}

/**
 * �Ƿ�����ƶ�
*/
int can_move(POSITION *pos, PIECE *piece, BLOCK (*d)(BLOCK))
{
    for (int i = 0; i < piece->n; i++)
    {
        BLOCK b = piece->blocks[i];
        BLOCK next_block = d(b);

        // ���귶ΧΥ��
        // if(!is_block_in_range(next_block))
        // {
        //     return 0;
        // }

        // if()
        
        if (!is_block_in_range(next_block) || (is_block_in_any_piece(next_block, pos) && !is_block_in_piece(next_block, piece)))
        // if (!is_block_in_range(next_block) || is_block_in_empty_piece(next_block,pos))
            return 0;
    }
    return 1;
}

static int head = 0;
static int rear = 0;

/***
 * @description:
 * @param {int} size
 * @return {*}
 */
int visit_map_init(int size)
{
    g_visited_map = (VISIT_MAP *)calloc(size, sizeof(VISIT_MAP));
    if (NULL == g_visited_map)
    {
        printf("[VISIT_MAP init]can not calloc %d bytes.", size * sizeof(VISIT_MAP));
        return 0;
    }

    head = rear = 0;
    return 1;
}

static void map_set(VISIT_MAP *map,  unsigned int  mach_id, int index)
{
    // int id = mach_id;
    // id = id << (index * 3);
    if (index < 10)
    {
        map->map_low |= (mach_id << (index * 3));
    }
    else if (index > 10)
    {
        map->map_high |= (mach_id << ((index - 11) * 3 + 2));
    }
    else
    {
        map->map_low |= (mach_id << (index * 3));
        map->map_high |= (mach_id >> 1);
    }
}

/***
 * @description: 
 * @param {VISIT_MAP} *map
 * @param {POSITION} *pos
 * @return {*}
 */
int visit_map_from_position(VISIT_MAP *map, POSITION *pos)
{
    memset(map, 0, sizeof(VISIT_MAP));

    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        // map_set(map, pos->pieces[i].mat_id, i);
        if(!pos->pieces[i].n)
            break;

        switch (pos->pieces[i].mat_id)
        {
        case MACHID_SOLIDER:
            map_set(map, MACHID_SOLIDER, i);
            break;
        case MACHID_GENERAL_VERT:
            map_set(map, MACHID_GENERAL_VERT, i);
            break;
        case MACHID_GENERAL_HORI:
            map_set(map, MACHID_GENERAL_HORI, i);
            break;
        case MACHID_CAO:
            map_set(map, MACHID_CAO, i);
            break;
        case MACHID_EMPTY:
            map_set(map, MACHID_EMPTY, i);
            break;
        default:
            map_set(map, MACHID_NOT_USED, i);
            break;
        }
    }

    return 1;
}

/***
 * @description: ���ݽṹ��mapת��Ϊpos
 * @param {POSITION} *pos
 * @param {VISIT_MAP} *map
 * @return {*}
 */
int position_from_visit_map(POSITION *pos, VISIT_MAP *map)
{
    unsigned char used[BOARD_X][BOARD_Y] = {0}; // ���ڱ�Ǹ�λ���Ƿ��Ѿ������ù�?
    int map_bit = 0;
    int found = 0;

    memset(pos, 0, sizeof(POSITION));

    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if (i < 10)
        {
            map_bit = (map->map_low >> 3 * i) & 0x07;
        }
        else if (i > 10)
        {
            map_bit = (map->map_high >> (3 * (i - 11) + 2)) & 0x07;
        }
        else
        {
            map_bit = (map->map_low >> 3 * i) & 0x01;
            map_bit |= ((map->map_high & 0x03) << 1);
        }
        // ����map_bitΪ0��˵�������Ѿ�����
        if (!map_bit)
            break;

        found = 0;
        switch (map_bit)
        {
        case MACHID_SOLIDER:
            for (int j = 0; j < BOARD_X; j++)
            {
                for (int k = 0; k < BOARD_Y; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_SOLIDER;
                        used[j][k] = 1;
                        found = 1;
                        break;

                    }
                }
                if(found)
                {
                    break;
                }
            }

            break;
        case MACHID_GENERAL_VERT:
            for (int j = 0; j < BOARD_X; j++)
            {
                for (int k = 0; k < BOARD_Y; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_GENERAL_VERT;
                        pos->mat[j+1][k] = MACHID_GENERAL_VERT;
                        used[j][k] = 1;
                        used[j+1][k] = 1;
                        found = 1;
                        break;
                    }
                }
                if(found)
                {
                    break;
                }
            }
            break;
        case MACHID_GENERAL_HORI:
            for (int j = 0; j < BOARD_X; j++)
            {
                for (int k = 0; k < BOARD_Y; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_GENERAL_HORI;
                        pos->mat[j][k+1] = MACHID_GENERAL_HORI;
                        used[j][k] = 1;
                        used[j][k+1] = 1;
                        found = 1;
                        break;
                    }
                }
                if(found)
                {
                    break;
                }
            }
            break;
        case MACHID_CAO:
            for (int j = 0; j < BOARD_X; j++)
            {
                for (int k = 0; k < BOARD_Y; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_CAO;
                        pos->mat[j][k+1] = MACHID_CAO;
                        pos->mat[j+1][k] = MACHID_CAO;
                        pos->mat[j+1][k+1] = MACHID_CAO;

                        used[j][k] = 1;
                        used[j][k+1] = 1;
                        used[j+1][k] = 1;
                        used[j+1][k+1] = 1;

                        found = 1;
                        break;
                    }
                }
                if(found)
                {
                    break;
                }
            }
            break;
        case MACHID_EMPTY:
            for (int j = 0; j < BOARD_X; j++)
            {
                for (int k = 0; k < BOARD_Y; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = 0;
                        used[j][k] = 1;
                        found = 1;
                        break;

                    }
                }
                if(found)
                {
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    pieces_from_matrix(pos);
}

int visit_map_write(VISIT_MAP *map)
{
    memset(&g_visited_map[rear], 0, sizeof(VISIT_MAP));
    memcpy(&g_visited_map[rear], map, sizeof(VISIT_MAP));
    rear++;
    if (rear >= VISIT_MAP_SIZE_MAX)
    {
        printf("visit map is full, you need to increase it. \r\n");
        return 0;
    }

    return 1;
}

int visit_map_read(VISIT_MAP *map)
{
    memset(map, 0, sizeof(VISIT_MAP));
    memcpy(map, &g_visited_map[head], sizeof(VISIT_MAP));
    head++;
    if (head > rear)
    {
        printf("visit map empty. \r\n");
        return 0;
    }

    return 1;
}

int visit_map_get_parent_index(void)
{
    return (head - 1);
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
    unsigned char mach_lable[BOARD_X][BOARD_Y] = {0};

    for (int i = 0; i < BOARD_X; i++)
    {
        for (int j = 0; j < BOARD_Y; j++)
        {
            if (0 == mach_lable[i][j])
            {
                if (MACHID_SOLIDER == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_SOLIDER;
                            pos->pieces[k].n = 1;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            mach_lable[i][j] = 1;
                            break;
                        }
                    }
                }

                if (MACHID_GENERAL_VERT == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_GENERAL_VERT;
                            pos->pieces[k].n = 2;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i + 1;
                            pos->pieces[k].blocks[1].y = j;
                            mach_lable[i][j] = 1;
                            mach_lable[i + 1][j] = 1;
                            break;
                        }
                    }
                }

                if (MACHID_GENERAL_HORI == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_GENERAL_HORI;
                            pos->pieces[k].n = 2;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i;
                            pos->pieces[k].blocks[1].y = j + 1;
                            mach_lable[i][j] = 1;
                            mach_lable[i][j + 1] = 1;
                            break;
                        }
                    }
                }

                if (MACHID_CAO == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_CAO;
                            pos->pieces[k].n = 4;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            pos->pieces[k].blocks[1].x = i;
                            pos->pieces[k].blocks[1].y = j + 1;
                            pos->pieces[k].blocks[2].x = i + 1;
                            pos->pieces[k].blocks[2].y = j;
                            pos->pieces[k].blocks[3].x = i + 1;
                            pos->pieces[k].blocks[3].y = j + 1;
                            mach_lable[i][j] = 1;
                            mach_lable[i][j + 1] = 1;
                            mach_lable[i + 1][j] = 1;
                            mach_lable[i + 1][j + 1] = 1;
                            break;
                        }
                    }
                }

                if (MACHID_NOT_USED == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_EMPTY;
                            pos->pieces[k].n = 1;
                            pos->pieces[k].blocks[0].x = i;
                            pos->pieces[k].blocks[0].y = j;
                            mach_lable[i][j] = 1;
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
    int mat[BOARD_X][BOARD_Y] = {0};

    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        PIECE *p = &pos->pieces[i];
        for (int j = 0; j < p->n; j++)
        {
            BLOCK *b = &p->blocks[j];
            
            if(MACHID_EMPTY == p->mat_id)
            {
                // mat[b->x][b->y] = MACHID_NOT_USED;
                continue;
            }
            else
            {
                mat[b->x][b->y] = p->mat_id;
            }
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
    POSITION *arr = (POSITION *)calloc(MAX_POSSIBLE_NEIGHBOURS , sizeof(POSITION));
    *n = 0;
    for (int i = 0; i < PIECES_IN_POSITION; i++)
    {
        if(!p->pieces[i].n)
            break;
        
        if(MACHID_EMPTY == p->pieces[i].mat_id)
        {
            continue;
        }

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


int is_symmetric(POSITION *pos)
{
    unsigned int index = 0;
    unsigned int symmetric = 0;
    POSITION visit_pos = {0};


    while (index < rear)
    {
        symmetric = 0;
        position_from_visit_map(&visit_pos,&g_visited_map[index]);
        for (int i = 0; i < BOARD_X; i++)
        {
            if ((symmetric != 0) && (symmetric % BOARD_Y != 0))
                break;

            for (int j = 0; j < BOARD_Y; j++)
            {
                if (visit_pos.mat[i][j] == pos->mat[i][BOARD_Y - j - 1])
                {
                    symmetric++;
                }
                else
                {
                    break;
                }
            }
        }
        if (symmetric == BOARD_X * BOARD_Y)
        {
            return 1;
        }

        index++;
    }

    return 0;
}

// int is_symmetric(POSITION *pos)
// {
//     VISIT_TABLE *p = g_lq.head;
//     int symmetric = 0;

//     while (p != NULL)
//     {
//         symmetric = 0;
//         for (int i = 0; i < BOARD_X; i++)
//         {
//             if ((symmetric != 0) && (symmetric % BOARD_Y != 0))
//                 break;
//             for (int j = 0; j < BOARD_Y; j++)
//             {
//                 if (p->visited[i][j] == pos->mat[i][BOARD_Y - j - 1])
//                 {
//                     symmetric++;
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }
//         }
//         if (symmetric == BOARD_X * BOARD_Y)
//         {
//             return 1;
//         }

//         p = p->next;
//     }

//     return 0;
// }

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
    unsigned int index = 0;
    // VISIT_MAP vmap={0};
    POSITION visit_pos = {0};
    int equal = 1;

    while (index<rear)
    {
        // visit_map_from_position(&vmap,pos);
        position_from_visit_map(&visit_pos,&g_visited_map[index]);

        if (0 != memcmp(visit_pos.mat, pos->mat, BOARD_X * BOARD_Y))
            equal = 0;
        else
            equal = 1;

        if (equal)
            return 1;

        index++;
    }
    return 0;
}

// int is_visited(POSITION *pos)
// {
//     VISIT_TABLE *p = g_lq.head;
//     int equal = 1;

//     while (p != NULL)
//     {
//         if (0 != memcmp(p->visited, pos->mat, BOARD_X * BOARD_Y))
//             equal = 0;
//         else
//             equal = 1;

//         if (equal)
//             return 1;

//         p = p->next;
//     }
//     return 0;
// }

void print_position(POSITION *pos)
{
    for (int i = 0; i < BOARD_X; i++)
    {
        for (int j = 0; j < BOARD_Y; j++)
            printf("%d ", pos->mat[i][j]);
        printf("\n");
    }

    printf("\n");
}

int is_good(POSITION *pos)
{
    BLOCK b1 = {3, 1};
    BLOCK b2 = {4, 2};
    int pieces_index = 0;

    for (int i = 0; i < MAX_POSSIBLE_NEIGHBOURS; i++)
    {
        if (MACHID_CAO == pos->pieces[i].mat_id)
        {
            pieces_index = i;
            break;
        }
    }
    if (is_block_in_piece(b1, &pos->pieces[pieces_index]) && is_block_in_piece(b2, &pos->pieces[pieces_index]))
        return 1;

    return 0;
}

void print_next_step_map_solution(VISIT_MAP *map)
{
    POSITION pos = {0};
    VISIT_MAP cur_map = {0};
    int parent_index = 0; //(map->map_high>>16)&0xFFFF;

    memcpy(&cur_map, map, sizeof(VISIT_MAP));
    parent_index = (cur_map.map_high >> 16) & 0xFFFF;
    while (parent_index)
    {
        position_from_visit_map(&pos, map);
        print_position(&pos);
        memcpy(&cur_map, &g_visited_map[parent_index], sizeof(VISIT_MAP));
        parent_index = (cur_map.map_high >> 16) & 0xFFFF;
    }
    print_position(&pos);
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
        // print_position(p_vis);
        p_vis = p_vis->parent;
    }
    // print_position(p_vis);
}

int get_visited_num(void)
{
    return n_visited;
}
