#include "library.h"

struct block *create_block(size_t size){
    if (size < 1) return NULL;

    struct block *array = (struct block*) calloc(1,sizeof(struct block));
    array -> operations = (char**) calloc(size, sizeof(char*));
    array -> number_of_operations = size;
    return array;
}

struct main_array *create_main_array(size_t size){
    if (size < 1) return NULL;

    struct main_array *array = (struct main_array*) calloc(1,sizeof(struct main_array));
    array -> blocks = (struct block**) calloc(size, sizeof(struct block*));
    array -> number_of_blocks = size;
    array -> number_of_occupied_blocks = 0;

    for(int i=0; i<size; i++) {
        array -> blocks[i] = NULL;
    }

    return array;
}

struct pair *create_pair(char *file1_name, char *file2_name){
    struct pair *new_pair = (struct pair*) calloc(1,sizeof(struct pair));
    new_pair -> first = file1_name;
    new_pair -> second = file2_name;
    return new_pair;
}

struct pair_sequence *create_pair_sequence(char **file_names, size_t size){
    struct pair_sequence *new_sequence = (struct pair_sequence*) calloc(1, sizeof(struct pair_sequence));
    new_sequence -> number_of_pairs = size/2;
    new_sequence -> pairs = (struct pair**) calloc(size/2, sizeof(struct pair*));

    for(int i=0; i<size; i+=2){
        FILE *file1 = fopen(file_names[i],"r");
        FILE *file2 = fopen(file_names[i+1],"r");

        if(!file1){
            printf("%s","Not found a file named: ");
            printf("%s\n",file_names[i]);
            fclose(file1);
            fclose(file2);
            clear_pair_sequence(new_sequence);
            return NULL;
        }
        if(!file2){
            printf("%s","Not found a file named: ");
            printf("%s\n",file_names[i+1]);
            fclose(file1);
            fclose(file2);
            clear_pair_sequence(new_sequence);
            return NULL;
        }

        fclose(file1);
        fclose(file2);

        new_sequence -> pairs[i/2] = create_pair(file_names[i],file_names[i+1]);
    }

    return new_sequence;
}

void compare_files(struct pair *pair_to_compare, int number){
    int to_divide = number;
    int counter = 0;

    if(to_divide == 0){
        counter = 1;
    }
    else if(to_divide < 0){
        printf("%s\n", "Uncorrect index of file name!");
        return;
    }
    else{
        while(to_divide > 0){
            to_divide /= 10;
            counter ++;
        }
    }
    
    char *command = calloc(strlen(pair_to_compare->first)+strlen(pair_to_compare->second)+25+counter, sizeof(char));
    strcpy(command,"diff ");
    strcat(command,pair_to_compare->first);
    strcat(command," ");
    strcat(command,pair_to_compare->second);
    strcat(command," > diff_result_");

    char *pointer = calloc(counter, sizeof(char));
    for(int i=counter-1; i>=0; i--){
        pointer[i] = number%10 + '0';
        number /= 10;
    }

    strcat(command,pointer);
    strcat(command,".txt");
    system(command);
    free(command);
    free(pointer);
}

struct block *parse_file_to_block(int file_index){
    // finding the number of digits in index

    int to_divide = file_index;
    int counter = 0;

    if(to_divide == 0){
        counter = 1;
    }
    else if(to_divide < 0){
        printf("%s\n", "Uncorrect index of file name!");
        return NULL;
    }
    else{
        while(to_divide > 0){
            to_divide /= 10;
            counter ++;
        }
    }

    // creating name of diff_result file

    char *file_name = calloc(16+counter, sizeof(char));
    strcpy(file_name,"diff_result_");

    char *pointer = calloc(counter, sizeof(char));
    for(int i=counter-1; i>=0; i--){
        pointer[i] = file_index%10 + '0';
        file_index /= 10;
    }

    strcat(file_name,pointer);
    strcat(file_name,".txt");

    // reading file

    FILE *file = fopen(file_name,"r");

    // getting number of lines

    counter = 0;
    size_t line_length = 0;
    char *line = NULL;
    while(getline(&line,&line_length,file) != EOF){
        if('0' <= line[0] && line[0] <= '9') counter ++;
    }

    // counting characters for each of blocks 

    file = fopen(file_name,"r");
    int i = -1;
    size_t *sizes_of_blocks = calloc(counter,sizeof(size_t));
    while(getline(&line,&line_length,file) != EOF){
        if('0' <= line[0] && line[0] <= '9')i++;
        sizes_of_blocks[i] += strlen(line)-1;
    }
    
    // parsing lines to blocks 

