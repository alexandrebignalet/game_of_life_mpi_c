#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "MiniCUnit.h"

#define ALIVE 1
#define DEAD 0

static SuiteDeTests suiteGameOfLife();

int sumCellNeighborhoodState(int* matrix, int size, int index) {
    int *ptr = &(matrix[index - size + 1]);
    int *val = &(matrix[index]);


    return matrix[index - size + 1] + matrix[index - size] + matrix[index - size - 1]
           + matrix[index - 1] + matrix[index + 1]
           + matrix[index + size - 1] + matrix[index + size] + matrix[index + size + 1];
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
    int size = 5400;
    int *matrix = (int*) malloc(size*size*sizeof(int));

    initialize(matrix, size, 2);

    assertEntiersEgaux(matrix[0], 0, "Test ghost cells up");
    assertEntiersEgaux(matrix[size+1], 0, "Test ghost cells left");
    assertEntiersEgaux(matrix[size*size], 0, "Test ghost cells bottom");
    assertEntiersEgaux(matrix[size-1], 0, "Test ghost cells right");

FIN_CAS_DE_TEST


CAS_DE_TEST( test_sum_alive_cell_neighbors )
    int size = 6;
    int* matrix = (int*) malloc(size*sizeof(int));

    assertVrai( sumCellNeighborhoodState(matrix, size, size+2) == 0 );

    int *ptr = matrix;
    for (int i = 0; i < 6; i++){
        for (int j = 0;  j < 6; j++){ *ptr++ = 1; }
    }


    assertVrai( sumCellNeighborhoodState(matrix, size, size+2) == 8 );

FIN_CAS_DE_TEST

SUITE_DE_TESTS( suiteGameOfLife )
       ajouterCasDeTest( test_generation_matrix );
       ajouterCasDeTest( test_sum_alive_cell_neighbors );
FIN_SUITE_DE_TESTS
