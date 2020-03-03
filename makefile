matrix: matrix.c matrix.h
	gcc -fopenmp -g -o matrix matrix.c matrix.h files.c n_functions.c f_functions.c -std=c11
matrix.o: matrix.c
	gcc -fopenmp -g -c matrix.c
files.o: files.c
	gcc -fopenmp -g -c files.c
n_functions.o: n_functions
	gcc -fopenmp -g -c n_functions.c
f_functions.o: f_functions
	gcc -fopenmp -g -c f_functions.c
