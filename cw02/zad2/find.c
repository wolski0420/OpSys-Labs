#include "find.h"

// function to returning errors during executing find
int find_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// function to print file statistics 
void print_file_info(char *path, struct stat *element){
    printf("%li\t",element->st_nlink);                                                          // hard links

    if(S_ISREG(element->st_mode) != 0) printf("file\t");                        // file?
    else if(S_ISDIR(element->st_mode) != 0) printf("dir\t");                 // directory?
    else if(S_ISCHR(element->st_mode) != 0) printf("char dev\t");    // char device?
    else if(S_ISBLK(element->st_mode) != 0) printf("block dev\t");   // block device?
    else if(S_ISFIFO(element->st_mode) != 0) printf("fifo\t");             // fifo?
    else if(S_ISLNK(element->st_mode) != 0) printf("slink\t");            // symbolic link?
    else if(S_ISSOCK(element->st_mode) != 0) printf("sock\t");         // socket?
    else printf("unknown\t");
    
    printf("%ld\t",element->st_size);                                                          // size in bytes

    char *a_time = ctime(&element->st_atime);
    for(int i=0; i<24; i++) printf("%c",a_time[i]);                                      // access time
    printf("\t");

    char *m_time = ctime(&element->st_mtime);
    for(int i=0; i<24; i++) printf("%c",m_time[i]);                                    // modify time

    char *direct_path = realpath(path,NULL);
    printf("\t%s\n",direct_path);                                                                 // path to file
}

// it's common function for callings -atime and -mtime, depth here is optional (-1 when not calling -maxdepth)
int find_time(char *path, char *operation, char sign, int number, int depth){
    // validate arguments
    if(path == NULL) return find_error("Path to directory is NULL!");
    if(operation == NULL) return find_error("Operation of find function is NULL!");
    if(depth <1 && depth != -1) return find_error("Depth < 1, I can't show you anything!");

    // opening the directory given in path
    DIR *directory = opendir(path);
    if(directory == NULL) return find_error("Cannot open a directory!");

    // iterating on elements in directory
    struct dirent *element;
    while((element = readdir(directory)) != NULL){
        // we don't want to see . and .. in find result
        if(strcmp(element->d_name,".") != 0 && strcmp(element->d_name,"..")){
            // creating path to element
            char *path_to_element = calloc(strlen(path)+1+strlen(element->d_name),sizeof(char));
            strcpy(path_to_element,path);
            strcat(path_to_element,"/");
            strcat(path_to_element,element->d_name);

            // getting statistics of element
            struct stat *element_stat =  calloc(1,sizeof(struct stat));
            stat(path_to_element,element_stat);

            // getting actual time
            time_t now = time(NULL);
            localtime(&now);

            // getting access/modify time
            time_t time_called = time(NULL);
            if(strcmp(operation,"-atime") == 0) time_called = element_stat->st_atime;
            else if(strcmp(operation,"-mtime") == 0) time_called = element_stat->st_mtime;
            else return find_error("Bad format of argument -atime/-mtime!");

            // counting difference between actual time and last access/modify time
            int time_difference = (int)(difftime(now,time_called)/86400);

            // printing only these ones which are compatible with given arguments (+n,-n,n)
            if(sign == '!' && time_difference == number) print_file_info(path_to_element,element_stat);
            else if(sign == '-' && time_difference < number) print_file_info(path_to_element,element_stat);
            else if(sign == '+' && time_difference > number) print_file_info(path_to_element,element_stat);

            // if directory and not max depth reached, we're going deeper
            if(S_ISDIR(element_stat->st_mode) != 0 && depth > 1){
                if(find_time(path_to_element,operation,sign,number,depth-1) == -1) return -1;
            }
        }
    }

    // closing directory and leaving
    closedir(directory);
    return 0;
}

// classic -max_depth parameter, without -atime or -mtime
int max_depth(char *path, int depth){
    // validating arguments
    if(path == NULL) return find_error("Path to directory is NULL!");
    if(depth < 1) return find_error("Depth < 1, I can't show you anything!");

    // opening directory given in path
    DIR *directory = opendir(path);
    if(directory == NULL) return find_error("Cannot open a directory!");

    // iterating on elements in directory
    struct dirent *element;
    while((element = readdir(directory)) != NULL){
        // we don't want to see . and .. in find result
        if(strcmp(element->d_name,".") != 0 && strcmp(element->d_name,"..") != 0){
            // creating path to element
            char *path_to_element = calloc(strlen(path)+1+strlen(element->d_name),sizeof(char));
            strcpy(path_to_element,path);
            strcat(path_to_element,"/");
            strcat(path_to_element,element->d_name);

            // getting statistics of element
            struct stat *element_stat = calloc(1,sizeof(struct stat));
            stat(path_to_element,element_stat);
        
            // printing information about element
            print_file_info(path_to_element, element_stat);
            
            // if directory and not max depth reached, we're going deeper
            if(S_ISDIR(element_stat->st_mode) != 0 && depth > 1){
                if(max_depth(path_to_element,depth-1) == -1) return -1;
            }
        }
    }

    closedir(directory);
    return 0;
}