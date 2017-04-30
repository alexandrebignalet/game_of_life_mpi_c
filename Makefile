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

CC     = mpicc
CFLAGS = -std=c99
RM     = rm -f

# Les divers fichiers objets.
TEST_OBJECTS = MiniCUnit.o
OBJECTS  = game_of_life.o
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

run: $(MESURE)
    @echo "run: On utilise les processeurs suivants"
    $(RUN) -np $(NP) --map-by node --hostfile plusieurs-hosts.txt hostname
    $(RUN) -np $(NP) --map-by node --hostfile plusieurs-hosts.txt ./$(MESURE)

debug1: $(MESURE)
    @echo "debug1: On utilise les deux processeurs suivants"
    $(RUN) -np 2 --hostfile un-host.txt hostname
    $(RUN) -np 2 --hostfile un-host.txt ./$(MESURE)

debug2: $(MESURE)
    @echo "debug2: On utilise les deux processeurs suivants"
    $(RUN) -np 2 --map-by node --hostfile deux-host.txt hostname
    $(RUN) -np 2 --map-by node --hostfile deux-host.txt ./$(MESURE)

#######################################
# Dependances pour les divers fichiers.
#######################################

# Regle implicite pour compilation des fichiers .c
.c.o:
	$(CC) -c $< $(CFLAGS)

MiniCUnit.o: MiniCUnit.h

game_of_life.o: game_of_life.h

$(TEST).o: MiniCUnit.h game_of_life.h $(TEST).c

$(TEST): $(TEST).o $(OBJECTS) $(TEST_OBJECTS)
	$(CC) -o $(TEST) $(TEST).o  $(OBJECTS) $(TEST_OBJECTS) $(CFLAGS)

clean:
	$(RM) *.o $(TEST) $(MESURE)



