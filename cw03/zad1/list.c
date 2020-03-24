#include "list.h"

// function to returning errors during executing find
int list_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// this function creates processes for every directory and "ls -l" them
int scan_directory(char *path){
    if(path == NULL) return list_error("Given path is NULL!");

    DIR *directory = opendir(path);
    if(directory == NULL) return list_error("Cannot open directory!");

    struct dirent *element;
    while((element = readdir(directory)) != NULL){
        if(strcmp(element->d_name,".") != 0 && strcmp(element->d_name,"..") != 0){
            char *path_to_element = calloc(strlen(path)+2+strlen(element->d_name),sizeof(char));
            strcpy(path_to_element,path);
            strcat(path_to_element,"/");
            strcat(path_to_element,element->d_name);

            struct stat *element_stat = calloc(1,sizeof(struct stat));
            stat(path_to_element,element_stat);

            if(S_ISDIR(element_stat->st_mode) != 0 && fork() == 0){
                printf("\nPID: %i\tPath: %s\n",getpid(),path_to_element);
                char *command = calloc(7 + strlen(path_to_element),sizeof(char));
                strcpy(command,"ls -l ");
                strcat(command,path_to_element);
                system(command);
                scan_directory(path_to_element);
                free(command);
                exit(0);
            }
            else{
                wait(NULL);
            }
            free(path_to_element);
            free(element_stat);
        }
    }

    closedir(directory);
    return 0;
}