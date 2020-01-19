#!/bin/sh

module add mpi/intel/2018

echo "staring.."
date

mpicxx $HOME/MPI/MPI_pro/matrix.cpp -o $HOME/MPI/MPI_pro/matrix -lm -w
mpirun $HOME/MPI/MPI_pro/matrix

date
