#!/bin/sh
#SBATCH --memper-cpu=512M # Memory per core, use --mem= for memory per node
#SBATCH --time=00:00:30 # Use the form DD-HH:MM:SS
#SBATCH --partition=killable.q # Job may run as killable on owned nodes
#..
#..
#SBATCH --mail-user=haufhun@ksu.edu

$HOME/myhost.sh

