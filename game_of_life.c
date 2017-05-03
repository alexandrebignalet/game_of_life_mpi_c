/***********************

Conway Game of Life

sequential version

************************/

#include "game_of_life.h"

int game_of_life_seq(int size, int nb_steps, int repartition_probability) {

    int width = size + GHOST_CELLS_SIZE;
    int *matrix = (int *) malloc(size * width * sizeof(int));

    initialize(matrix, size, width);

    //set first and last lines as ghosts cells
    int* first_line = &(matrix[0]);
    int* last_line = &(matrix[(size-1)*width]);
    for(int i = 0; i < width; i++) {
        *first_line++ = DEAD;
        *last_line++ = DEAD;
    }

    for(int step = 0; step < nb_steps; step++) {
        generation(matrix, size, width);
    }

    int nb_cells_alive = 0;
    for (int j = 0; j < size*width; j++) {
        nb_cells_alive += *matrix++;
    }

    return nb_cells_alive;
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
void generation(int* block, int block_size, int size) {

    int *ptr = &(block[size+1]);

    for( int x = 1; x < block_size-1; x++){

        for( int y = 1; y < size-1; y++){

            switch(sumCellNeighborhoodState(block, size, size*x+y)) {

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
int sumCellNeighborhoodState(int* matrix, int size, int index) {

    return matrix[index - size + 1] + matrix[index - size] + matrix[index - size - 1]
           + matrix[index - 1] + matrix[index + 1]
           + matrix[index + size - 1] + matrix[index + size] + matrix[index + size + 1];
}

// by line
void updateLines(int* lines, int line_size, int* line, int index) {
    int* ptr_lines = &(lines[index]);
    int* ptr_line = line;

    for(int i = 0 ; i < line_size; i++) {
        *ptr_lines++ = *ptr_line++;
    }
}
int game_of_life_by_line(int size, int nb_steps, int repartition_probability) {


    int my_id, nb_procs;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);

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

        for (int num_proc = 1; num_proc < nb_procs; num_proc++) {

            getLines(matrix, width, my_lines, nb_line_blocs, num_proc);
            MPI_Send(my_lines, width * line_block_size, MPI_INT, num_proc, 0, MPI_COMM_WORLD);
        }

        getLines(matrix, width, my_lines, nb_line_blocs, ROOT);
    } else {
        MPI_Recv(my_lines, width * line_block_size, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int* nextLine = (int *) malloc( width * sizeof(int));
    int* prevLine = (int *) malloc( width * sizeof(int));
    int nb_cells_alive;
    int my_nb_cells_alive = 0;

    for( int step = 0; step < nb_steps; step++) {

        generation(my_lines, line_block_size, width);

        if(my_id == ROOT) {
            MPI_Send(&(my_lines[(line_block_size-2)*width]), width, MPI_INT, next, 0, MPI_COMM_WORLD);
            MPI_Recv(nextLine, width, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        } else if (my_id == last) {
            MPI_Send(&(my_lines[size]), width, MPI_INT, prev, 0, MPI_COMM_WORLD);
            MPI_Recv(prevLine, width, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Send(&(my_lines[size]), width, MPI_INT, prev, 0, MPI_COMM_WORLD);
            MPI_Send(&(my_lines[(line_block_size-2)*width]), width, MPI_INT, next, 0, MPI_COMM_WORLD);

            MPI_Recv(prevLine, width, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(nextLine, width, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (my_id != ROOT) {
            updateLines(my_lines, width, prevLine, size);
        } else if (my_id != last) {
            updateLines(my_lines, width, nextLine, (line_block_size-1)*width);
        }
    }

    int* ptr = &(my_lines[size]);
    for ( int i = 0; i < width * nb_line_blocs; i++) {
        my_nb_cells_alive += *ptr++;
    }

    MPI_Reduce( &my_nb_cells_alive, &nb_cells_alive, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );

    return nb_cells_alive;
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

// utils
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
void print(int *matrix, int width, int height) {

    int* pt = matrix;

    for ( int i = 0; i < width; i++ ) {
        printf("\n");
        for (int j = 0; j < height; j++) {
            printf("%d ", *pt++);
        }
    }
}