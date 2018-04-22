#!/bin/bash -l

#SBATCH --mem-per-cpu=4G   # Memory per core, use --mem= for memory per node
#SBATCH --time=2:00:00   # Use the form DD-HH:MM:SS
#SBATCH --partition=killable.q   # Job may run as killable on owned nodes
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name=LCSomp
#SBATCH --mail-type=ALL   # same as =BEGIN,FAIL,END
#SBATCH --mail-user=tjstock@ksu.edu
#SBATCH --constraint=elves

for i in 1000 10000 100000 500000 1000000
do
    sbatch --mem-per-cpu=4G --time=1:00:00 --partition=killable.q --nodes=1 --ntasks-per-node=1 --job-name=LCSomp --mail-type=ALL --mail-user=tjstock@ksu.edu --constraint=elves run.sh $i
end


