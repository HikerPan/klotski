#include "klotski_position.h"
#include "klotski_list_queue.h"
#include "string.h"

void bfs(POSITION root, int max_depth);

int main()
{
    POSITION root =
    {
        {   { 2, 4, 4, 2},
            { 2, 4, 4, 2},
            { 2, 3, 3, 2},
            { 2, 1, 1, 2},
            { 1, 0, 0, 1}
        },
        {   {1, 2, 2, {{0, 0}, {1, 0}}},
            {2, 4, 4, {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
            {3, 2, 2, {{0, 3}, {1, 3}}},
            {4, 2, 2, {{2, 0}, {3, 0}}},
            {5, 3, 2, {{2, 1}, {2, 2}}},
            {6, 2, 2, {{2, 3}, {3, 3}}},
            {7, 1, 1, {{3, 1}}},
            {8, 1, 1, {{3, 2}}},
            {9, 1, 1, {{4, 0}}},
            {10, 1, 1, {{4, 3}}}
        }, NULL
    };

    bfs(root, 200);

    return 0;
}


void bfs(POSITION root, int max_depth)
{
    POSITION pos = {0};
    int n = 0;
    int real_n = 0;
    int current_depth = 0;
    int elements_till_next_level = 1;
    int elements_for_next_level = 0;
    VISIT_TABLE *vis_out = NULL;
    VISIT_TABLE *vis = (VISIT_TABLE *)calloc(1,sizeof(VISIT_TABLE));

    // 将POSITION结构体转化为VISIT_TABL后存入
    if(visit_table_from_position(vis,&root))
        list_queue_init(vis);

    // while(list_queue_out(vis_out))
    vis_out = list_queue_out();
    while(vis_out)
    {
        // VISIT_TABLE转为POSITION
        memset(&pos,0,sizeof(POSITION));
        position_from_visit_table(&pos, vis_out);

        // Cao 走到了出口位置
        if(is_good(&pos))
        {
            printf("Success!!total visited %d\n",get_visited_num());
            // 打印root的下一步走法
            print_next_step_solution(vis_out);

            exit(0);
        }

        POSITION *arr = all_neighbours(&pos, &n);
        real_n  = 0;
        for(int i = 0; i < n; i++)
        {
            if(!is_visited(&arr[i]))
            {
                vis = (VISIT_TABLE *)malloc(sizeof(VISIT_TABLE));
                if(NULL == vis)
                {
                    printf("can not malloc for dis.\r\n");
                    exit(0);
                }
                if(visit_table_from_position(vis,&arr[i]))
                {
                    vis->parent = vis_out;
                    list_queue_in(vis);
                    real_n++;
                } 
            }
        }

        free(arr);

        elements_for_next_level += real_n;
        int abc = 0;
        // if(current_depth == 87)
        //     printf("Current level: %d\n", current_depth);
        if(--elements_till_next_level == 0)
        {
            if(++current_depth > max_depth)
                return;
            printf("Current level: %d\n", current_depth);
            printf("New positions for level: %d\n", elements_for_next_level);
            elements_till_next_level = elements_for_next_level;
            elements_for_next_level = 0;
            // printf("free heap:%d\n", esp_get_free_heap_size());
        }

        vis_out = list_queue_out();
    }
}



// void bfs(POSITION root, int max_depth)
// {
//     QUEUE q = { NULL, -1, 0 };
//     q.arr = malloc(MAX * sizeof(POSITION));

//     POSITION u;
//     add(&q, root);
//     add_to_visited(&root);

//     int n, real_n;
//     int current_depth = 0;
//     int elements_till_next_level = 1;
//     int elements_for_next_level = 0;

//     while(get(&q, &u))
//     {
//         if(is_good(&u))
//         {
//             printf("Success!!\n");
//             print_solution(&u);
//             free(q.arr);
//             exit(0);
//         }

//         POSITION *arr = all_neighbours(&u, &n);
//         real_n  = 0;
//         for(int i = 0; i < n; i++)
//         {
//             if(!is_visited(&arr[i]))
//             {
//                 arr[i].parent = &q.arr[q.front - 1];
//                 add(&q, arr[i]);
//                 add_to_visited(&arr[i]);
//                 real_n++;
//             }
//         }
//         free(arr);
//         elements_for_next_level += real_n;
//         if(--elements_till_next_level == 0)
//         {
//             if(++current_depth > max_depth)
//                 return;
//             printf("Current level: %d\n", current_depth);
//             printf("New positions for level: %d\n", elements_for_next_level);
//             elements_till_next_level = elements_for_next_level;
//             elements_for_next_level = 0;
//         }
//     }
// }
