/***********************

Conway Game of Life

sequential version

************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "game_of_life.h"

int CELL_NEIGHBORS_SIZE = 3;

GenerationMatrix getLines(GenerationMatrix matrix, GenerationMatrix lines_block, int block_size, int num_proc) {

    int start_line_chunk = block_size * num_proc;
    int k = 0;


    if (num_proc == 0) {
        // set ghost cells
        lines_block[0] = matrix[0];
        lines_block[1] = matrix[0];
    } else {
        lines_block[0] = matrix[start_line_chunk-2];
        lines_block[1] = matrix[start_line_chunk-1];
    }

    k = 2;
    int relative_size = start_line_chunk+block_size;
    for (int i = start_line_chunk; i < relative_size ; i++) {
            lines_block[k] = matrix[i];
            k++;
    }
}

int game_of_life_par_static(int size, int nb_steps, int repartition_probability) {

    int my_id, nb_procs;

    int nb_live_cells = 0;
    int my_sum = 0;
    MPI_Comm_rank( MPI_COMM_WORLD, &my_id );
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs );

    int my_size = size / nb_procs;
    GenerationMatrix matrix;

    GenerationMatrix my_matrix = (GenerationMatrix) malloc( (my_size+GHOST_CELLS_SIZE) * sizeof(int*));
    for( int i = 0; i < my_size; i++) {
        my_matrix[i] = (int*) malloc(size * sizeof(int));
    }

    if (my_id == ROOT) {

        matrix = initGenerationMatrix(size, repartition_probability);

        getLines(matrix, my_matrix, my_size, ROOT);

        for (int num_proc = 1 ; num_proc < nb_procs; num_proc++) {
            getLines(matrix, my_matrix, my_size, num_proc);
            MPI_Send(*my_matrix, (my_size+GHOST_CELLS_SIZE), MPI_INT, num_proc, 0, MPI_COMM_WORLD);
        }
    } else {

        MPI_Recv(*my_matrix, (my_size+GHOST_CELLS_SIZE), MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("id: %d, sum: %d", my_id, nb_live_cells);




    return nb_live_cells;
}

/*  initialize elements of last_gen to 0 or 1 without ghosts cells */
GenerationMatrix initGenerationMatrix(int size, int repartition_probability)
{
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

int sumCellNeighborhoodState(GenerationMatrix matrix, int x, int y) {
    int x_lower = x-1;
    int x_upper = x+1;
    int y_lower = y-1;
    int y_upper = y+1;

    return matrix[x_lower][y_upper] + matrix[x][y_upper] + matrix[x_upper][y_upper]
           + matrix[x_lower][y] + matrix[x_upper][y] + matrix[x_lower][y_lower]
           + matrix[x][y_lower] + matrix[x_upper][y_lower];
}

void printGeneration(GenerationMatrix matrix, int size) {

    for (int i = 0 ; i < size ; i++) {
        printf("\n");
        for (int j = 0 ; j < size ; j++) {
            printf("%d ", matrix[i][j]);
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
void generation(GenerationMatrix base_matrix, int size, int steps) {

    GenerationMatrix tmp_matrix = (int**) malloc(size * sizeof(int*));

    for( int x = 0; x < size; x++) {
        tmp_matrix[x] = (int*) malloc(size * sizeof(int));
    }

    for( int step = 0; step < steps; step++){

        for( int x = 1; x < size-1; x++){

            for( int y = 1; y < size-1; y++){

                switch(sumCellNeighborhoodState(base_matrix, x, y)){

                    case CELL_GOES_ALIVE:
                        tmp_matrix[x][y] = ALIVE;
                        break;

                    case CELL_KEEP_STATE:
                        tmp_matrix[x][y] = base_matrix[x][y];
                        break;

                    default:
                        tmp_matrix[x][y] = DEAD;
                }
            }
        }

        base_matrix = tmp_matrix;
    }
}
