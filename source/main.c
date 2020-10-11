#include "position.h"
#include "queue.h"

void bfs(POSITION root, int max_depth)
{
    QUEUE q = { NULL, -1, 0 };
    q.arr = malloc(MAX * sizeof(POSITION));

    int n;
    POSITION u;
    add(&q, root);
    add_to_visited(&root);

    while(get(&q, &u))
    {
        if(is_good(&u))
        {
            printf("Success!\n");
            print_solution(&u);
            free(q.arr);
            exit(0);
        }

        POSITION *arr = all_neighbours(&u, &n);
        for(int i = 0; i < n; i++)
        {
            if(!is_visited(&arr[i]))
            {
                arr[i].parent = &q.arr[q.front - 1];
                add(&q, arr[i]);
                add_to_visited(&arr[i]);
            }
        }
        free(arr);
    }
}

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
