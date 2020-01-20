#!/bin/sh

module add mpi/intel/2018

echo "staring.."
date

mpicxx $HOME/MPI/MPI_pro/matrix_mul_v1.cpp -o $HOME/MPI/MPI_pro/matrix_mul_v1 -lm -w
mpirun $HOME/MPI/MPI_pro/matrix_mul_v1

date
