#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
    FILE *fin;
    int rows, cols;
    fin = fopen("Islands.dat", "w");
    if (!fin){
        printf("Error");
        return 1;
    }
    scanf("%d %d", &rows, &cols);
    srand(time(NULL));
    for(int i=0; i<rows;i++){
        for (int j=0;j<cols;j++){
            if(rand()%5==0){
                fprintf(fin, "%d ", 1);
            }
            else{
                fprintf(fin, "%d ", 0);
            }
        }
        fprintf(fin, "\n");
    }
    fclose(fin);
    return 0;
}