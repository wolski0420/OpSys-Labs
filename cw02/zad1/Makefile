output: main.o records.o
	gcc -Wall main.o records.o -o output

main.o: main.c
	gcc -Wall -c main.c

records.o: records.c records.h
	gcc -Wall -c records.c

clean:
	rm *.o output

test_size1:
	make
	./output generate data 3000 1 copy data data_sys 3000 1 sys copy data data_lib 3000 1 lib sort data_sys 3000 1 sys sort data_lib 3000 1 lib
	./output generate data 15000 1 copy data data_sys 15000 1 sys copy data data_lib 15000 1 lib sort data_sys 15000 1 sys sort data_lib 15000 1 lib
	rm *.o output data data_sys data_lib

test_size4:
	make
	./output generate data 20000 4 copy data data_sys 20000 4 sys copy data data_lib 20000 4 lib sort data_sys 20000 4 sys sort data_lib 20000 4 lib
	./output generate data 200000 4 copy data data_sys 200000 4 sys copy data data_lib 200000 4 lib sort data_sys 200000 4 sys sort data_lib 200000 4 lib
	rm *.o output data data_sys data_lib

test_size512:
	make
	./output generate data 40000 512 copy data data_sys 40000 512 sys copy data data_lib 40000 512 lib sort data_sys 40000 512 sys sort data_lib 40000 512 lib
	./output generate data 400000 512 copy data data_sys 400000 512 sys copy data data_lib 400000 512 lib sort data_sys 400000 512 sys sort data_lib 400000 512 lib
	rm *.o output data data_sys data_lib

test_size1024:
	make
	./output generate data 80000 1024 copy data data_sys 80000 1024 sys copy data data_lib 80000 1024 lib sort data_sys 80000 1024 sys sort data_lib 80000 1024 lib
	./output generate data 800000 1024 copy data data_sys 800000 1024 sys copy data data_lib 800000 1024 lib sort data_sys 800000 1024 sys sort data_lib 800000 1024 lib
	rm *.o output data data_sys data_lib

test_size4096:
	make
	./output generate data 20000 4096 copy data data_sys 20000 4096 sys copy data data_lib 20000 4096 lib sort data_sys 20000 4096 sys sort data_lib 20000 4096 lib
	./output generate data 200000 4096 copy data data_sys 200000 4096 sys copy data data_lib 200000 4096 lib sort data_sys 200000 4096 sys sort data_lib 200000 4096 lib
	rm *.o output data data_sys data_lib

test_size8192:
	make
	./output generate data 10000 8192 copy data data_sys 10000 8192 sys copy data data_lib 10000 8192 lib sort data_sys 10000 8192 sys sort data_lib 10000 8192 lib
	./output generate data 100000 8192 copy data data_sys 100000 8192 sys copy data data_lib 100000 8192 lib sort data_sys 100000 8192 sys sort data_lib 100000 8192 lib
	rm *.o output data data_sys data_lib