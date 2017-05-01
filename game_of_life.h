#ifndef DEVOIR2_GAME_OF_LIFE_H
#define DEVOIR2_GAME_OF_LIFE_H

#define GENERATION_SIZE 18        /* generation array size */
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

int sumCellNeighborhoodState(GenerationMatrix matrix, int x, int y);

void generation(GenerationMatrix base_matrix, int size, int steps);

GenerationMatrix initGenerationMatrix(int size, int repartition_probability);

void printGeneration(GenerationMatrix matrix, int size);
void printLinesBlock(GenerationMatrix matrix, int size_x, int size_y);

#endif //DEVOIR2_GAME_OF_LIFE_H
