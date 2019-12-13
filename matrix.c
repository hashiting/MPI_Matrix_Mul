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

//cache effiency
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

void matrix_multiple(int **left, int **right,int **result,int row1,int col1,int row2,int col2){
    if(col1 != row2){
        PANIC("dimension not match");
        return;
    }

    //int **result = generate_space(row1,col2);
    //memset(result,0,sizeof(result));
    for(int i = 0;i < row1;i++){
        for(int j = 0;j < col2;j++){
            for(int k = 0;k < row2;k++){
                result[i][j] += left[i][k] * right[k][j];//this may be slow because the memory is not continuous//cache usage
            }
        }
    }
}

int** get_block(int row_i,int col_i,int row,int col,int **matrix){//index//size of block
    int **temp = generate_matrix(row,col);
    for(int i = row_i,int t1 = 0;i < row_i + row;i++,t1++){
        for(int j = col_i,int t2 = 0;j < col + col_i;j++,t2++){
            temp[t1][t2] = matrix[i][j];
        }
    }
    return temp;
}

void Set_block(int row_i,int col_i,int row,int col,int **result,int **small){
    for(int i = row_i,int t1 = 0;i < row_i + row;i++,t1++){
        for(int j = col_i,int t2 = 0;j < col + col_i;j++,t2++){
            result[i][j] = small[t1][t2];
        }
    }
}

void block_scatter_matrix(int **matrix,int row,int col,int block_size,int **Bufer,int tag){//size block
    for(int i = 0;i <block_size; i++){
        for(int j = 0;j < block_size;j++){
            if(i==0&&j==0){
                memcpy(Bufer,get_block(i,j,row,col,matrix),row*col*sizeof(int));
            } 
            else{
                MPI_Send(get_block(i,j,row,col,matrix),row*col,MPI_INT,i*block_size + j,tag,MPI_COMM_WORLD);
            }
        }
    }
}   

void preprocessing_matrix(int **left,int **left_Buf,int left_r,int left_c,int **right,int **right_Buf,int right_r,int right_c,int block_size,int rank){
    MPI_Status status;
    int row_i = rank/block_size;
    int col_i = rank%block_size;

    for(int i = 0;i <row_i; i++){
        MPI_Sendrecv(left,left_r*left_c,MPI_INT,get_left_index(rank,block_size),102,
                 left_Buf,left_r*left_c,MPI_INT,get_right_index(rank,block_size),102,MPI_COMM_WORLD,&status);
        memcpy(left,left_Buf,left_r*left_c*sizeof(int));
    }

    for(int j = 0;j <col_i; j++){
        MPI_Sendrecv(right,right_r*right_c,MPI_INT,get_up_index(rank,block_size),103,
                 right_Buf,right_r*right_c,MPI_INT,get_down_index(rank,block_size),103,MPI_COMM_WORLD,&status);
        memcpy(right,right_Buf,right_r*right_c*sizeof(int));
    }

}


int get_left_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return row*block_size + (col - 1)%current_rank;
}

int get_right_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return row*block_size + (col + 1)%current_rank;
}

int get_up_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return ((row-1)%block_size)*block_size + col;
}

int get_down_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return ((row+1)%block_size)*block_size + col;
}

void cannon(int **left,int **left_Buf,int left_r,int left_c,int **right,int **right_Buf,int right_r,int right_c,int **result,int block_size,int rank){//each step recesend//computer
    MPI_Status status;
    memset(result,0,sizeof(result));
    for(int i = 0;i < block_size;i++){
        matrix_multiple(left,right,result,left_r,left_c,right_r,right_c);

        MPI_Sendrecv(left,left_r*left_c,MPI_INT,get_left_index(rank,block_size),102,
                 left_Buf,left_r*left_c,MPI_INT,get_right_index(rank,block_size),102,MPI_COMM_WORLD,&status);

        
        MPI_Sendrecv(right,right_r*right_c,MPI_INT,get_up_index(rank,block_size),103,
                 right_Buf,right_r*right_c,MPI_INT,get_down_index(rank,block_size),103,MPI_COMM_WORLD,&status);

                 //copy data;
        memcpy(left,left_Buf,left_r*left_c*sizeof(int));
        memcpy(right,right_Buf,right_r*right_c*sizeof(int));
    }

    MPI_Send(result,left_r*right_c,MPI_INT,0,104,MPI_COMM_WORLD);
}

void gather_matrix(int **result,int row,int col,int block_size){
    MPI_Status status;
    int **temp = generate_space(row,col);
    for(int i = 0;i<block_size;i++){
        for(int j = 0;j<block_size;j++){
            MPI_Recv(temp,row*col,MPI_INT,i*block_size+j,104,MPI_COMM_WORLD,&status);
            Set_block(i*block_size,j*block_size,row,col,result,temp);
        }
    }
    printf("gathering done\n");
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
        int **result = generate_space(ROW1,COL2);
    }

    print_matrx(matrix1,ROW1,COL1);
    print_matrx(matrix2,ROW2,COL2);
    matrix_multiple(matrix1,matrix2,result,ROW1,COL1,ROW2,COL2);//calculate time
    print_matrx(result,ROW1,COL2)

    //preprocessing_matrix(matrix1,block_size,ROW1,COL1);//shift to left and up
    //preprocessing_matrix(matrix2,block_size,ROW2,COL2);//may be slower than communication

    int matrix1_row = ROW1/block_size;
    int matrix1_col = COL1/block_size;
    int matrix2_row = matrix1_col;
    int matrix2_col = COL2/block_size;

    int **A = generate_space(matrix1_row,matrix1_col);//
    int **B = generate_space(matrix2_row,matrix2_col);
    int **C = generate_space(matrix1_row,matrix2_col);
    int **A_Buf = generate_space(matrix1_row,matrix1_col);//buffer for communication
    int **B_Buf = generate_space(matrix2_row,matrix2_col);

    if(rank == 0){
        block_scatter_matrix(matrix1,matrix1_row,matrix1_col,block_size,A,100);
        block_scatter_matrix(matrix2,matrix2_row,matrix2_col,block_size,B,101);//send i^th block to i^th node
    }
    else{
        MPI_Recv(A,matrix1_row*matrix1_col,MPI_INT,0,100,MPI_COMM_WORLD,&status);
        MPI_Recv(B,matrix2_row*matrix2_col,MPI_INT,0,101,MPI_COMM_WORLD,&status);
    }

    MPI_Barrier(MPI_COMM_WORLD);//sync

    preprocessing_matrix(A,A_Buf,matrix1_row,matrix1_col,B,B_Buf,matrix2_row,matrix2_col,block_size,rank);

    MPI_Barrier(MPI_COMM_WORLD);//sync
    
    Cannon(A,A_Buf,matrix1_row,matrix1_col,B,B_Buf,matrix2_row,matrix2_col,C,block_size,rank); // do computing and shifting

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
        gather_matrix(result,matrix1_row1,matrix2_col,block_size);//gathering the result
        print_matrx(result,ROW1,COL2);
    }

    MPI_Finalize();
    return 0;
}
