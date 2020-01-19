#!/bin/sh

module add mpi/intel/2018

echo "staring.."
date

mpicc -std=c99 $HOME/MPI/MPI_pro/matrix.c -o $HOME/MPI/MPI_pro/matrix -lm -w
mpirun $HOME/MPI/MPI_pro/matrix

date
