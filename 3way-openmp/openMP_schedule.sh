#!/bin/bash -l

for i in 1000 10000 100000 500000 1000000
do
    sbatch --mem-per-cpu=4G --time=3:00:00 --partition=killable.q --nodes=1 --ntasks-per-node=1 --job-name=LCSomp --mail-type=ALL --mail-user=tjstock@ksu.edu --constraint=elves run.sh $i
done


