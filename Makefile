################################################################################
#
# Fichier makefile pour tester et executer Game Of Life.
#
# Auteur: Alexandre BIGNALET.
#
################################################################################


NP=4

# Pour qu'une erreur d'execution ne termine pas le reste du makefile.
.IGNORE:

MPICC  = mpicc
CC     = /usr/bin/g++
MPICFLAGS = -std=c99
CFLAGS = -std=c++11 -Wno-write-strings
RM     = rm -f
RUN    = mpirun

# Les divers fichiers objets.
TEST_OBJECTS = MiniCUnit.o
OBJECTS  = game_of_life.o
TEST     = tester_game_of_life
MESURE   = mesurer-game-of-life

##############################################
# Cibles principales: compilation et tests
##############################################
default: compile tests

tests: tests_gol

tests_gol: $(TEST)
	./$(TEST)

#######################################
# Mesures
#######################################

$(MESURE).o: game_of_life.h $(MESURE).c

$(MESURE): $(MESURE).o $(OBJECTS)
	$(MPICC) -o $(MESURE) $(MESURE).o  $(OBJECTS) $(MPICFLAGS)

mesures: clean $(MESURE)

run: $(MESURE)
	@echo "run: On utilise les processeurs suivants"
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt hostname
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt ./$(MESURE)

debug1: $(MESURE)
	@echo "debug1: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --hostfile config/un-host.txt hostname
	$(RUN) -np 2 --hostfile config/un-host.txt ./$(MESURE)

debug2: $(MESURE)
	@echo "debug2: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt hostname
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt ./$(MESURE)

#######################################
# Dependances pour les divers fichiers.
#######################################

# Regle implicite pour compilation des fichiers .c
.c.o:
	$(MPICC) -c $< $(MPICFLAGS)

MiniCUnit.o: MiniCUnit.h

game_of_life.o: game_of_life.h game_of_life.c

$(TEST).o: MiniCUnit.h game_of_life.h $(TEST).c

$(TEST):
	$(CC) -c MiniCUnit.h MiniCUnit.c $(CFLAGS)
	$(CC) -c game_of_life.h game_of_life.c tester_game_of_life.c $(CFLAGS)
	$(CC) -o $(TEST) $(TEST).o  $(OBJECTS) $(TEST_OBJECTS) $(CFLAGS)

clean:
	$(RM) *.o $(TEST) $(MESURE)



