all:
	gcc -Wall -pthread -c main.c -o main.o
	gcc -Wall -pthread main.o -o main
	./main 1 sign brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 2 sign brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 4 sign brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 8 sign brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 1 block brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 2 block brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 4 block brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 8 block brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 1 interleaved brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 2 interleaved brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 4 interleaved brain_604.ascii.pgm brain_604_result.ascii.pgm
	./main 8 interleaved brain_604.ascii.pgm brain_604_result.ascii.pgm

clean:
	rm -f main.o main brain_604_result.ascii.pgm