#ifndef sysopy_library_h
#define sysopy_library_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// structures

struct pair{
    char *first;
    char *second;
};

struct pair_sequence{
    struct pair **pairs;
    size_t number_of_pairs;
};

struct block{
    char **operations;
    size_t number_of_operations;
};

struct main_array{
    struct block **blocks;
    size_t number_of_blocks;
    size_t number_of_occupied_blocks;
};

// functions

struct block *create_block(size_t size);

struct main_array *create_main_array(size_t size);

struct pair *create_pair(char *file1_name, char *file2_name);

struct pair_sequence *create_pair_sequence(char **file_names, size_t size);

void compare_files(struct pair *pair_to_compare, int number);

void compare_all_files(char **names_of_files, size_t size, struct main_array *all_blocks);

struct block *parse_file_to_block(int file_index);

void delete_block(struct main_array *main_array, int index);

void delete_operation(struct block *block, int index);

void clear_pair_sequence(struct pair_sequence *sequence);

void clear_block(struct block *block_to_free);

void clear_main_array(struct main_array *all_blocks);

#endif