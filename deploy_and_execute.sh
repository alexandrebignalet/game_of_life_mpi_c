#!/bin/sh

ssh bignalet_a:BIGA2206

git pull origin master

make clean

make

make mesures

make clean