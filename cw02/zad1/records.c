#include "records.h"

int records_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

int compare_records(char *record1, char *record2){
    if(record1 == NULL) return records_error("Record1 is empty!");
    if(record2 == NULL) return records_error("Record2 is empty!");
    if(strlen(record1) > strlen(record2)) return 2;
    if(strlen(record1) < strlen(record2)) return -2;

    for(int i=0; i<strlen(record1); i++){
        if(record1[i] > record2[i]) return -2;
        if(record1[i] < record2[i]) return 2;
    }

    return 0;
}

// system methods

int swap_sys(int file_descriptor, int length_of_record, int index1, int index2){
    char *record1 = calloc(length_of_record,sizeof(char));
    char *record2 = calloc(length_of_record,sizeof(char));
    if(lseek(file_descriptor,(length_of_record+1)*index1,0) < 0) return records_error("Cannot change position in file at swap for index1!");
    if(read(file_descriptor,record1,length_of_record) < 0) return records_error("Cannot read line in file at swap for index1!");
    if(lseek(file_descriptor,(length_of_record+1)*index2,0) < 0) return records_error("Cannot change position in file at swap for index2!");
    if(read(file_descriptor,record2,length_of_record) < 0) return records_error("Cannot read line in file at swap for index2!");

    if(lseek(file_descriptor,(length_of_record+1)*index1,0) < 0) return records_error("Cannot change position in file at swap for index1 (second time)!");
    if(write(file_descriptor,record2,length_of_record) < 0) return records_error("Cannot write to file at swap for index1!");
    if(lseek(file_descriptor,(length_of_record+1)*index2,0) < 0) return records_error("Cannot change position in file at swap for index2 (second time)!");
    if(write(file_descriptor,record1,length_of_record) < 0) return records_error("Cannot write to file at swap for index2!");

    free(record1);
    free(record2);
    return 0;
}

int partition_sys(int file_descriptor, int length_of_record, int left, int right){
    int pivot_index = right, i = left;
    char *pivot = calloc(length_of_record,sizeof(char));
    if(lseek(file_descriptor,(length_of_record+1)*pivot_index,0) < 0) return records_error("Cannot change position in file for pivot!");
    if(read(file_descriptor,pivot,length_of_record) < 0) return records_error("Cannot read line in file for pivot!");

    for(int j=left; j<pivot_index; j++){
        char *record_to_compare = calloc(length_of_record,sizeof(char));
        if(lseek(file_descriptor,(length_of_record+1)*j,0) < 0) return records_error("Cannot change position in file for record!");
        if(read(file_descriptor,record_to_compare,length_of_record) < 0) return records_error("Cannot read line in file for record!");

        int compare_result = compare_records(record_to_compare,pivot);
        if(compare_result == -1) return -1;
        if(compare_result >= 0) 
            if(swap_sys(file_descriptor,length_of_record,j,i++) == -1) return -1;
        
        free(record_to_compare);
    }

    swap_sys(file_descriptor,length_of_record,i,pivot_index);
    free(pivot);
    return i;
}

int quick_sort_sys(int file_descriptor, int length_of_record, int left, int right){
    if(left < right){
        int pivot = partition_sys(file_descriptor, length_of_record, left, right);
        if(pivot == -1) return -1;
        if(quick_sort_sys(file_descriptor,length_of_record,left,pivot-1) == -1) return -1;
        if(quick_sort_sys(file_descriptor,length_of_record,pivot+1,right) == -1) return -1;
    }
    return 0;
}

// library methods

int swap_lib(FILE *file, int length_of_record, int index1, int index2){
    char *record1 = calloc(length_of_record,sizeof(char));
    char *record2 = calloc(length_of_record,sizeof(char));
    if(fseek(file,(length_of_record+1)*index1,0) != 0) return records_error("Cannot change position in file at swap for index1!");
    if(fread(record1,1,length_of_record,file) != length_of_record) return records_error("Cannot read line in file at swap for index1!");
    if(fseek(file,(length_of_record+1)*index2,0) != 0) return records_error("Cannot change position in file at swap for index2!");
    if(fread(record2,1,length_of_record,file) != length_of_record) return records_error("Cannot read line in file at swap for index2!");

    if(fseek(file,(length_of_record+1)*index1,0) != 0) return records_error("Cannot change position in file at swap for index1 (second time)!");
    if(fwrite(record2,1,length_of_record,file) != length_of_record) return records_error("Cannot write to file at swap for index1!");
    if(fseek(file,(length_of_record+1)*index2,0) != 0) return records_error("Cannot change position in file at swap for index2 (second time)!");
    if(fwrite(record1,1,length_of_record,file) != length_of_record) return records_error("Cannot write to file at swap for index2!");

    free(record1);
    free(record2);
    return 0;
}

