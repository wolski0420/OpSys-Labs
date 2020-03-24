output: main.o list.o
	gcc -Wall main.o list.o -o output

main.o: main.c
	gcc -Wall -c main.c

list.o: list.c list.h
	gcc -Wall -c list.c

clean:
	rm *.o output