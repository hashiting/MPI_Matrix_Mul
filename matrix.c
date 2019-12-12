#include<iostream>
#include<mpi.h>
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
    for(int i = 0;i < row;i++){
        matrix[i] = (int *)malloc(sizeof(int)*col);
    }
    return matrix;
}

int** generate_matrix(int row,int col,int range){
    int **matrix = generate_space(row,col);
    srand(time(NULL) + rand());
    for(int i = 0;i < row;i++){
        for(int j = 0;j < col;j++){
            matrix[i][j] = rand()%range;
        }
    }
    return matrix;
}

void print_matrx(int **a,int row,int col)
{
    for(int i = 0; i < row; i++)
    {
        for(int j = 0; j < col; j++)
        {
            printf("%d ",a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int** matrix_multiple(int **left, int **right,int row1,int col1,int row2,int col2){
    if(col1 != row2){
        PANIC("dimension not match");
        return;
    }

    int **result = generate_space(row1,col2);
    memset(result,0,sizeof(result));
    for(int i = 0;i < row1;i++){
        for(int j = 0;j < col2;j++){
            for(int k = 0;k < row2;k++){
                result[i][j] += left[i][k] * right[k][j];//this may be slow because the memory is not continuous
            }
        }
    }

    return result;
}
void block_scatter_matrix(int **matrix,int col,int row,int num_process,int** my_buffer){

}

void preprocessing_matrix(int **matrix,int block_size,int row,int col){

}

void cannon(){

}

int main(int argv,char** argc){
    int size,rank,dest;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(comm,&size);
    MPI_Comm_rank(comm,&rank);
    if(rank == 0)
        printf("we have %d process\n",size);
    int block_size = sqrt(size);
    
    if(rank == 0){
        int **matrix1 = generate_matrix(ROW1,COL1,20);
        int **matrix2 = generate_matrix(ROW2,COL2,10);
    }

    preprocessing_matrix(matrix1,block_size,ROW1,COL1);//shift to left and up
    preprocessing_matrix(matrix2,block_size,ROW2,COL2);//may be slower than communication

    int matrix1_row = ROW1/block_size;
    int matrix1_col = COL1/block_size;
    int matrix2_row = matrix1_col;
    int matrix2_col = COL2/block_size;

    int **A = generate_space(matrix1_row,matrix1_col);
    int **B = generate_space(matrix2_row,matrix2_col);
    int **C = generate_space(matrix1_row,matrix2_col);
    int **A_Buf = generate_space(matrix1_row,matrix1_col);
    int **B_Buf = generate_space(matrix2_row,matrix2_col);

    if(rank == 0){
        block_scatter_matrix(matrix1,matrix1_row,matrix1_col,size,A);
        block_scatter_matrix(matrix2,matrix2_row,matrix2_col,size,B);//send i^th block to i^th node
    }
    else{
        MPI_Recv();
        MPI_Recv();
    }

    MPI_Barrier(MPI_COMM_WORLD);//sync

    Cannon(); // do computing and shifting

    if(rank == 0){
        gather_matrix();//gathering the result
    }


}
