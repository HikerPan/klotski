#include "string.h"
#include "klotski_position.h"

VISIT_MAP *g_visited_map = NULL;
int n_visited = 0;

int is_block_in_piece(BLOCK b, PIECE *p)
{
    if (MACHID_EMPTY == p->mat_id)
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
    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        if ((MACHID_EMPTY == p->pieces[i].mat_id) && (p->pieces->blocks[0].x == b.x) && (p->pieces->blocks[0].y == b.y))
        {
            return 1;
        }
    }

    return 0;
}

int is_block_in_any_piece(BLOCK b, POSITION *p)
{
    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        if (MACHID_EMPTY == p->pieces[i].mat_id)
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
 * 是否可以移动
 */
int can_move(POSITION *pos, PIECE *piece, BLOCK (*d)(BLOCK))
{
    for (int i = 0; i < piece->n; i++)
    {
        BLOCK b = piece->blocks[i];
        // 按照d方法走完后的block坐标
        BLOCK next_block = d(b);

        /**
         * 目标坐标非法
         * 目标坐标在其他pieces中，注意要去掉空位置的pieces
         * 不在自己的pieces中，注意空位置的pieces不做判断
         */
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

static void map_set(VISIT_MAP *map, unsigned int mach_id, int index)
{
    // int id = mach_id;
    // id = id << (index * 3);
    if (index < 10)
    {
        map->map_low &= (~(0x07 << (index * 3)));
        map->map_low |= (mach_id << (index * 3));
    }
    else if (index > 10)
    {
        map->map_high &= (~(0x07 << ((index - 11) * 3 + 2)));
        map->map_high |= (mach_id << ((index - 11) * 3 + 2));
    }
    else
    {
        map->map_low &= (~(0x07 << (10 * 3)));
        map->map_high &= 0xFFFFFFFE;

        map->map_low |= ((mach_id & 0x03) << (10 * 3));
        map->map_high |= (mach_id >> 2);
    }
}

/***
 * @description:
 * @param {VISIT_MAP} *map
 * @param {POSITION} *pos
 * @return {*}
 */
int visit_map_and_pieces_from_matirx(VISIT_MAP *map, POSITION *pos)
{
    unsigned char check_flag[BOARD_Y][BOARD_X] = {0};
    int count = 0;

    map->map_low = 0xFFFFFFFF;
    map->map_high = 0xFFFF;

    // 遍历棋盘
    for (int i = 0; i < BOARD_Y; i++)
    {
        for (int j = 0; j < BOARD_X; j++)
        {
            // check_flag标记该棋盘坐标是否已经被处理过
            if (!check_flag[i][j])
            {
                switch (pos->mat[i][j])
                {
                case MACHID_EMPTY:
                    // 根据棋盘设置map结构
                    map_set(map, MACHID_EMPTY, count);

                    // 同时更新自己的pieces
                    pos->pieces[count].id = count + 1;
                    pos->pieces[count].mat_id = MACHID_EMPTY;
                    pos->pieces[count].n = 1;
                    pos->pieces[count].blocks[0].x = j;
                    pos->pieces[count].blocks[0].y = i;

                    check_flag[i][j] = 1;
                    count++;
                    break;

                case MACHID_SOLIDER:
                    map_set(map, MACHID_SOLIDER, count);

                    pos->pieces[count].id = count + 1;
                    pos->pieces[count].mat_id = MACHID_SOLIDER;
                    pos->pieces[count].n = 1;
                    pos->pieces[count].blocks[0].x = j;
                    pos->pieces[count].blocks[0].y = i;

                    check_flag[i][j] = 1;
                    count++;
                    break;

                case MACHID_GENERAL_VERT:
                    map_set(map, MACHID_GENERAL_VERT, count);

                    pos->pieces[count].id = count + 1;
                    pos->pieces[count].mat_id = MACHID_GENERAL_VERT;
                    pos->pieces[count].n = 2;
                    pos->pieces[count].blocks[0].x = j;
                    pos->pieces[count].blocks[0].y = i;
                    pos->pieces[count].blocks[1].x = j;
                    pos->pieces[count].blocks[1].y = i + 1;

                    check_flag[i][j] = 1;
                    check_flag[i + 1][j] = 1;
                    count++;
                    break;

                case MACHID_GENERAL_HORI:
                    map_set(map, MACHID_GENERAL_HORI, count);

                    pos->pieces[count].id = count + 1;
                    pos->pieces[count].mat_id = MACHID_GENERAL_HORI;
                    pos->pieces[count].n = 2;
                    pos->pieces[count].blocks[0].x = j;
                    pos->pieces[count].blocks[0].y = i;
                    pos->pieces[count].blocks[1].x = j + 1;
                    pos->pieces[count].blocks[1].y = i;

                    check_flag[i][j] = 1;
                    check_flag[i][j + 1] = 1;
                    count++;
                    break;

                case MACHID_CAO:
                    map_set(map, MACHID_CAO, count);

                    pos->pieces[count].id = count + 1;
                    pos->pieces[count].mat_id = MACHID_CAO;
                    pos->pieces[count].n = 4;

                    pos->pieces[count].blocks[0].x = j;
                    pos->pieces[count].blocks[0].y = i;

                    pos->pieces[count].blocks[1].x = j + 1;
                    pos->pieces[count].blocks[1].y = i;

                    pos->pieces[count].blocks[2].x = j;
                    pos->pieces[count].blocks[2].y = i + 1;

                    pos->pieces[count].blocks[3].x = j + 1;
                    pos->pieces[count].blocks[3].y = i + 1;

                    check_flag[i][j] = 1;
                    check_flag[i + 1][j] = 1;
                    check_flag[i][j + 1] = 1;
                    check_flag[i + 1][j + 1] = 1;
                    count++;
                    break;

                default:
                    printf("the %d is not valid.", pos->mat[i][j]);
                    break;
                }
            }
        }
    }

    return 1;
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

    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        // map_set(map, pos->pieces[i].mat_id, i);
        if (!pos->pieces[i].n)
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
 * @description:
 * @param {POSITION} *pos
 * @param {VISIT_MAP} *map
 * @return {*}
 */
int position_from_visit_map(POSITION *pos, VISIT_MAP *map)
{
    unsigned char used[BOARD_Y][BOARD_X] = {0};
    int map_bit = 0;
    int found = 0;

    memset(pos, 0, sizeof(POSITION));

    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
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
            map_bit = (map->map_low >> (3 * 10)) & 0x03;
            map_bit |= ((map->map_high & 0x01) << 2);
        }

        if (0x07 == map_bit)
            break;

        found = 0;
        switch (map_bit)
        {
        case MACHID_SOLIDER:
            for (int j = 0; j < BOARD_Y; j++)
            {
                for (int k = 0; k < BOARD_X; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_SOLIDER;
                        used[j][k] = 1;
                        found = 1;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }

            break;
        case MACHID_GENERAL_VERT:
            for (int j = 0; j < BOARD_Y; j++)
            {
                for (int k = 0; k < BOARD_X; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_GENERAL_VERT;
                        pos->mat[j + 1][k] = MACHID_GENERAL_VERT;

                        used[j][k] = 1;
                        used[j + 1][k] = 1;
                        found = 1;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }
            break;
        case MACHID_GENERAL_HORI:
            for (int j = 0; j < BOARD_Y; j++)
            {
                for (int k = 0; k < BOARD_X; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_GENERAL_HORI;
                        pos->mat[j][k + 1] = MACHID_GENERAL_HORI;
                        used[j][k] = 1;
                        used[j][k + 1] = 1;
                        found = 1;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }
            break;
        case MACHID_CAO:
            for (int j = 0; j < BOARD_Y; j++)
            {
                for (int k = 0; k < BOARD_X; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_CAO;
                        pos->mat[j][k + 1] = MACHID_CAO;
                        pos->mat[j + 1][k] = MACHID_CAO;
                        pos->mat[j + 1][k + 1] = MACHID_CAO;

                        used[j][k] = 1;
                        used[j][k + 1] = 1;
                        used[j + 1][k] = 1;
                        used[j + 1][k + 1] = 1;

                        found = 1;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }
            break;
        case MACHID_EMPTY:
            for (int j = 0; j < BOARD_Y; j++)
            {
                for (int k = 0; k < BOARD_X; k++)
                {
                    if (!used[j][k])
                    {
                        pos->mat[j][k] = MACHID_EMPTY;
                        used[j][k] = 1;
                        found = 1;
                        break;
                    }
                }
                if (found)
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

void pieces_from_matrix(POSITION *pos)
{
    unsigned char mach_lable[BOARD_Y][BOARD_X] = {0};

    memset(pos->pieces, 0, MAX_POSSIBLE_PIECES_IN_POSITION * sizeof(PIECE));

    for (int i = 0; i < BOARD_Y; i++)
    {
        for (int j = 0; j < BOARD_X; j++)
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
                            pos->pieces[k].blocks[0].x = j;
                            pos->pieces[k].blocks[0].y = i;
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
                            pos->pieces[k].blocks[0].x = j;
                            pos->pieces[k].blocks[0].y = i;
                            pos->pieces[k].blocks[1].x = j;
                            pos->pieces[k].blocks[1].y = i + 1;
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
                            pos->pieces[k].blocks[0].x = j;
                            pos->pieces[k].blocks[0].y = i;
                            pos->pieces[k].blocks[1].x = j + 1;
                            pos->pieces[k].blocks[1].y = i;
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
                            pos->pieces[k].blocks[0].x = j;
                            pos->pieces[k].blocks[0].y = i;
                            pos->pieces[k].blocks[1].x = j + 1;
                            pos->pieces[k].blocks[1].y = i;
                            pos->pieces[k].blocks[2].x = j;
                            pos->pieces[k].blocks[2].y = i + 1;
                            pos->pieces[k].blocks[3].x = j + 1;
                            pos->pieces[k].blocks[3].y = i + 1;
                            mach_lable[i][j] = 1;
                            mach_lable[i][j + 1] = 1;
                            mach_lable[i + 1][j] = 1;
                            mach_lable[i + 1][j + 1] = 1;
                            break;
                        }
                    }
                }

                if (MACHID_EMPTY == pos->mat[i][j])
                {
                    for (int k = 0; k < MAX_POSSIBLE_NEIGHBOURS; k++)
                    {
                        if (0 == pos->pieces[k].id)
                        {
                            pos->pieces[k].id = k + 1;
                            pos->pieces[k].mat_id = MACHID_EMPTY;
                            pos->pieces[k].n = 1;
                            pos->pieces[k].blocks[0].x = j;
                            pos->pieces[k].blocks[0].y = i;
                            mach_lable[i][j] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void matrix_from_pieces(POSITION *pos)
{
    int mat[BOARD_X][BOARD_Y] = {0};

    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        PIECE *p = &pos->pieces[i];
        for (int j = 0; j < p->n; j++)
        {
            BLOCK *b = &p->blocks[j];

            if (MACHID_EMPTY == p->mat_id)
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

    // 遍历所有pieces
    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        PIECE *tmp = &new_pos.pieces[i];
        if (tmp->id == p->id)
        {
            // 先根据原先的坐标，把对应位置写为空旗子
            for (int j = 0; j < tmp->n; j++)
            {
                new_pos.mat[tmp->blocks[j].y][tmp->blocks[j].x] = MACHID_EMPTY;
            }
            // 更改pieces中的坐标
            for (int j = 0; j < tmp->n; j++)
            {
                BLOCK b = d(tmp->blocks[j]);
                tmp->blocks[j] = b;
            }
            // 根据新的pieces中在坐标，写match_id
            for (int j = 0; j < tmp->n; j++)
            {
                new_pos.mat[tmp->blocks[j].y][tmp->blocks[j].x] = tmp->mat_id;
            }

            break;
        }
    }
    // 根据最新的matrix跟心pieces
    pieces_from_matrix(&new_pos);

    // matrix_from_pieces(&new_pos);
    return new_pos;
}

BLOCK up(BLOCK b)
{
    BLOCK tmp = {b.x, b.y - 1};
    return tmp;
}

BLOCK down(BLOCK b)
{
    BLOCK tmp = {b.x, b.y + 1};
    return tmp;
}

BLOCK left(BLOCK b)
{
    BLOCK tmp = {b.x - 1, b.y};
    return tmp;
}

BLOCK right(BLOCK b)
{
    BLOCK tmp = {b.x + 1, b.y};
    return tmp;
}

// int find_empty_move_count(int x, int y)
// {
// }

// /**
//  * 利用空旗子位置去找可能的走法，看每个空旗子的上下左右可能的走法
//  *
//  */
// POSITION *all_neighbours_by_empty(POSITION *p, int *n)
// {
//     BLOCK b = {0};

//     for (int i = 0; i < BOARD_Y; i++)
//     {
//         for (int j = 0; j < BOARD_X; j++)
//         {
//             if (MACHID_EMPTY == p->mat[i][j])
//             {
//                 // find_empty_move_count(i,j);
//                 // 上面的棋子
//                 b.x = i;
//                 b.y = j - 1;
//                 if (is_block_in_range(b))
//                 {
//                     if (MACHID_SOLIDER == p->mat[b.y][b.x])
//                     {
//                         *n++;
//                     }
//                 }
//             }
//         }
//     }
// }

POSITION *all_neighbours(POSITION *p, int *n)
{
    POSITION *arr = (POSITION *)calloc(MAX_POSSIBLE_NEIGHBOURS, sizeof(POSITION));
    *n = 0;

    // 根据matrix更新pieces
    pieces_from_matrix(p);

    for (int i = 0; i < MAX_POSSIBLE_PIECES_IN_POSITION; i++)
    {
        // block的个数为0，表示已经结束
        if (!p->pieces[i].n)
            break;

        // 对empty棋子不找可能走法
        if (MACHID_EMPTY == p->pieces[i].mat_id)
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
        position_from_visit_map(&visit_pos, &g_visited_map[index]);
        for (int i = 0; i < BOARD_Y; i++)
        {
            if ((symmetric != 0) && (symmetric % BOARD_X != 0))
                break;

            for (int j = 0; j < BOARD_X; j++)
            {
                if (visit_pos.mat[i][j] == pos->mat[i][BOARD_X - j - 1])
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

int is_visited(POSITION *pos)
{
    unsigned int index = 0;
    // VISIT_MAP vmap={0};
    POSITION visit_pos = {0};
    int equal = 1;

    while (index < rear)
    {
        // visit_map_from_position(&vmap,pos);
        position_from_visit_map(&visit_pos, &g_visited_map[index]);

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

void print_position(POSITION *pos)
{
    for (int i = 0; i < BOARD_Y; i++)
    {
        for (int j = 0; j < BOARD_X; j++)
            printf("%d ", pos->mat[i][j]);
        printf("\n");
    }

    printf("\n");
}

int is_good(POSITION *pos)
{
    if ((pos->mat[3][1] == MACHID_CAO) && (pos->mat[3][1] == pos->mat[3][2]) && (pos->mat[3][1] == pos->mat[4][1]) && (pos->mat[3][1] == pos->mat[4][2]))
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

int get_visited_num(void)
{
    return n_visited;
}
