#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "MiniCUnit.h"
#include "game_of_life.h"

static SuiteDeTests suiteGameOfLife();


int main( int argc, char* argv[] )
{
  executerSuiteDeTests( suiteGameOfLife );

  printf( "\n%s", sommaireDeTests() );

  return 0;
}


//
// Cas de tests pour valider la génération de la matrice de jeu et des ghosts cells.
//
CAS_DE_TEST( test_generation_matrix )
    int size = 5435;
    GenerationMatrix matrix;

    matrix = initGenerationMatrix(size, REPARTITION_PROBABILITY);

    for (int i = 0 ; i < size ; i++) {
        assertEntiersEgaux(matrix[0][i], 0, "Test ghost cells up");
        assertEntiersEgaux(matrix[i][0], 0, "Test ghost cells left");
        assertEntiersEgaux(matrix[i][size-1], 0, "Test ghost cells bottom");
        assertEntiersEgaux(matrix[size-1][i], 0, "Test ghost cells right");
    }
FIN_CAS_DE_TEST


CAS_DE_TEST( test_sum_alive_cell_neighbors )

    int** matrix = (int**) malloc(6*sizeof(int*));
    for (int i = 0; i < 6; i++){
        matrix[i] = (int*) malloc(6*sizeof(int));
        for (int j = 0;  j < 6; j++){ matrix[i][j] = 0; }
    }

    assertVrai( sumCellNeighborhoodState(matrix, 2, 2) == 0 );


    for (int i = 0; i < 6; i++){
        matrix[i] = (int*) malloc(6*sizeof(int));
        for (int j = 0;  j < 6; j++){ matrix[i][j] = 1; }
    }


    assertVrai( sumCellNeighborhoodState(matrix, 2, 2) == 8 );

FIN_CAS_DE_TEST

SUITE_DE_TESTS( suiteGameOfLife )
       ajouterCasDeTest( test_generation_matrix );
       ajouterCasDeTest( test_sum_alive_cell_neighbors );
FIN_SUITE_DE_TESTS
