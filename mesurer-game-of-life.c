#include "game_of_life.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int my_id, nb_procs;

    // On initialise MPI.
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_id );
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs );

    srand(time(NULL));

//    int SIZE_WITH_GHOSTS_CELLS = GENERATION_SIZE + GHOST_CELLS_SIZE; // New array size including ghost cells size

    int nb_live_cells = game_of_life_par_static(GENERATION_SIZE, NB_STEPS, REPARTITION_PROBABILITY);
    if (my_id == 0) {
        printf("\nNumber of live cells = %d\n", nb_live_cells);
    }


    MPI_Finalize();
    return 0;
}
