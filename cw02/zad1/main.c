#include "records.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>

int main_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

double time_calculate(clock_t start, clock_t end){
    return (double)(end-start)/(double)sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv){
    struct tms *su_time_start = calloc(1,sizeof(struct tms));
    struct tms *su_time_end = calloc(1,sizeof(struct tms));
    
    int  index = 1;
    while(index < argc){
        if(strcmp(argv[index],"generate") == 0){
            // validating arguments:
            if(index + 4 > argc) return main_error("Invalid number of arguments for \"generate\" function!");

            char *check = NULL;
            int number_of_records = (int)strtol(argv[index+2],&check,10);
            if(strcmp(check,"") != 0){
                printf("%s\n",check);
                return main_error("Cannot read second argument of \"generate\" function!");
            }

            int length_of_records = (int)strtol(argv[index+3],&check,10);
            if(strcmp(check,"") != 0){
               printf("%s\n",check);
                return main_error("Cannot read third argument of \"generate\" function!");
            }

            index += 4;

            // generating:
            if(generate(argv[index-3],number_of_records,length_of_records) == -1) return -1;
        }
        else if(strcmp(argv[index],"sort") == 0){
            // validating arguments:
            if(index + 5 > argc) return main_error("Invalid number of arguments for \"sort\" function!");

            char *check;
            int number_of_records = (int)strtol(argv[index+2],&check,10);

            if(strcmp(check,"") != 0){
                printf("%s\n",check);
                return main_error("Cannot read third argument of  \"copy\" function!");
            }

            int length_of_records = (int)strtol(argv[index+3],&check,10);

            if(strcmp(check,"") != 0){
                printf("%s\n",check);
                return main_error("Cannot read fourth argument of  \"copy\" function!");
            }
            
            index += 5;

            // sorting and reporting time:
            printf("Number of records: %d, Length of record: %d",number_of_records,length_of_records);
            printf("\n\t\t%s\t\t%s\n","System","User");
            times(su_time_start);

            if(sort(argv[index-4],number_of_records,length_of_records,argv[index-1]) == -1) return -1;

            times(su_time_end);
            printf("Sorting by %s\t%f\t%f\n",argv[index-1],
                        time_calculate(su_time_start->tms_stime,su_time_end->tms_stime),
                        time_calculate(su_time_start->tms_utime,su_time_end->tms_utime));
        }
        else if(strcmp(argv[index],"copy") == 0){
            // validating arguments:
            if(index + 6 > argc) return main_error("Invalid number of arguments for \"copy\" function!");

            char *check = NULL;
            int number_of_records = (int)strtol(argv[index+3],&check,10);

            if(strcmp(check,"") != 0){
                printf("%s\n",check);
                return main_error("Cannot read third argument of  \"copy\" function!");
            }

            int length_of_records = (int)strtol(argv[index+4],&check,10);

            if(strcmp(check,"") != 0){
                printf("%s\n",check);
                return main_error("Cannot read fourth argument of  \"copy\" function!");
            }

            index += 6;

            // copying and reporting time:
            printf("Number of records: %d, Length of record: %d",number_of_records,length_of_records);
            printf("\n\t\t%s\t\t%s\n","System","User");
            times(su_time_start);

            if(copy(argv[index-5],argv[index-4],number_of_records,length_of_records,argv[index-1]) == -1) return -1;

            times(su_time_end);
            printf("Copying by %s\t%f\t%f\n",argv[index-1],
                        time_calculate(su_time_start->tms_stime,su_time_end->tms_stime),
                        time_calculate(su_time_start->tms_utime,su_time_end->tms_utime));
        }
        else return main_error("Invalid type of argument!");
    }

    free(su_time_start);
    free(su_time_end);
    return 0;
}