int partition_lib(FILE *file, int length_of_record, int left, int right){
    int pivot_index = right, i = left;
    char *pivot = calloc(length_of_record,sizeof(char));
    if(fseek(file,(length_of_record+1)*pivot_index,0) != 0) return records_error("Cannot change position in file for pivot!");
    if(fread(pivot,1,length_of_record,file) != length_of_record) return records_error("Cannot read line in file for pivot!");

    for(int j=left; j<pivot_index; j++){
        char *record_to_compare = calloc(length_of_record,sizeof(char));
        if(fseek(file,(length_of_record+1)*j,0) != 0) return records_error("Cannot change position in file for record!");
        if(fread(record_to_compare,1,length_of_record,file) != length_of_record) return records_error("Cannot read line in file for record!");

        int compare_result = compare_records(record_to_compare,pivot);
        if(compare_result == -1) return -1;
        if(compare_result >= 0) 
            if(swap_lib(file,length_of_record,j,i++) == -1) return -1;

        free(record_to_compare);
    }

    swap_lib(file,length_of_record,i,pivot_index);
    free(pivot);
    return i;
}

int quick_sort_lib(FILE *file, int length_of_record, int left, int right){
    if(left < right){
        int pivot = partition_lib(file,length_of_record,left,right);
        if(pivot == -1) return -1;
        if(quick_sort_lib(file,length_of_record,left,pivot-1) == -1) return -1;
        if(quick_sort_lib(file,length_of_record,pivot+1,right) == -1) return -1; 
    }
   return 0;
}

// "records.h" methods

int generate(char *filename,  int number_of_records, int length_of_record){
    if(filename == NULL) return records_error("Filename is NULL!");
    if(number_of_records <= 0) return records_error("Number of records is incorrect!");
    if(length_of_record <= 0) return records_error("Length of records is incorrect!");

    int file_descriptor = open(filename,O_WRONLY | O_CREAT ,0777);

    if(file_descriptor < 0){
        close(file_descriptor);
        return records_error("Cannot open file with that name!");
    }

    char *buffor = calloc(length_of_record+1,sizeof(char));
    srand(time(NULL));

    for(int i=0; i<number_of_records; i++){
        for(int j=0; j<length_of_record; j++){
            buffor[j] = rand()%26+65;
        }
        buffor[length_of_record] = '\n';

        if(write(file_descriptor,buffor,length_of_record+1) != length_of_record+1){
            close(file_descriptor);
            return records_error("Cannot write file!");
        }
    }

    free(buffor);
    close(file_descriptor);
    return 0;
}

int sort(char *filename, int number_of_records, int length_of_record, char *variant){
    if(variant == NULL) return records_error("You didnt choose a variant!");
    if(filename == NULL) return records_error("Filename is NULL!");
    if(number_of_records <= 0) return records_error("Number of records is incorrect!");
    if(length_of_record <= 0) return records_error("Length of records is incorrect!");

    if(strcmp(variant,"sys") == 0){
        int file_descriptor = open(filename,O_RDWR);
        if(file_descriptor < 0) return records_error("Cannot open file!");

        if(quick_sort_sys(file_descriptor,length_of_record,0,number_of_records-1) == -1) return -1;

        close(file_descriptor);
    }
    else if(strcmp(variant,"lib") == 0){
        FILE *file = fopen(filename,"r+");
        if(file == NULL) return records_error("Cannot open file!");

        if(quick_sort_lib(file,length_of_record,0,number_of_records-1) == -1) return -1;

        fclose(file);
    }
    else return records_error("Invalid type of variant!");

    return 0;
}

int copy(char *filename1, char *filename2,  int number_of_records, int length_of_record, char *variant){
    if(variant == NULL) return records_error("You didnt choose a variant!");
    if(filename1 == NULL) return records_error("Filename1 is NULL!");
    if(filename2 == NULL) return records_error("Filename2 is NULL!");
    if(number_of_records <= 0) return records_error("Number of records is incorrect!");
    if(length_of_record <= 0) return records_error("Length of records is incorrect!");

    if(strcmp(variant,"sys") == 0){
        int file1_descriptor = open(filename1,O_RDONLY);
        int file2_descriptor = open(filename2,O_WRONLY|O_CREAT,0777);

        if(file1_descriptor < 0){
            close(file1_descriptor);
            return records_error("Cannot open file1!");
        }

        char *buffor = calloc(length_of_record+1,sizeof(char));

        for(int i=0; i<number_of_records; i++){
            if(read(file1_descriptor,buffor,length_of_record+1) < 0){
                close(file1_descriptor);
                close(file2_descriptor);
                return records_error("Cannot read the content of file1!");
            }

            if(write(file2_descriptor,buffor,length_of_record+1) != length_of_record+1){
                close(file1_descriptor);
                close(file2_descriptor);
                return records_error("Cannot write the content of file1 to file2!");
            }
        }

        close(file1_descriptor);
        close(file2_descriptor);
        free(buffor);
    }
    else if(strcmp(variant,"lib") == 0){
        FILE *file1 = fopen(filename1,"r");
        FILE *file2 = fopen(filename2,"wa");

        if(file1 == NULL) return records_error("Cannot open file1!");
        
        char *buffor = calloc(length_of_record+1,sizeof(char));
        
        for(int i=0; i<number_of_records; i++){
            if(fread(buffor,1,length_of_record+1,file1) <= 0) return records_error("Cannot read from file1!");
            if(fwrite(buffor,1,length_of_record+1,file2) <= 0) return records_error("Cannot write to file2!");
        }

        fclose(file1);
        fclose(file2);
        free(buffor);
    }
    else return records_error("Invalid type of variant!");

    return 0;
}