    struct block *new_block = create_block(counter);
    new_block -> number_of_operations = counter;
    for(int i=0; i<counter; i++){
        new_block -> operations[i] = (char*) calloc(sizes_of_blocks[i],sizeof(char));
    }

    i = -1;
    file = fopen(file_name,"r");
    while(getline(&line,&line_length,file) != EOF){
        if('0' <= line[0] && line[0] <= '9')i++;
        strncat(new_block -> operations[i], line, strlen(line)-1);
    }
    fclose(file);

    char *command = calloc(3+strlen(file_name),sizeof(char));
    strcpy(command,"rm ");
    strcat(command,file_name);
    system(command);

    free(file_name);
    free(pointer);
    free(sizes_of_blocks);
    free(command);

    return new_block;
}

void compare_all_files(char **names_of_files, size_t size, struct main_array *all_blocks){
    if(size%2 != 0){
        printf("%s","Odd number of files!");
        return;
    }
    if(all_blocks == NULL || all_blocks->blocks == NULL){
        printf("%s","Main array is not created!");
        return;
    }
    
    if(all_blocks->number_of_blocks-all_blocks->number_of_occupied_blocks < size/2){
        printf("%s\n","Error - not enough space for inserting blocks!");
        return;
    }

    struct pair_sequence *sequence = create_pair_sequence(names_of_files, size);

    for(int i=0; i<sequence->number_of_pairs; i++){
        compare_files(sequence->pairs[i],i);
    }

    if(all_blocks->number_of_occupied_blocks == 0){
        for(int i=0; i<sequence->number_of_pairs; i++){
            all_blocks -> blocks[all_blocks->number_of_occupied_blocks++] = parse_file_to_block(i);
        }
    }
    else{
        int index_to_insert = 0;
        for(int i=0; i<sequence->number_of_pairs; i++){
            while(index_to_insert < all_blocks->number_of_blocks && all_blocks->blocks[index_to_insert] != NULL){
                index_to_insert++;
            }

            if(index_to_insert < all_blocks->number_of_blocks){
                all_blocks->blocks[index_to_insert] = parse_file_to_block(i);
                all_blocks->number_of_occupied_blocks++;
                index_to_insert++;
            }
        }
    }

    clear_pair_sequence(sequence);
}

void delete_block(struct main_array *main_array, int index){
    if(main_array == NULL || main_array->blocks == NULL){
        printf("%s\n","Main array is null!");
        return;
    }
    if(index < 0 || index >= main_array->number_of_blocks){
        printf("%s\n","Invalid index - out of boundary!");
        return;
    }
    if(main_array->blocks[index] == NULL){
        printf("%s\n","Invalid index - block doesn't exist!");
        return;
    }

    clear_block(main_array->blocks[index]);
    main_array->blocks[index] = NULL;
    main_array->number_of_occupied_blocks--;
}

void delete_operation(struct block *block, int index){
    if(block == NULL || block->operations == NULL){
        printf("%s\n","Block is null!");
        return;        
    }
    if(index < 0 || index >= block->number_of_operations){
        printf("%s\n","Invalid index - out of boundary!");
        return;
    }
    if(block->operations[index] == NULL){
        printf("%s\n","Invalid index - block doesn't exist!");
        return;
    }

    free(block->operations[index]);
    block->operations[index] = NULL;
}

void clear_pair_sequence(struct pair_sequence *sequence){
    if(sequence == NULL) return;
    else if(sequence->pairs == NULL){
        free(sequence);
        return;
    }

    for(int i=0; i<sequence->number_of_pairs; i++){
        if(sequence->pairs[i] != NULL){
            free(sequence->pairs[i]);
        }
    }
    free(sequence->pairs);
    free(sequence);
}

void clear_block(struct block *block_to_free){
    if(block_to_free == NULL) return;
    else if(block_to_free -> operations == NULL){
        free(block_to_free);
        return;
    }

    for(int i=0; i<block_to_free->number_of_operations; i++){
        if(block_to_free->operations[i] != NULL) {
            free(block_to_free->operations[i]);
        }
    }
    free(block_to_free->operations);
    free(block_to_free);
}

void clear_main_array(struct main_array *all_blocks){
    if(all_blocks == NULL) return;
    else if(all_blocks -> blocks == NULL){
        free(all_blocks);
        return;
    }

    for(int i=0; i<all_blocks->number_of_occupied_blocks; i++){
        if(all_blocks->blocks[i] != NULL) {
            clear_block(all_blocks->blocks[i]);
        }
    }
    free(all_blocks->blocks);
    free(all_blocks);
}