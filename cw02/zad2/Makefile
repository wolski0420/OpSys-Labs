output: main.o find.o
	gcc -Wall main.o find.o -o output

main.o: main.c
	gcc -Wall -c main.c

find.o: find.c find.h
	gcc -Wall -c find.c

clean:
	rm *.o output

test_a:
	make
	./output find ./test_dir -atime -1 -maxdepth 3
	make clean

test_m:
	make
	./output find ./test_dir -mtime -1 -maxdepth 3
	make clean

test_md:
	make
	./output find ./test_dir -maxdepth 3
	make clean