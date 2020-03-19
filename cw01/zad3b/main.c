#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

double time_calculate(clock_t start, clock_t end){
    return (double)(end-start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv){
    struct main_array *main_table = NULL;
    int number_of_instructions = 4;
    char **instruction_names = (char**) calloc(number_of_instructions,sizeof(char*));
    instruction_names[0] = "create_table";
    instruction_names[1] = "compare_pairs";
    instruction_names[2] = "remove_block_index";
    instruction_names[3] = "remove_operation_index";

    struct tms *global_su_time_start = calloc(1,sizeof(struct tms));
    struct tms *global_su_time_end = calloc(1,sizeof(struct tms));
    struct tms *su_time_start = calloc(1,sizeof(struct tms));
    struct tms *su_time_end = calloc(1,sizeof(struct tms));
    clock_t global_real_time_start;
    clock_t global_real_time_end;
    clock_t real_time_start;
    clock_t real_time_end;

    global_real_time_start = times(global_su_time_start);
    printf("\n\t\t\t%s\t\t%s\t\t%s\n","Real","System","User");

    int index = 1;
    while(index < argc){
        real_time_start = times(su_time_start);

        if(strcmp(argv[index],"create_table") == 0 && index+1 < argc){
            size_t size_of_table;

            if((size_of_table = strtol(argv[index+1],NULL,10)) == 0){
                printf("%s\n","Bad format of table size!");
                return -1;
            }
            
            main_table = create_main_array(size_of_table);
            index += 2;
            
            printf("%s\t\t","Creating:");
        }
        else if(strcmp(argv[index],"compare_pairs") == 0){
            if(main_table == NULL || main_table->blocks == NULL){
                printf("%s\n","Main table is not created!");
                return -1;
            }

            int can_continue = 1;
            int counter = 0;
            int tmp_index = index+1;

            while(tmp_index < argc && can_continue == 1){
                for(int i=0; i<number_of_instructions; i++){
                    if(strcmp(argv[tmp_index],instruction_names[i]) == 0){
                        can_continue = 0;
                    }
                }

                if(can_continue == 1){
                    counter ++;
                    tmp_index ++;
                }
            }

            if(counter%2 != 0){
                printf("%s","Invalid number of files! It must be even!");
                return -1;
            }

            index ++;
            char **files_names = (char**) calloc(counter,sizeof(char*));
            for(int i=0; i<counter; i++){
                files_names[i] = (char*) calloc(strlen(argv[index]),sizeof(char));
                strcpy(files_names[i],argv[index]);
                index++;
            }

            compare_all_files(files_names,counter,main_table);

            printf("%s\t\t","Comparing:");
        }
        else if(strcmp(argv[index],"remove_block_index") == 0 && index+1 < argc){
            if(main_table == NULL || main_table->blocks == NULL){
                printf("%s\n","Main array is null!");
                return -1;        
            }

            int index_to_remove = strtol(argv[index+1],NULL,10);

            delete_block(main_table, index_to_remove);
            index += 2;

            printf("%s\t\t","Removing block:");
        }
        else if(strcmp(argv[index],"remove_operation_index") == 0 && index+2 < argc){
            if(main_table == NULL || main_table->blocks == NULL){
                printf("%s\n","Block is null!");
                return -1;        
            }

            int index_of_block = strtol(argv[index+1],NULL,10);
            int index_of_operation = strtol(argv[index+2],NULL,10);

            delete_operation(main_table->blocks[index_of_block],index_of_operation);
            index += 3;

            printf("%s\t","Removing operation:");
        }
        else{
            printf("%s\n","Invalid type of argument!");
            return -1;
        }

        real_time_end = times(su_time_end);

        printf("%f\t",time_calculate(real_time_start, real_time_end));
        printf("%f\t",time_calculate(su_time_start -> tms_cstime, su_time_end -> tms_cstime));
        printf("%f\n",time_calculate(su_time_start -> tms_cutime, su_time_end -> tms_cutime));
    }

    global_real_time_end = times(global_su_time_end);

    printf("%s\t\t\t","Global:");
    printf("%f\t",time_calculate(global_real_time_start, global_real_time_end));
    printf("%f\t",time_calculate(global_su_time_start -> tms_cstime, global_su_time_end -> tms_cstime));
    printf("%f\n\n",time_calculate(global_su_time_start -> tms_cutime, global_su_time_end -> tms_cutime));

    if(main_table!=NULL) clear_main_array(main_table);
    free(global_su_time_start);
    free(global_su_time_end);
    free(su_time_start);
    free(su_time_end);

    return 0;
}