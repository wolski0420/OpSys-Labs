output: main.o library.o
	gcc -Wall main.o library.o -o output

main.o: main.c
	gcc -Wall -c main.c

library.o: library.c library.h
	gcc -Wall -c library.c

clean: 
	rm *.o output
