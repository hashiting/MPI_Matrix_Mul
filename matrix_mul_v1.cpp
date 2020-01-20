#include "mpi.h"
#include<math.h>
#include<stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define ROW1 400
#define COL1 400
#define ROW2 400
#define COL2 400

//cache effiency
int *generate_space(int row,int col){
    int *matrix = (int*)calloc(row*col,sizeof(int));;
    return matrix;
}

int* generate_matrix(int row,int col,int range){
    int *matrix = generate_space(row,col);
    srand(time(NULL) + rand());
    for(int i=0;i < row;i++){
        for(int j=0;j < col;j++){
            matrix[i*col + j] = rand()%range;
        }
    }
    return matrix;
}

int get_value(int *matrix,int i,int j,int col){
    return matrix[i*col+j];
}

void set_value(int *matrix,int i,int j,int col,int value){
    matrix[i*col + j] = value;
}

void addc_value(int *matrix,int i,int j,int col,int value){
    matrix[i*col + j] += value;
}
void print_matrx(int *a,int row,int col)
{
    for(int i=0; i < row; i++)
    {
        for(int j=0; j < col; j++)
        {
            std::cout<<a[i*col + j]<<" ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}

bool validate(int* a,int* b, int row, int col){
    for(int i=0; i < row; i++)
    {
        for(int j=0; j < col; j++)
        {
            if(a[i*col + j] != b[i*col + j]){
                return false;
            }
        }
    }
    return true;
}


void matrix_multiple(int* A,int *B,int *C,int row1,int col1,int col2){
    int* tmp_C=(int*)malloc(row1*col2*sizeof(int));
    memset(tmp_C,0,sizeof(int)*row1*col2);    
    for(int i=0;i<row1;i++){
        for(int j=0;j<col2;j++){
            for(int k=0;k<col1;k++){
                addc_value(tmp_C,i,j,col2,get_value(A,i,k,col1)*get_value(B,k,j,col2));
            }
            addc_value(C,i,j,col2,get_value(tmp_C,i,j,col2));
        }      
    }
    free(tmp_C); 
}

int* get_block(int row_i,int col_i,int row,int col,int *matrix,int lencol){//index//size of block//len of big
    int *temp = generate_space(row,col);
    int i,t1,j,t2;
    for(i = row_i,t1=0;i < row_i + row;i++,t1++){
        for(j= col_i,t2=0;j < col + col_i;j++,t2++){
            //temp[t1*row + t2] = matrix[i*lenrow + j];
            set_value(temp,t1,t2,col,get_value(matrix,i,j,lencol));
        }
    }
    
    //print_matrx(temp,2,2);
    return temp;
}

void Set_block(int row_i,int col_i,int row,int col,int *result,int *small,int lencol){
    int i,t1,j,t2;
    for(i=row_i,t1=0;i < row_i + row;i++,t1++){
        for(j = col_i,t2=0;j < col + col_i;j++,t2++){
            //result[i][j] = small[t1][t2];
            set_value(result,i,j,lencol,get_value(small,t1,t2,col));
        }
    }
}

void block_scatter_matrix(int *matrix,int row,int col,int block_size,int *Bufer,int tag,int lencol){//size block
    for(int i=0;i <block_size; i++){
        for(int j=0;j < block_size;j++){
            if(i==0&&j==0){
                memcpy(Bufer,get_block(i*row,j*col,row,col,matrix,lencol),row*col*sizeof(int));
            } 
            else{
                MPI_Send(get_block(i*row,j*col,row,col,matrix,lencol),row*col,MPI_INT,i*block_size + j,tag,MPI_COMM_WORLD);
            }
        }
    }
}   

int get_left_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return row*block_size + (col - 1 + block_size)%block_size;
}

int get_right_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return row*block_size + (col + 1)%block_size;
}

int get_up_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return ((row-1 +block_size)%block_size)*block_size + col;
}

int get_down_index(int current_rank,int block_size){
    int row = current_rank/block_size;
    int col = current_rank%block_size;
    return ((row+1)%block_size)*block_size + col;
}

void cannon(int *left,int *left_Buf,int left_r,int left_c,int *right,int *right_Buf,int right_r,int right_c,int *result,int block_size,int rank){//each step recesend//computer
    MPI_Status status;
    memset(result,0,sizeof(result));
    for(int i=0;i < block_size;i++){
        matrix_multiple(left,right,result,left_r,left_c,right_c);
        if(i != block_size - 1){//do block_size-1 times
            MPI_Sendrecv(left,left_r*left_c,MPI_INT,get_left_index(rank,block_size),102,
                 left_Buf,left_r*left_c,MPI_INT,get_right_index(rank,block_size),102,MPI_COMM_WORLD,&status);

        
            MPI_Sendrecv(right,right_r*right_c,MPI_INT,get_up_index(rank,block_size),103,
                    right_Buf,right_r*right_c,MPI_INT,get_down_index(rank,block_size),103,MPI_COMM_WORLD,&status);

            memcpy(left,left_Buf,left_r*left_c*sizeof(int));
            memcpy(right,right_Buf,right_r*right_c*sizeof(int));
        }
    }
    /*
    if(rank != 0)
        MPI_Send(result,left_r*right_c,MPI_INT,0,104,MPI_COMM_WORLD);
    */
}

