#include<iostream>
#include<mpi.h>
#include<math.h>
#include<stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define ROW1 4
#define COL1 5
#define ROW2 5
#define COL2 4

int** generate_matrix(int row,int col,int range){
    int **matrix;
    matrix = (int **)malloc(sizeof(int *)* row);
    for(int i = 0;i < col;i++){
        matrix[i] = (int *)malloc(sizeof(int) * col);
    }
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




int main(int argv,char** argc){
    int size,rank,dest;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(comm,&size);
    MPI_Comm_rank(comm,&rank);

    if(rank == 0){
        int **matrix1 = generate_matrix(ROW1,COL1,50);
        int **matrix2 = generate_matrix(ROW2,COL2,40);
    }
    MPI_scatter();

    MPI_barrier();
    MPI_gather();

}