output: main.o library.o
	gcc -Wall main.o library.o -o output

main.o: main.c
	gcc -Wall -c main.c

library.o: library.c library.h
	gcc -Wall -c library.c

clean: 
	rm *.o output

first_optimalization:
	gcc -O0 -Wall -c library.c
	gcc -O0 -Wall -c main.c
	gcc -O0 -Wall main.o library.o -o output

second_optimalization:
	gcc -O1 -Wall -c library.c
	gcc -O1 -Wall -c main.c
	gcc -O1 -Wall main.o library.o -o output

third_optimalization:
	gcc -O2 -Wall -c library.c
	gcc -O2 -Wall -c main.c
	gcc -O2 -Wall main.o library.o -o output

fourth_optimalization:
	gcc -Os -Wall -c library.c
	gcc -Os -Wall -c main.c
	gcc -Os -Wall main.o library.o -o output

test_compare_small_number_of_similar_pairs:
	./output create_table 2 compare_pairs a.txt b.txt c.txt d.txt

test_compare_medium_number_of_similar_pairs:
	./output create_table 8 compare_pairs a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt k.txt l.txt m.txt n.txt o.txt p.txt

test_compare_big_number_of_similar_pairs:
	./output create_table 24 compare_pairs a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt k.txt l.txt m.txt n.txt o.txt p.txt a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt k.txt l.txt m.txt n.txt o.txt p.txt a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt k.txt l.txt m.txt n.txt o.txt p.txt

test_compare_big_number_of_dissimilar_pairs:
	./output create_table 24 compare_pairs a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt

test_compare_medium_number_of_dissimilar_pairs:
	./output create_table 8 compare_pairs a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt

test_compare_small_number_of_dissimilar_pairs:
	./output create_table 2 compare_pairs a.txt h.txt b.txt g.txt

test_delete_big_number_of_blocks:
	./output create_table 24 compare_pairs a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt remove_block_index 0 remove_block_index 1 remove_block_index 2 remove_block_index 3 remove_block_index 4 remove_block_index 5 remove_block_index 6 remove_block_index 7 remove_block_index 8 remove_block_index 9 remove_block_index 10 remove_block_index 11 remove_block_index 12 remove_block_index 13 remove_block_index 14 remove_block_index 15 remove_block_index 16 remove_block_index 17 remove_block_index 18 remove_block_index 19 remove_block_index 20 remove_block_index 21 remove_block_index 22 remove_block_index 23

test_delete_medium_number_of_blocks:
	./output create_table 8 compare_pairs a.txt h.txt b.txt g.txt c.txt f.txt d.txt e.txt i.txt p.txt j.txt o.txt k.txt n.txt l.txt m.txt remove_block_index 0 remove_block_index 1 remove_block_index 2 remove_block_index 3 remove_block_index 4 remove_block_index 5 remove_block_index 6 remove_block_index 7

test_delete_small_number_of_blocks:
	./output create_table 2 compare_pairs a.txt h.txt b.txt g.txt remove_block_index 0 remove_block_index 1

test_alternately_add_and_delete_blocks:
	./output create_table 2 compare_pairs a.txt b.txt remove_block_index 0 compare_pairs a.txt b.txt remove_block_index 0 compare_pairs a.txt b.txt remove_block_index 0 compare_pairs a.txt b.txt remove_block_index 0
