#include "game_of_life.h"
#include <time.h>

int main(int argc, char *argv[])
{
    assert( argc > 5);

    int matrix_size = atoi(argv[1]);
    int steps = atoi(argv[2]);
    int repartition_probability = atoi(argv[3]);
    int warm_up = atoi(argv[4]);
    int only_results = atoi(argv[5]);

    int my_id, nb_procs;

    // On initialise MPI.
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_id );
    MPI_Comm_size( MPI_COMM_WORLD, &nb_procs );

    assert(matrix_size % nb_procs == 0 && "Matrix size should be divisible by nb proc used.");

    srand(time(NULL));

    double temps_ecoule_seq_version;
    MPI_Barrier( MPI_COMM_WORLD );
    temps_ecoule_seq_version = -MPI_Wtime();

    game_of_life_seq(matrix_size, steps, repartition_probability);

    double temps_sum_seq_version;
    temps_ecoule_seq_version += MPI_Wtime();
    MPI_Reduce( &temps_ecoule_seq_version, &temps_sum_seq_version, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );


    int nb_live_cells;
    double temps_ecoule_line_version;
    double temps_max_line_version;

    for (int i = 0; i <= warm_up; i++) {
        // On demarre la minuterie.
        MPI_Barrier( MPI_COMM_WORLD );
        temps_ecoule_line_version = -MPI_Wtime();

        nb_live_cells = game_of_life_by_line(matrix_size, steps, repartition_probability);

        // On mesure le temps requis pour effectuer les generations.
        temps_ecoule_line_version += MPI_Wtime();
        MPI_Reduce( &temps_ecoule_line_version, &temps_max_line_version, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    }

    if (my_id == 0) {
        double seq_avg = temps_sum_seq_version / nb_procs;

        if ( only_results == 0) {
            printf( "# SIZE = %d\n", matrix_size );
            printf( "# STEPS = %d\n", steps );
            printf( "# REPARTITION_PROBABILITY = %d\n", repartition_probability );
            printf( "# NB_CELLS_ALIVE = %d\n", nb_live_cells );
            printf( "# WARMUP_NB = %d\n", warm_up );
            printf( "# nb_procs     seq        par_line    acc_line\n" );
        }

        // Impression des resultats.
        printf( "%7d ", nb_procs );
        printf( "%12.2f ", 1000.0*seq_avg );
        printf( "%10.2f ", 1000.0*temps_max_line_version );
        printf( "%10.2f ", seq_avg/temps_max_line_version );
        printf( "\n" );
    }

    MPI_Finalize();


    return 0;
}
