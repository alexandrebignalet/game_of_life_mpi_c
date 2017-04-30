#!/bin/sh

git add .

git commit -m "adding source"

git push origin master

ssh bignalet_a@turing.hpc.uqam.ca 'bash -s' < execute.sh