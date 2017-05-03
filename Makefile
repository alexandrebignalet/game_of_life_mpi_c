################################################################################
#
# Fichier makefile pour tester et executer Game Of Life.
#
# Auteur: Alexandre BIGNALET.
#
################################################################################


NP=10
MATRIX_SIZE = 300
STEPS = 200
REPARTITION_PROBABILITY = 2

# Pour qu'une erreur d'execution ne termine pas le reste du makefile.
.IGNORE:

MPICC  = mpicc
CC     = /usr/bin/g++
MPICFLAGS = -std=c99 -lm
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

default: clean $(MESURE)

tests: test

mesures: $(MESURE)
	$(RUN) -np 2   ./$(MESURE)  128 100 2 5 0 >  ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 4   ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 8   ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 16  ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 32  ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 64  ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt
	$(RUN) -np 128 ./$(MESURE)  128 100 2 5 1 >> ./benchmarks/data-files/temps-128-100-2.txt

	$(RUN) -np 2   ./$(MESURE)  512 100 2 5 0 >  ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 4   ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 8   ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 16  ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 32  ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 64  ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt
	$(RUN) -np 128 ./$(MESURE)  512 100 2 5 1 >> ./benchmarks/data-files/temps-500-100-2.txt

	$(RUN) -np 2   ./$(MESURE)  1024 100 2 5 0 >  ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 4   ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 8   ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 16  ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 32  ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 64  ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt
	$(RUN) -np 128 ./$(MESURE)  1024 100 2 5 1 >> ./benchmarks/data-files/temps-1024-100-2.txt

graphes_acc:
	./benchmarks/scripts/plot-acc.sh 128 100 2
	./benchmarks/scripts/plot-acc.sh 512 100 2
	./benchmarks/scripts/plot-acc.sh 1024 100 2

graphes_temps:
	./benchmarks/scripts/plot-temps.sh 128 100 2
	./benchmarks/scripts/plot-temps.sh 500 100 2
	./benchmarks/scripts/plot-temps.sh 1000 100 2

#######################################
# Mesures
#######################################

$(MESURE).o: game_of_life.h $(MESURE).c

$(MESURE): $(MESURE).o $(OBJECTS)
	$(MPICC) -o $(MESURE) $(MESURE).o  $(OBJECTS) $(MPICFLAGS)


run: $(MESURE)
	@echo "run: On utilise les processeurs suivants"
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt hostname
	$(RUN) -np $(NP) --map-by node --hostfile config/plusieurs-hosts.txt ./$(MESURE) $(MATRIX_SIZE) $(STEPS) $(REPARTITION_PROBABILITY)

debug: $(MESURE)
	@echo "debug: On utilise les deux processeurs suivants"
	$(RUN) -np 3 ./$(MESURE) 300 100 2 5 0

debug1: $(MESURE)
	@echo "debug1: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --hostfile config/un-host.txt hostname
	$(RUN) -np 2 --hostfile config/un-host.txt ./$(MESURE) $(MATRIX_SIZE) $(STEPS) $(REPARTITION_PROBABILITY)

debug2: $(MESURE)
	@echo "debug2: On utilise les deux processeurs suivants"
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt hostname
	$(RUN) -np 2 --map-by node --hostfile config/deux-host.txt ./$(MESURE) $(MATRIX_SIZE) $(STEPS) $(REPARTITION_PROBABILITY)

#######################################
# Dependances pour les divers fichiers.
#######################################

# Regle implicite pour compilation des fichiers .c
.c.o:
	$(MPICC) -c $< $(MPICFLAGS)

MiniCUnit.o: MiniCUnit.h

game_of_life.o: game_of_life.h game_of_life.c

$(TEST).o: MiniCUnit.h game_of_life.h $(TEST).c

test:
	$(CC) -c MiniCUnit.h MiniCUnit.c $(CFLAGS)
	$(CC) -c tester_game_of_life.c $(CFLAGS)
	$(CC) -o $(TEST) $(TEST).o $(TEST_OBJECTS) $(CFLAGS)
	./$(TEST)

clean:
	$(RM) *.o $(TEST) $(MESURE)



