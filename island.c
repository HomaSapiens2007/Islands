#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* points;
    int size;
    int capacity;
} Island;

typedef struct {
    int area;
    double center_r;
    double center_c;
    int height;
    int width;
    Island* island_ref;
} IslandStats;

typedef struct IslandNode {
    IslandStats stats;
    struct IslandNode* next;
} IslandNode;

int main(void) {
    int rows, cols;
    int** matrix;
    int island_count;
    Island** islands;
    IslandNode* stats_list;
    IslandStats* stats_array;
    int len;
    IslandNode* current;
    int idx;
    IslandStats stats;

    stats_list = NULL;
    stats_array = NULL;
    printf("Enter numbers of rows and cols");
    scanf("%d %d", &rows, &cols);

    matrix = read_matrix("Islands.dat", rows, cols);
    if (matrix==NULL){
        printf("Error of reading");
        return 1;
    }
    
    if (!matrix) {
        return 1;
    }
    
    islands = find_islands(matrix, rows, cols, &island_count);
    
    if (island_count == 0) {
        printf("Islands not founded.\n");
        for (int i = 0; i < rows; i++) free(matrix[i]);
        free(matrix);
        return 0;
    }
    print_matrix(matrix, rows, cols);
    for (int i = 0; i < island_count; i++) {
        stats = compute_stats(islands[i], rows, cols);
        append_node(&stats_list, stats);
    }
    print_areas_list(stats_list);
    len = list_length(stats_list);
    stats_array = list_to_array(stats_list, len);
    qsort(stats_array, len, sizeof(IslandStats), compare_stats);
    display_island(rows, cols, stats_array[0].island_ref, "The biggest island");
    display_island(rows, cols, stats_array[len - 1].island_ref, "The smallest island");
    printf("Stats: \n");
    current = stats_list;
    idx = 1;
    while (current != NULL) {
        printf("Island %d: area=%d, center=(%.2f, %.2f), size=%dx%d\n",
               idx++, current->stats.area, current->stats.center_r,
               current->stats.center_c, current->stats.height, current->stats.width);
        current = current->next;
    }
    free(stats_array);
    free_list(stats_list);
    
    for (int i = 0; i < island_count; i++) {
        free_island(islands[i]);
        free(islands[i]);
    }
    free(islands);
    
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    
    return 0;
    
    return 0;
}
