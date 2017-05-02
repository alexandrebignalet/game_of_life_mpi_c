#ifndef DEVOIR2_GAME_OF_LIFE_H
#define DEVOIR2_GAME_OF_LIFE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

#define GHOST_CELLS_SIZE 2

#define ALIVE 1
#define DEAD 0

#define CELL_GOES_ALIVE 3
#define CELL_KEEP_STATE 2

#define ROOT 0


int game_of_life_by_line(int size, int nb_steps, int repartition_probability);

int game_of_life_seq(int size, int nb_steps, int repartition_probability);

void initialize(int* matrix, int size, int repartition_probability);

void getLines(int* matrix, int size, int* buffer, int block_size, int num_proc);

void updateLines(int* lines, int line_size, int* line, int index);

void generation(int* line_block, int block_size, int size);

int sumCellNeighborhoodState(int* matrix, int size, int index);



void print(int *matrix, int width, int height);
void printLine(int *matrix, int size_x, int size_y);

#endif //DEVOIR2_GAME_OF_LIFE_H
