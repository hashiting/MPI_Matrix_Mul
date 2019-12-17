#include<math.h>
#include<stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define ROW1 9
#define COL1 9
#define ROW2 9
#define COL2 9

int **generate_space(int row,int col){
    int **matrix = (int **)malloc(sizeof(int *)* row);
    int i = 0;
	for(;i < row;i++){
        matrix[i] = (int *)malloc(sizeof(int)*col);
    }
    return matrix;
}

int** generate_matrix(int row,int col,int range){
    int **matrix = (int **)malloc(sizeof(int *)* row);
    int i = 0;
	for(;i < row;i++){
        matrix[i] = (int *)malloc(sizeof(int)*col);
    }
    srand(time(NULL) + rand());
    i = 0;
    int j = 0;
	for(;i < row;i++){
        for(;j < col;j++){
            matrix[i][j] = rand()%range;
            printf("%d ",(*(matrix+i))[j]);
        }
        printf("\n");
    }
    printf("\n");
    return matrix;
}

void print_matrx(int **a,int row,int col)
{
	int i = 0;
	int j = 0;
    for(; i < row; i++)
    {
        for(; j < col; j++)
        {
            printf("%d ",(*(a + i))[j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(){
	
	int **matrix1 = generate_matrix(ROW1,COL1,20);
    //int **matrix2 = generate_matrix(ROW2,COL2,10);
    print_matrx(matrix1,ROW1,COL1);    
}