void gather_matrix(int *result,int row,int col,int block_size,int lencol){
    MPI_Status status;
    int *temp = generate_space(row,col);
    for(int i=0;i<block_size;i++){
        for(int j=0;j<block_size;j++){
            int temp_rank = i*block_size+j;
            if(temp_rank != 0){
                MPI_Recv(temp,row*col,MPI_INT,temp_rank,104,MPI_COMM_WORLD,&status);
                Set_block(i*row,j*col,row,col,result,temp,lencol);
            }
        }
    }
}

void transform_matrix(int *matrix,int block_size,int row,int col,int small_row,int small_col,bool left){
    int *temp = generate_space(row,col);
    memcpy(temp,matrix,row*col*sizeof(int));
    
    for(int i = 0;i < row;i++){
        for(int j = 0;j < col;j++){
            int row_i = (i/small_row)*block_size + j/small_col;//rank
            int col_j = (i%small_row)*small_col + j%small_col;
            if(left){
                int row = row_i/block_size;
                int col = row_i%block_size;
                row_i = row*block_size + (col - row + block_size)%block_size;
            }
            else{
                int row = row_i/block_size;
                int col = row_i%block_size;
                row_i = ((row-col +block_size)%block_size)*block_size + col;;
            }
            //std::cout<<row_i<<" "<<col_j<<"\n";
            matrix[row_i*small_row*small_col + col_j] = temp[i*col + j];
        }
    }
    
    free(temp);
}

void inverse_matrix(int *matrix,int num_process,int row,int col,int small_row,int small_col){

}

int main(int argc,char** argv){
    int size,rank,dest;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(comm,&size);
    MPI_Comm_rank(comm,&rank);
    if(rank == 0)
        printf("we have %d process\n",size);
    int block_size = sqrt(size);
    
    int *matrix1;
    int *matrix2;
    int *result;
    int *result1;
    double s,e;
    if(rank == 0){
        matrix1 = generate_matrix(ROW1,COL1,20);
        matrix2 = generate_matrix(ROW2,COL2,10);
        result = generate_space(ROW1,COL2);
        result1 = generate_space(ROW1,COL2);
        print_matrx(matrix1,ROW1,COL1);
        printf("print1 done\n");
        print_matrx(matrix2,ROW2,COL2);
        printf("print2 done\n");
        s = MPI_Wtime();
        matrix_multiple(matrix1,matrix2,result1,ROW1,COL1,COL2);//calculate time
        e = MPI_Wtime();
        std::cout<<"normal take time "<<e-s<<"\n";
        //print_matrx(result1,ROW1,COL2);
        //printf("print3 done\n");
    }
    
    int matrix1_row = ROW1/block_size;
    int matrix1_col = COL1/block_size;
    int matrix2_row = matrix1_col;
    int matrix2_col = COL2/block_size;

    int *A = generate_space(matrix1_row,matrix1_col);//
    int *B = generate_space(matrix2_row,matrix2_col);
    int *C = generate_space(matrix1_row,matrix2_col);
    int *A_Buf = generate_space(matrix1_row,matrix1_col);//buffer for communication
    int *B_Buf = generate_space(matrix2_row,matrix2_col);


    if(rank == 0){
        transform_matrix(matrix1,block_size,ROW1,COL1,matrix1_row,matrix1_col,true);
        transform_matrix(matrix2,block_size,ROW2,COL2,matrix2_row,matrix2_col,false);
        s = MPI_Wtime();
    }

    MPI_Scatter(matrix1,matrix1_row*matrix1_col, MPI_INT, A,
            matrix1_row*matrix1_col, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(matrix2,matrix2_row*matrix2_col, MPI_INT, B,
            matrix2_row*matrix2_col, MPI_INT, 0, MPI_COMM_WORLD);        

    printf("process: %d scatter done\n",rank);
    print_matrx(A,matrix1_row,matrix1_col);
    print_matrx(B,matrix2_row,matrix2_col);
    MPI_Barrier(MPI_COMM_WORLD);//sync
    //printf("process: %d preprocessing done\n",rank);
    cannon(A,A_Buf,matrix1_row,matrix1_col,B,B_Buf,matrix2_row,matrix2_col,C,block_size,rank); // do computing and shifting

    //printf("process: %d cannon done\n",rank);
    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(C, matrix1_row*matrix2_col, MPI_INT, result1, 1, MPI_INT, 0,
           MPI_COMM_WORLD);
    if(rank == 0){
        //printf("begin gathering\n");
        //Set_block(0,0,matrix1_row,matrix2_col,result,C,COL2);
        //gather_matrix(result,matrix1_row,matrix2_col,block_size,COL2);//gathering the result
        e = MPI_Wtime();
        std::cout<<"cannon time "<<e-s<< "\n";
        print_matrx(result,size,matrix1_row*matrix2_col);
        if(validate(result,result1,ROW1,COL2)){
            std::cout<<"successfully compute\n";
        }
    }
    
    MPI_Finalize();
    return 0;
}
