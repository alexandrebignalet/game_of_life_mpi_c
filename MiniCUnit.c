#include "MiniCUnit.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static int nb_suites = 0;
static bool suite_avortee = false;

static int nb_tests_echoues = 0;
static int nb_tests_executes = 0;

static int nb_asserts = 0;
static int nb_asserts_echoues = 0;
static int nb_erreurs = 0;

static bool debut_cas_de_test;

char _nomCasDeTest[1024];
char _nomSuiteDeTests[1024] = "";
char _msgTmp[1024];


static void ajouterErreurCasDeTest( char *msg )
{
  nb_erreurs += 1;
  if (debut_cas_de_test & !suite_avortee) {
    nb_tests_echoues += 1;
    debut_cas_de_test = false;
  }
  printf( " %d) \"%s\"::\"%s\"\n", nb_erreurs, _nomSuiteDeTests, _nomCasDeTest );
  if (!suite_avortee) {
    printf( "\t#%d: '%s'\n", nb_asserts, msg ); 
  } else {
    printf( "\t??: '%s'\n", msg ); 
  }
}

#define MAX_SOMMAIRE 256

char* sommaireDeTests()
{
  char* sommaire = (char*) malloc(MAX_SOMMAIRE);

  sprintf( sommaire, "*** Sommaire: %d suites executees; %d tests executes, %d tests echoues; %d assertions evaluees, %d assertions echouees\n",
           nb_suites, nb_tests_executes, nb_tests_echoues, nb_asserts, nb_asserts_echoues );
  return( sommaire );
}

static int nbChiffres( int v )
{
  int r = 1;
  while (v > 0) {
    r += 1;
    v /= 10;
  }
  return( r );
}

void assertEntiersEgaux( int v1, int v2, char* message )
{
  if (TRACE_MINICUNIT) {
    printf( "  assertEntierEgaux( %d, %d, '%s' )\n", v1, v2, message );
  }

  nb_asserts++;
  if ( v1 != v2 ) {
    nb_asserts_echoues++; 
    char* tmp = (char*) malloc( nbChiffres(v1) + nbChiffres(v2) + strlen(message) + 10 );
    sprintf( tmp, "%s (%d, %d)", message, v1, v2 );
    ajouterErreurCasDeTest(tmp);
  }
}

void assertChainesEgales( char* c1, char* c2, char* message )
{
  if (TRACE_MINICUNIT) {
    printf( "  assertChaines( '%s', '%s', '%s' )\n", c1, c2, message );
  }
  nb_asserts++;
  if ( strcmp((c1), (c2)) != 0 ) {
    nb_asserts_echoues++; 
    char* tmp = (char*) malloc( strlen(c1) + strlen(c2) + strlen(message) + 10 );
    sprintf( tmp, "%s (\"%s\", \"%s\")", message, c1, c2 );
    ajouterErreurCasDeTest(tmp);
  }
}

void assertVrai( bool test, char* message )
{
  if (TRACE_MINICUNIT) {
    printf( "  assertVrai( %d, '%s' )\n", test, message );
  }
  nb_asserts++;
  if (!(test)) {
    nb_asserts_echoues++; 
    if ( message != NULL ) {
      ajouterErreurCasDeTest(message);
    } else {
      ajouterErreurCasDeTest( _nomCasDeTest );
    }
  }
}

void assertVrai( bool test )
{
  sprintf( _msgTmp, "%s: assertVrai a echoue", _nomCasDeTest );
  assertVrai( test, _msgTmp );
}

void assertFaux( bool test, char* message )
{
  if (TRACE_MINICUNIT) {
    printf( "  assertFaux( %d, '%s' )\n", test, message );
  }
  nb_asserts++;
  if ((test)) {
    nb_asserts_echoues++; 
    ajouterErreurCasDeTest(message);
  }
}

void assertFaux( bool test )
{
  sprintf( _msgTmp, "%s: assertFaux a echoue", _nomCasDeTest );
  assertFaux( test, _msgTmp );
}

void monSignalHandler( int );

void setSignals()
{
  signal( SIGINT, monSignalHandler );  
  signal( SIGFPE, monSignalHandler );  
  signal( SIGBUS, monSignalHandler );  
  signal( SIGSEGV, monSignalHandler );  
}

void executerSuiteDeTests( SuiteDeTests (*suite_de_tests)() )
{
  setSignals();
  suite_de_tests();
  nb_suites++;
}

void ajouterCasDeTest( CasDeTest (*test)() ) 
{
  assert( strcmp(_nomSuiteDeTests, "") != 0 && "Appel a ajouterCasDeTest a l'exterieur de la definition d'une suite de tests" );

  debut_cas_de_test = true;
  test(); 
  nb_tests_executes++;
}


static char* signaux[] = {
  "INEXISTANT",
  "HUP",
  "INT",
  "QUIT",
  "ILL",
  "TRAP",
  "IOT",
  "EMT",
  "FPE",
  "KILL",
  "BUS",
  "SEGV",
  "SYS",
  "PIPE",
  "ALRM",
  "TERM",
  "USR1",
  "USR2",
  "CLD",
  "PWR"
};
  

#define MAX_MESSAGE_SIGNAL 256

void monSignalHandler( int numSig )
{
  char* tmp = (char*) malloc( MAX_MESSAGE_SIGNAL );
  if ( (numSig < 0) | (numSig > 19) ) {
    numSig = 0;
  }
  sprintf( tmp, "??? Erreur d'execution = signal SIG%s =>\n\t\t\tOn avorte l'execution du programme !!!", signaux[numSig] );

  suite_avortee = true;
  ajouterErreurCasDeTest( tmp );
  printf( "[Etat juste avant l'arret du programme] %s", sommaireDeTests() );
  exit(-1);
}
