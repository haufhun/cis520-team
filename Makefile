CC = gcc

lcsmake: LCS.c
	$(CC) -fopenmp -o lcs LCS.c

clean:
	rm lcs

