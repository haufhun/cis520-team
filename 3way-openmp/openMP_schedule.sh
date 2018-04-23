#!/bin/bash -l

for j in 1 2 4 8 16
do
    for i in 1000 10000 100000 500000 1000000
    do
        k=$(( $i/1000 ))
        #k=$(( $k/$j ))      
        #k=$(( $k < 1 ? 1 : $k ))
        h=$(( $k/60 ))
        m=$(( $k%60 ))
        
        echo "Cores:"$j", Lines:"$i", Time:"$h:$m":00"
        sbatch --mem-per-cpu=4G --time=$h:$m:00 --partition=killable.q --nodes=1 --ntasks-per-node=$j --job-name=LCSomp --mail-type=ALL --mail-user=tjstock@ksu.edu --constraint=elves run.sh $i $j
        echo #newLine
    done
done



