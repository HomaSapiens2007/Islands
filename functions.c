#include <stdio.h>
#include <stdlib.h>
#include "calls.h"
#include <string.h>
#include <math.h>

void init_island(Island* island) {
    island->capacity = 10;
    island->size = 0;
    island->points = (Point*)malloc(island->capacity * sizeof(Point));
}

void add_point(Island* island, int x, int y) {
    if (island->size >= island->capacity) {
        island->capacity *= 2;
        island->points = (Point*)realloc(island->points, island->capacity * sizeof(Point));
    }
    island->points[island->size].x = x;
    island->points[island->size].y = y;
    island->size++;
}

void free_island(Island* island) {
    free(island->points);
    island->points = NULL;
    island->size = 0;
    island->capacity = 0;
}

int** read_matrix(const char* filename, int rows, int cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error 1");
        return NULL;
    }
    
    int** matrix = (int**)malloc(rows*sizeof(int*));
    for (int i=0;i<rows;i++){
        matrix[i] = (int*)malloc(cols*sizeof(int));
        for (int j=0;j<cols;j++){
            if (fscanf(file, "%d", &matrix[i][j])!=1){
                printf("Error 2");
                return NULL;
            }
        }
    }
    
    fclose(file);
    return matrix;
}

void bfs(int** matrix, int rows, int cols, int start_x, int start_y, 
         int** visited, Island* island) {
    int *queue_x, *queue_y;
    queue_x = (int*)malloc(cols*rows*sizeof(int));
    queue_y = (int*)malloc(cols*rows*sizeof(int));
    int front = 0;
    int last = 0;
    int x,y, nx, ny;
    
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    queue_x[last] = start_x;
    queue_y[last] = start_y;
    last++;
    visited[start_x][start_y] = 1;
    
    while (front < last) {
        x = queue_x[front];
        y = queue_y[front];
        front++;
        
        add_point(island, x, y);
        
        for (int i = 0; i < 4; i++) {
            nx = x + dx[i];
            ny = y + dy[i];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && 
                matrix[nx][ny] == 1 && !visited[nx][ny]) {
                visited[nx][ny] = 1;
                queue_x[last] = nx;
                queue_y[last] = ny;
                last++;
            }
        }
    }
    free(queue_x);
    free(queue_y);
}

Island** find_islands(int** matrix, int rows, int cols, int* island_count) {
    int** visited = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        visited[i] = (int*)calloc(cols, sizeof(int));
    }
    
    Island** islands = (Island**)malloc((rows+1)*(cols+1)/4 * sizeof(Island*));
    *island_count = 0;
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == 1 && !visited[i][j]) {
                islands[*island_count] = (Island*)malloc(sizeof(Island));
                init_island(islands[*island_count]);
                bfs(matrix, rows, cols, i, j, visited, islands[*island_count]);
                (*island_count)++;
            }
        }
    }
    
    for (int i = 0; i < rows; i++) {
        free(visited[i]);
    }
    free(visited);
    
    return islands;
}

void display_island(int rows, int cols, Island* island, const char* title) {
    char** grid;
    int i, j, x, y;
    
    grid = (char**)malloc(rows * sizeof(char*));
    for (i = 0; i < rows; i++) {
        grid[i] = (char*)malloc(cols * sizeof(char));
        for (j = 0; j < cols; j++) {
            grid[i][j] = '0';
        }
    }
    
    for (i = 0; i < island->size; i++) {
        x = island->points[i].x;
        y = island->points[i].y;
        grid[x][y] = '1';
    }
    
    printf("\n%s:\n", title);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    
    for (i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
}

void print_matrix(int** matrix, int rows, int cols) {
    printf("\nYour matrix:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

IslandStats compute_stats(Island* island, int rows, int cols) {
    IslandStats stats;
    double sum_r = 0, sum_c = 0;
    int min_r, min_c, max_r, max_c;
    int i, x, y;
    
    min_r = rows;
    min_c = cols;
    max_r = -1;
    max_c = -1;
    
    stats.area = island->size;
    
    for (i = 0; i < island->size; i++) {
        x = island->points[i].x;
        y = island->points[i].y;
        sum_r += x;
        sum_c += y;
        if (x < min_r) min_r = x;
        if (x > max_r) max_r = x;
        if (y < min_c) min_c = y;
        if (y > max_c) max_c = y;
    }
    
    stats.center_r = sum_r / island->size;
    stats.center_c = sum_c / island->size;
    stats.height = max_r - min_r + 1;
    stats.width = max_c - min_c + 1;
    stats.island_ref = island;
    
    return stats;
}

int compare_stats(const void* a, const void* b) {
    IslandStats* sa = (IslandStats*)a;
    IslandStats* sb = (IslandStats*)b;
    return sb->area - sa->area;
}

void append_node(IslandNode** head, IslandStats stats) {
    IslandNode* new_node;
    IslandNode* current;
    
    new_node = (IslandNode*)malloc(sizeof(IslandNode));
    new_node->stats = stats;
    new_node->next = NULL;
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

int list_length(IslandNode* head) {
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

IslandStats* list_to_array(IslandNode* head, int length) {
    IslandStats* arr;
    int i;
    
    arr = (IslandStats*)malloc(length * sizeof(IslandStats));
    for (i = 0; i < length; i++) {
        arr[i] = head->stats;
        head = head->next;
    }
    return arr;
}

void free_list(IslandNode* head) {
    IslandNode* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void print_areas_list(IslandNode* head) {
    int num = 1;
    printf("\n=== The list of arear of all islands ===\n");
    while (head != NULL) {
        printf("Island %d: area = %d\n", num++, head->stats.area);
        head = head->next;
    }
}
