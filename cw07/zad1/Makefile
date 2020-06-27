all:
	gcc -Wall -c common.c -o common.o
	gcc -Wall -c main.c -o main.o 
	gcc -Wall common.o main.o -o main 
	gcc -Wall -c getter.c -o getter.o
	gcc -Wall common.o getter.o -o getter 
	gcc -Wall -c packer.c -o packer.o
	gcc -Wall common.o packer.o -o packer 
	gcc -Wall -c sender.c -o sender.o
	gcc -Wall common.o sender.o -o sender 
	./main

clean:
	rm main getter packer sender *.o 