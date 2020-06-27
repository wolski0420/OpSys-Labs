all:
	gcc -Wall -pthread -c board.c -o board.o
	gcc -Wall -pthread -c server.c -o server.o
	gcc -Wall -pthread board.o server.o -o server
	gcc -Wall -pthread -c client.c -o client.o
	gcc -Wall -pthread board.o client.o -o client
	./server 111 /tmp/socket

clean:
	rm -f *.o client server