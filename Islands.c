#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ROWS 1000
#define MAX_COLS 1000

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

void init_island(Island* island);
void add_point(Island* island, int x, int y);
void free_island(Island* island);
int** read_matrix(const char* filename, int rows, int cols);
void bfs(int** matrix, int rows, int cols, int start_x, int start_y, int** visited, Island* island);
Island** find_islands(int** matrix, int rows, int cols, int* island_count);
void display_island(int rows, int cols, Island* island, const char* title);
void print_matrix(int** matrix, int rows, int cols);
IslandStats compute_stats(Island* island, int rows, int cols);
int compare_stats(const void* a, const void* b);
void append_node(IslandNode** head, IslandStats stats);
int list_length(IslandNode* head);
IslandStats* list_to_array(IslandNode* head, int length);
void free_list(IslandNode* head);
void print_areas_list(IslandNode* head);

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
}

Island** find_islands(int** matrix, int rows, int cols, int* island_count) {
    int** visited = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        visited[i] = (int*)calloc(cols, sizeof(int));
    }
    
    Island** islands = (Island**)malloc(MAX_ROWS * sizeof(Island*));
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
    printf("\n Исходная матрица:\n");
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
    printf("\n=== Список площадей всех островов ===\n");
    while (head != NULL) {
        printf("Остров %d: площадь = %d\n", num++, head->stats.area);
        head = head->next;
    }
}

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
        printf("Острова не найдены.\n");
        for (int i = 0; i < rows; i++) free(matrix[i]);
        free(matrix);
        return 0;
    }

    for (int i = 0; i < island_count; i++) {
        stats = compute_stats(islands[i], rows, cols);
        append_node(&stats_list, stats);
    }
    print_areas_list(stats_list);
    len = list_length(stats_list);
    stats_array = list_to_array(stats_list, len);
    qsort(stats_array, len, sizeof(IslandStats), compare_stats);
    display_island(rows, cols, stats_array[0].island_ref, "Самый большой остров");
    display_island(rows, cols, stats_array[len - 1].island_ref, "Самый маленький остров");
    printf("Statistics of islands");
    current = stats_list;
    idx = 1;
    while (current != NULL) {
        printf("Остров %d: площадь=%d, центр=(%.2f, %.2f), размеры=%dx%d\n",
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