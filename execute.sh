#!/usr/bin/env bash


cd ~/Documents

rm -r game_of_life_mpi_c

git clone https://github.com/alexandrebignalet/game_of_life_mpi_c.git

cd game_of_life_mpi_c

make clean

make compile

make debug1

make clean