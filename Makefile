################################################################################
#
# Fichier makefile pour tester et executer Game Of Life.
#
# Auteur: Alexandre BIGNALET.
#
################################################################################


N=1000

# Pour qu'une erreur d'execution ne termine pas le reste du makefile.
.IGNORE:

CC     = /usr/bin/g++
CFLAGS = -std=c++11 -Wno-write-strings -Wall
RM     = rm -f

# Les divers fichiers objets.
OBJECTS  = MiniCUnit.o game_of_life.o
TEST     = tester_game_of_life
MESURE   = mesurer-game-of-life

##############################################
# Cibles principales: compilation et tests
##############################################
default: compile tests

compile: $(TEST)

tests: tests_gol

tests_gol: $(TEST)
	./$(TEST)

#######################################
# Mesures
#######################################

$(MESURE).o: game_of_life.h $(MESURE).c

$(MESURE): $(MESURE).o $(OBJECTS)
	$(CC) -o $(MESURE) $(MESURE).o  $(OBJECTS) $(CFLAGS)

mesures: mesures-gol

mesures-gol: $(MESURE)
	./$(MESURE)

#######################################
# Dependances pour les divers fichiers.
#######################################

# Regle implicite pour compilation des fichiers .c
.c.o:
	$(CC) -c $< $(CFLAGS)

MiniCUnit.o: MiniCUnit.h

game_of_life.o: game_of_life.h

$(TEST).o: MiniCUnit.h game_of_life.h $(TEST).c

$(TEST): $(TEST).o $(OBJECTS)
	$(CC) -o $(TEST) $(TEST).o  $(OBJECTS) $(CFLAGS)

clean:
	$(RM) *.o $(TEST) $(MESURE)



