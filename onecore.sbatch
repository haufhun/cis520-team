#!/bin/bash -l

#SBATCH --mem-per-cpu=4G   # Memory per core, use --mem= for memory per node
#SBATCH --time=1:00:00   # Use the form DD-HH:MM:SS
#SBATCH --partition=killable.q   # Job may run as killable on owned nodes
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name=LCSTestOne
#SBATCH --mail-type=ALL   # same as =BEGIN,FAIL,END
#SBATCH --mail-user=haufhun@ksu.edu

# $HOME/cis520/lcs
