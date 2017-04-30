/** Mini cadre de tests unitaires pour le langage C.

    Cr�� � l'hiver 2006 dans le cadre d'un projet du Fonds de
    Developpement Pedagogique de l'UQAM.  Initialement inspir� du
    cadre de tests MinUnit d�crit � l'URL suivant:
    http://www.jera.com/techinfo/jtns/jtn002.html

    Par la suite, il a �t� am�lior� pour le rendre plus robuste,
    notamment en assurant que lorsqu'une assertion incorrecte est
    rencontr�e, l'ex�cution se poursuive quand m�me (ce qui n'etait
    pas le cas avec MinUnit). 

    De plus, la plupart des macros ont �t� remplac�es par des
    proc�dures ou des fonctions, sauf pour la d�claration des cas de
    tests et des suites de tests, o� on voulait pouvoir un acc�s
    explicite au nom du cas ou de la suite pour g�n�rer des messages
    plus significatifs.  

    Finalement, certaines statistiques additionnelles ont �t� ajout�es
    au sommaire, pour lequel une proc�dure a �t� aussi �t� d�finie.

    @name MiniCUnit
    @author Guy Tremblay
*/


//@{

#ifndef MINI_CUNIT_H
#define MINI_CUNIT_H

#include <string.h>

// Symbole qui, si a 1, permet de generer une trace d'execution (pour debogage).
#define TRACE_MINICUNIT 0

/// Type pour le r�sultat d'un test
typedef void CasDeTest;

/// Type pour le r�sultat d'une suite de tests
typedef void SuiteDeTests;

extern char _nomCasDeTest[];
extern char _nomSuiteDeTests[];

/// Macro pour d�buter la d�finition d'un nouveau cas de test.
#define CAS_DE_TEST( cas ) static CasDeTest cas() { strcpy(_nomCasDeTest, #cas); if (TRACE_MINICUNIT) printf( "+++ CAS_DE_TEST( %s )\n", #cas );

/// Macro pour terminer la d�finition d'un cas de test.
#define FIN_CAS_DE_TEST if (TRACE_MINICUNIT) printf( "--- CAS_DE_TEST( %s )\n", _nomCasDeTest ); }


/// Macro pour d�buter la d�finition d'une suite de tests.
#define SUITE_DE_TESTS( suite ) static SuiteDeTests suite() { strcpy(_nomSuiteDeTests, #suite);
/// Macro pour terminer la d�finition d'une suite de tests.
#define FIN_SUITE_DE_TESTS strcpy(_nomSuiteDeTests, ""); }


/** Ajoute un cas de test � la suite de tests en cours de d�finition
    (un appel � SUITE_DE_TESTS a �t� rencontr� et l'appel correspondant �
    FIN_SUITE_DE_TESTS n'a pas encore �t� rencontr�).

    @precondition Une suite de tests est en cours de d�finition
    @postcondition Le cas de test a �t� ajout� � la suite de tests
    @param test La proc�dure de test
*/
void ajouterCasDeTest( CasDeTest (*test)() );


/** Ex�cute une suite de tests.
  
    @postcondition Les diff�rents cas de test de la suite ont �t� ex�cut�s
    @param suite La suite de tests � ex�cuter
*/
void executerSuiteDeTests( SuiteDeTests (*suite_de_tests)() );


/** �value une assertion bool�enne pour v�rifier qu'elle est vraie.
  
    @param test L'expression bool�enne � v�rifier
    @param message Le message � afficher si l'assertion n'est pas vrai
    @postcondition Un message est affich� sur stdout seulement si test est faux
*/
void assertVrai( bool test, char* message );
void assertVrai( bool test );

/** �value une assertion bool�enne pour v�rifier qu'elle est fausse.
  
    @param test L'expression bool�enne � v�rifier
    @param message Le message � afficher si l'assertion n'est pas fausse
    @postcondition Un message est affich� sur stdout seulement si test est vrai
*/
void assertFaux( bool test, char* message );
void assertFaux( bool test );


/** �value une assertion qui v�rifie si deux cha�nes de caract�res
    sont �gales.
  
    @param c1 La premi�re cha�ne
    @param c2 La deuxi�me cha�ne
    @param message Le message � afficher si les deux cha�nes ne sont pas identiques
    @postcondition Un message est affich� sur stdout seulement si strcmp(c1, c2) != 0
*/
void assertChainesEgales( char* ch1, char* ch2, char* message );


/** �value une assertion qui v�rifie si deux entiers sont �gaux.
  
    @param v1 Le premier entier
    @param v2 Le deuxi�me entier
    @param message Le message � afficher si les deux entiers ne sont pas �gaux
    @postcondition Un message est affich� sur stdout seulement si v1 != v2
*/
void assertEntiersEgaux( int v1, int v2, char* message );


/** Retourne une cha�ne qui repr�sente le sommaire de l'ex�cution des
    suites et cas de tests. La cha�ne indique diff�rentes informations
    quant au nombre de suites de tests ex�cut�es, au nombre de cas de
    tests, d'assertions, etc.

*/
char* sommaireDeTests();

#endif

//@}
