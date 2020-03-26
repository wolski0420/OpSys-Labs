output: main.o matrix.o workers.o
	gcc -Wall main.o matrix.o workers.o  -o output

main.o: main.c
	gcc -Wall -c main.c

workers.o: workers.c workers.h
	gcc -Wall -c workers.c

matrix.o: matrix.c matrix.h
	gcc -Wall -c matrix.c

clean:
	rm *.o output c.t* f.t*
	rm -rf .program