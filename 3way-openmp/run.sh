#!/bin/bash

make clean
make

for i in `seq 1 10`;
do
	 ./LCS $1
done

