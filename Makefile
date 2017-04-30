################################################################################
#
# Fichier makefile pour tester et executer Game Of Life.
#
# Auteur: Alexandre BIGNALET.
#
################################################################################

# Nombre de processeurs
NP=4

# Pour qu'une erreur d'execution ne termine pas le reste du makefile.
.IGNORE:

MPICC = mpicc
RUN = mpirun
CFLAGS = -std=c99
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

default: run

compile: $(MESURE)

run: $(MESURE)
	@echo "run: On utilise les processeurs suivants"
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt hostname
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt ./$(MESURE)

debug1: $(EXEC)
	@echo "debug1: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --hostfile config/un-host.txt hostname
	$(RUN) -np 2 --hostfile config/un-host.txt ./$(MESURE)

debug2: $(EXEC)
	@echo "debug2: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt hostname
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt ./$(MESURE)


#######################################
# Dependances pour les divers fichiers.
#######################################

# Regle implicite pour compilation des fichiers .c
.c.o:
	$(MPICC) -c $< $(CFLAGS)

MiniCUnit.o: MiniCUnit.h

game_of_life.o: game_of_life.h

$(TEST).o: MiniCUnit.h game_of_life.h $(TEST).c

$(TEST): $(TEST).o $(OBJECTS)
	$(CC) -o $(TEST) $(TEST).o  $(OBJECTS) $(CFLAGS)

clean:
	$(RM) *.o $(TEST) $(MESURE)



