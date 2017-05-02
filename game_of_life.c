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

    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);

    assert(size % nb_procs == 0 && "Mauvaise valeur pour GENERATION_SIZE: nb_procs ne divise pas GENERATION_SIZE");

    int nb_line_blocs = size / nb_procs;
    int line_block_size = nb_line_blocs + GHOST_CELLS_SIZE;
    int height = size;
    int width = size + GHOST_CELLS_SIZE;

    int *matrix = (int *) malloc(height * width * sizeof(int));
    int *my_lines = (int *) malloc( line_block_size * width * sizeof(int));

    int next = my_id + 1;
    int prev = my_id - 1;
    int last = nb_procs - 1;

    if (my_id == ROOT) {

        initialize(matrix, height, repartition_probability);
//        print(matrix, height);
        int begin, end;

        for (int num_proc = 1; num_proc < nb_procs; num_proc++) {

            getLines(matrix, width, my_lines, nb_line_blocs, num_proc);

            MPI_Send(my_lines, width * line_block_size, MPI_INT, num_proc, 0, MPI_COMM_WORLD);
        }

        getLines(matrix, width, my_lines, nb_line_blocs, ROOT);
//        printf("\n create");
//        printLine(my_lines, line_block_size, width);
    } else {
        MPI_Recv(my_lines, width * line_block_size, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//        printf("\n");
//        printLine(my_lines, line_block_size, width);
    }

    int* nextLine = (int *) malloc( width * sizeof(int));
    int* prevLine = (int *) malloc( width * sizeof(int));
    int nb_cells_alive;
    int my_nb_cells_alive = 0;

    for( int step = 0; step < NB_STEPS; step++) {

        generation(my_lines, line_block_size, width);

        if(my_id == ROOT) {
            MPI_Send(&(my_lines[(line_block_size-2)*width]), width, MPI_INT, next, 0, MPI_COMM_WORLD);
            MPI_Recv(nextLine, width, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //update my_lines
            updateLines(my_lines, width, nextLine, (line_block_size-1)*width);
//            printf("\n updated");
//            printf("\n line receive: ");
//            printLine(nextLine, 1, width);printf("\n");
//            printLine(my_lines, line_block_size, width);

        } else if (my_id == last) {
            MPI_Send(&(my_lines[size]), width, MPI_INT, prev, 0, MPI_COMM_WORLD);
            MPI_Recv(prevLine, width, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //update my_lines
            updateLines(my_lines, width, prevLine, size);
//            printf("\n");
//            printLine(my_lines, line_block_size, width);
        } else {
            MPI_Send(&(my_lines[size]), width, MPI_INT, prev, 0, MPI_COMM_WORLD);
            MPI_Send(&(my_lines[(line_block_size-2)*width]), width, MPI_INT, next, 0, MPI_COMM_WORLD);

            MPI_Recv(nextLine, width, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(prevLine, width, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //update mylines
            updateLines(my_lines, width, nextLine, (line_block_size-1)*width);
            updateLines(my_lines, width, prevLine, size);
        }
    }

    int* ptr = &(my_lines[size]);
    for ( int i = 0; i < width * nb_line_blocs; i++) {
        my_nb_cells_alive += *ptr++;
    }

    MPI_Reduce( &my_nb_cells_alive, &nb_cells_alive, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );

    return nb_cells_alive;
}

void updateLines(int* lines, int line_size, int* line, int index) {
    int* ptr_lines = &(lines[index]);
    int* ptr_line = line;

    for(int i = 0 ; i < line_size; i++) {
        *ptr_lines = *ptr_line;
//        printf("%d == %d ? \n", *ptr_lines++, *ptr_line++);
        *ptr_lines++; *ptr_line++;
    }
}

void initialize(int* matrix, int size, int repartition_probability) {
    int* pt = matrix;
    int rand_number;
    float predicate;

    for ( int i = 0; i < size; i++ ) {
        *pt++ = DEAD;

        for ( int j = 0; j < size; j++ ) {
            rand_number = rand() % repartition_probability;
            predicate = (float) 1 / repartition_probability;

            if( rand_number < predicate )
            {
                *pt++ = ALIVE;
            } else {
                *pt++ = DEAD;
            }
        }

        *pt++ = DEAD;
    }
}

int sumCellNeighborhoodState(int* matrix, int size, int index) {

    return matrix[index - size + 1] + matrix[index - size] + matrix[index - size - 1]
           + matrix[index - 1] + matrix[index + 1]
           + matrix[index + size - 1] + matrix[index + size] + matrix[index + size + 1];



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
        for (int j = 0; j < (size + GHOST_CELLS_SIZE); j++) {
            printf("%d ", *pt++);
        }
    }
}

void getLines(int* matrix, int size, int* buffer, int block_size, int num_proc) {

    int nb_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);

    int begin = num_proc * block_size * size;
    int end = begin + block_size;

    int *line_ptr = &(buffer[0]);


    if (num_proc != 0) {
        // set dead ghost cells
        // retrieve last block line
        int ghost_cells_index = begin - size;
        int *pt = &(matrix[ghost_cells_index]);
        for (int j = 0; j < size ; j++) {
            *line_ptr++ = *pt++;
        }
    } else {
        for (int j = 0; j < size ; j++) {
            *line_ptr++ = DEAD;
        }
    }

    int *ptr = &(matrix[begin]);

    for(int i = begin; i < end; i++) {
        for (int j = 0; j < size; j++) {
            *line_ptr++ = *ptr++;
        }
    }

    if (num_proc != nb_procs-1) {
        for(int j = 0; j < size; j++) {
            *line_ptr++ = *ptr++;
        }
    } else {
        for(int j = 0; j < size; j++) {
            *line_ptr++ = DEAD;
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

void generation(int* line_block, int block_size, int size) {

    int *ptr = &(line_block[size+1]);

    for( int x = 1; x < block_size-1; x++){

        for( int y = 1; y < size-1; y++){

            switch(sumCellNeighborhoodState(line_block, size, size*x+y)) {

                case CELL_GOES_ALIVE:
                    *ptr++ = ALIVE;
                    break;

                case CELL_KEEP_STATE:
                    *ptr++;
                    break;

                default:
                    *ptr++ = DEAD;
                    break;
            }
        }
        *ptr++;
        *ptr++;
    }
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

    // Init the real generation with a repartition of 0.5 DEAD and 0.5 ALIVE
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