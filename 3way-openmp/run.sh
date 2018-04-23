#!/bin/bash
echo "Cores:"$2", Lines:"$1
for i in `seq 1 10`;
do
	./LCS $1 $2
done
