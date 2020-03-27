output: main.o matrix.o workers.o
	gcc -Wall main.o matrix.o workers.o  -o output

test: support.o matrix.o workers.o
	gcc -Wall support.o matrix.o workers.o  -o output_test

support.o: support.c
	gcc -Wall -c support.c

main.o: main.c
	gcc -Wall -c main.c

workers.o: workers.c workers.h
	gcc -Wall -c workers.c

matrix.o: matrix.c matrix.h
	gcc -Wall -c matrix.c

clean:
	rm -rf .program
	rm output
	rm *.o  c.t* f.t*

clean_test:
	rm -rf .files
	rm output_test
	rm lista
	rm *.o
	rm -rf .program