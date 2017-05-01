/***********************

Conway Game of Life

sequential version

************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "game_of_life.h"

int CELL_NEIGHBORS_SIZE = 3;

int game_of_life_par_static(int size, int nb_steps, int repartition_probability) {


    int my_id, nb_procs;

    int nb_live_cells = 0;
    int my_sum = 0;
    MPI_Comm_rank( MPI_COMM_WORLD, &my_id );
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs );

    assert(  size % nb_procs == 0        && "Mauvaise valeur pour GENERATION_SIZE: nb_procs ne divise pas GENERATION_SIZE" );

    int line_block_size = size / nb_procs;

    int *matrix = (int*) malloc(size * size * sizeof(int));
    int* my_lines = (int*) malloc (size*(line_block_size + GHOST_CELLS_SIZE)* sizeof(int));;

    if (my_id == ROOT) {
        initialize(matrix, size, repartition_probability);
        printf("\n");
        print(matrix, size);
        printf("\n");
    }
//    for( int step = 0; step < NB_STEPS; step++) {

        if (my_id == ROOT) {
            int begin, end;

            for (int num_proc = 1 ; num_proc < nb_procs; num_proc++) {

                getLines(matrix, size, my_lines, line_block_size, num_proc);
                printf("\n");
                printLine(my_lines, (line_block_size + GHOST_CELLS_SIZE), size);
                printf("\n");
                // TODO async
                MPI_Send(my_lines, size*(line_block_size + GHOST_CELLS_SIZE), MPI_INT, num_proc, 0, MPI_COMM_WORLD);
            }
            getLines(matrix, size, my_lines, line_block_size, ROOT);
            printf("\n");
            printLine(my_lines, (line_block_size + GHOST_CELLS_SIZE), size);
            printf("\n");
        } else {
            MPI_Recv(my_lines, size*(line_block_size + GHOST_CELLS_SIZE), MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

    return nb_live_cells;
}

void initialize(int* matrix, int size, int repartition_probability) {
    int* pt = matrix;
    int rand_number;
    float predicate;

    for ( int i = 0; i < size; i++ ) {
        for ( int j = 0; j < size; j++ ) {

            if (i == 0 || i == size-1) {
                *pt++ = DEAD;
            } else if (j == 0 || j == size-1) {
                *pt++ = DEAD;
            } else {
                rand_number = rand() % repartition_probability;
                predicate = (float) 1 / repartition_probability;

                if( rand_number < predicate )
                {
                    *pt++ = ALIVE;
                } else {
                    *pt++ = DEAD;
                }
            }
        }
    }
}

int sumCellNeighborhoodState(GenerationMatrix matrix, int x, int y) {
    int x_lower = x-1;
    int x_upper = x+1;
    int y_lower = y-1;
    int y_upper = y+1;

    return matrix[x_lower][y_upper] + matrix[x][y_upper] + matrix[x_upper][y_upper]
           + matrix[x_lower][y] + matrix[x_upper][y] + matrix[x_lower][y_lower]
           + matrix[x][y_lower] + matrix[x_upper][y_lower];
}

void printLine(int *matrix, int size_x, int size_y) {
    int* ptr = matrix;

    for (int i = 0; i < size_x; i++) {
        printf("\n");
        for (int j = 0; j < size_y; j++) {
            printf("%d ", *ptr++);
        }
    }
}

void printRange(int* matrix, int begin, int size_x, int size_y) {
    int* ptr = &(matrix[begin]);

    for (int i = 0; i < size_x; i++) {
        printf("\n");
        for (int j = 0; j < size_y; j++) {
            printf("%d ", *ptr++);
        }
    }
}

void print(int *matrix, int size) {

    int* pt = matrix;

    for ( int i = 0; i < size; i++ ) {
        printf("\n");
        for (int j = 0; j < size; j++) {
            printf("%d ", *pt++);
        }
    }
}

void getLines(int* matrix, int size, int* buffer, int block_size, int num_proc) {

    int begin = num_proc * block_size * size;
    int end = begin + block_size;

    int *line_ptr = &(buffer[0]);

    if (num_proc == 0) {
        // set dead ghost cells
        for (int j = 0; j < size*2; j++) {
            *line_ptr++ = DEAD;
        }
    } else {
        int ghost_cells_index = begin - 2 * size;
        int *pt = &(matrix[ghost_cells_index]);
        for (int j = ghost_cells_index; j < begin ; j++) {
            *line_ptr++ = *pt++;
        }
    }

    int *ptr = &(matrix[begin]);

    for(int i = begin; i < end; i++) {
        for (int j = 0; j < size; j++) {
            *line_ptr++ = *ptr++;
        }
    }
}

void printLinesBlock(GenerationMatrix matrix, int size_x, int size_y) {

    for (int i = 0 ; i < size_x ; i++) {
        printf("\n");
        for (int j = 0 ; j < size_y ; j++) {
            printf("%d ", matrix[i][j]);
        }
    }
}

void generation(int size, GenerationMatrix line_block, int block_size) {

    GenerationMatrix tmp = (GenerationMatrix) malloc(block_size * sizeof(int*));
    for(int i = 0; i < block_size; i++) {
        tmp[i] = (int*) malloc(size* sizeof(int));
    }

    for( int x = 1; x < block_size-1; x++){
        for( int y = 1; y < size-1; y++){

//            switch(sumCellNeighborhoodState(line_block, x, y)){
//
//                case CELL_GOES_ALIVE:
//                    tmp[x][y] = ALIVE;
//                    break;
//
//                case CELL_KEEP_STATE:
//                    tmp[x][y] = line_block[x][y];
//                    break;
//
//                default:
//                    tmp[x][y] = DEAD;
//            }
        }
    }

    printf("\n generate");
    printLinesBlock(line_block, block_size, size);
    printf("\n");
    line_block = tmp;
}



/*  initialize elements of last_gen to 0 or 1 without ghosts cells */
GenerationMatrix initGenerationMatrix(int size, int repartition_probability) {
    float rand_between_zero_and_one;
    int x, y;

    GenerationMatrix matrix = (int**) malloc(size * sizeof(int*));

    for( int x = 0; x < size; x++) {
        matrix[x] = (int*) malloc(size * sizeof(int));
    }



    // Init the boundary (ghost cells) as DEAD
    for(x = 0; x < size; x++) {
        matrix[x][0] = DEAD;
        matrix[x][size-1] = DEAD;
    }

    for (y = 0; y < size; y++) {
        matrix[0][y] = DEAD;
        matrix[size-1][y] = DEAD;
    }

    // Init the real generation with a repartition as 0.5 DEAD and 0.5 ALIVE
    for(x = 1; x < size-1; x++)
    {
        for( y = 1; y < size-1; y++)
        {
            rand_between_zero_and_one = rand() % repartition_probability;

            if( rand_between_zero_and_one < 0.5)
            {
                matrix[x][y] = ALIVE;
            } else {
                matrix[x][y] = DEAD;
            }
        }
    }

    return matrix;
}

void printGeneration(GenerationMatrix matrix, int size) {

    for (int i = 0 ; i < size ; i++) {
        printf("\n");
        for (int j = 0 ; j < size ; j++) {
            printf("%d ", matrix[i][j]);
        }
    }
}

int getCellState(CellNeighbors cell_neighbors) {

    int sum_state_neighbors;

    for ( int i = 0 ; i < CELL_NEIGHBORS_SIZE ; i++) {
        for ( int j = 0 ; j < CELL_NEIGHBORS_SIZE ; j++) {
            sum_state_neighbors += cell_neighbors[i][j];
        }
    }
    // the current state of the cell is not considered in the process
    return sum_state_neighbors;
}