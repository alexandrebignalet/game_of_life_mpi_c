#ifndef DEVOIR2_GAME_OF_LIFE_H
#define DEVOIR2_GAME_OF_LIFE_H

#define GENERATION_SIZE 1000000        /* generation array size */
#define GHOST_CELLS_SIZE 2

#define ALIVE 1
#define DEAD 0

#define CELL_GOES_ALIVE 3
#define CELL_KEEP_STATE 2

#define REPARTITION_PROBABILITY 2

#define NB_STEPS 201    /* number of time steps */

#define ROOT 0

typedef int** GenerationMatrix;

typedef int** CellNeighbors;

int game_of_life_par_static(int size, int nb_steps, int repartition_probability);

int getCellState(CellNeighbors cell_neighbors);

void updateLines(int* lines, int line_size, int* line, int index);
int sumCellNeighborhoodState(int* matrix, int size, int index);

void generation(int* line_block, int block_size, int size);

GenerationMatrix initGenerationMatrix(int size, int repartition_probability);
void initialize(int* matrix, int size, int repartition_probability);
void printGeneration(GenerationMatrix matrix, int size);
void print(int* matrix, int size);
void printLine(int *matrix, int size_x, int size_y);
void getLines(int* matrix, int size, int* buffer, int block_size, int num_proc);
void printLinesBlock(GenerationMatrix matrix, int size_x, int size_y);

#endif //DEVOIR2_GAME_OF_LIFE_H
