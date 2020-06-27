#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 70

// global variables
int threads_number;
int **histogram;
int (*variant)(int *);

int image_height;
int image_width;
unsigned char **image;

// method to validate
int validate_error(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// it counts difference of two time points and returns microseconds
int time_difference(struct timespec *t1, struct timespec *t2) {
    return (t2->tv_sec - t1->tv_sec)*1000000 + (t2->tv_nsec - t1->tv_nsec)/1000.0;
}

// reading line 
void read_specific_line(char *reader, FILE *input_file) {
    fgets(reader, MAX_LINE_LENGTH, input_file);
    while(reader[0] == '#' || reader[0] == '\n') fgets(reader, MAX_LINE_LENGTH, input_file);
}

// loading image
void load_image_from_file(FILE *input_file) {
    char read_buffer[MAX_LINE_LENGTH + 1] = {0};

    // reading size
    read_specific_line(read_buffer, input_file);
    read_specific_line(read_buffer, input_file);
    image_width = atoi(strtok(read_buffer, " \t\r\n"));
    image_height = atoi(strtok(NULL, " \t\r\n"));

    image = calloc(image_height, sizeof(char *));
    for (int i=0; i<image_height; i++) image[i] = calloc(image_width, sizeof(char));
    
    // reading image
    read_specific_line(read_buffer, input_file);
    read_specific_line(read_buffer, input_file);
    char *encoded = strtok(read_buffer, " \t\r\n");
    for (int i=0; i<image_width*image_height; i++) {
        if (encoded == NULL) {
            read_specific_line(read_buffer, input_file);
            encoded = strtok(read_buffer, " \t\r\n");
        }

        image[i/image_width][i%image_width] = atoi(encoded);
        encoded = strtok(NULL, " \t\r\n");
    }
}

// saving result in histogram to specific file
void save_histogram_to_file(FILE *output_file) {
    // preparing
    int complete_histogram[256] = {0};
    for (int i=0; i<threads_number; i++)
        for (int x=0; x<256; x++) complete_histogram[x] += histogram[i][x];

    float max = complete_histogram[0];
    for(int i=1; i<256; i++) 
        if(max < complete_histogram[i]) max = complete_histogram[i];

    //writing
    fprintf(output_file,"P2\n256 50\n255\n");

    for(int y=0; y<50; y++){
        for(int x=0; x<256; x++){
            if(50-y > 50*(complete_histogram[x]/max)) fprintf(output_file,"0\n");
            else fprintf(output_file, "255\n"); 
        }
    }
}

// processing histogram by sign method
int sign_worker(int *thread_index) {
    // start time
    struct timespec *start_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, start_time);

    // writing histogram
    int size_chunk = 256 / threads_number;
    for (int y=0; y<image_height; y++)
        for (int x=0; x<image_width; x++)
            if (image[y][x]/size_chunk == *thread_index) histogram[0][image[y][x]]++;

    // end time
    struct timespec *end_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, end_time);

    return time_difference(start_time,end_time);
}

// processing histogram by block method
int block_worker(int *thread_index) {
    // start time
    struct timespec *start_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, start_time);

    // writing histogram
    int index = *thread_index;
    int size_chunk = image_width / threads_number;
    for (int x=index*size_chunk; x<(index+1)*size_chunk; x++) 
        for (int y=0; y<image_height; y++) histogram[index][image[y][x]]++;

    // end time
    struct timespec *end_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, end_time);

    return time_difference(start_time,end_time);
}

// processing histogram by interleaved method
int interleaved_worker(int *thread_index) {
    // time start
    struct timespec *start_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, start_time);

    // writing histogram
    int index = *thread_index;
    for (int x=index; x<image_width; x+=threads_number) 
        for (int y=0; y<image_height; y++) histogram[index][image[y][x]]++;
    
    // time end
    struct timespec *end_time = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, end_time);

    return time_difference(start_time, end_time);
}

int main(int argc, char *argv[]) {
    // validating data
    if (argc != 5) return validate_error("Bad number of arguments!");

    threads_number = atoi(argv[1]);
    if(threads_number <= 0) return validate_error("Bad number of threads!");

    if (strcmp(argv[2], "sign") == 0) variant = sign_worker;
    else if (strcmp(argv[2], "block") == 0) variant = block_worker;
    else if (strcmp(argv[2], "interleaved") == 0) variant = interleaved_worker;
    else return validate_error("Bad variant!");

    FILE *input_file = fopen(argv[3], "r");
    if(input_file == NULL) return validate_error("Cannot open file with given name!");

    FILE *output_file = fopen(argv[4], "w+");
    if(output_file == NULL) return validate_error("Cannot open output file!");

    load_image_from_file(input_file);

    // preparing histogram
    histogram = calloc(threads_number, sizeof(int *));
    for (int i=0; i<threads_number; i++) histogram[i] = calloc(256, sizeof(int));

    // preparing threads
    pthread_t *all_threads = calloc(threads_number, sizeof(pthread_t));
    int *args = calloc(threads_number, sizeof(int));

    // starting counting time
    struct timespec *start = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, start);

    // threads creating
    for (int i=0; i<threads_number; i++) {
        args[i] = i;
        pthread_create(&all_threads[i], NULL, (void *(*)(void *))variant, args + i);
    }

    // joining
    for (int i=0; i<threads_number; i++) {
        int time_spend;
        pthread_join(all_threads[i], (void *)&time_spend);
        printf("Time of %d threat work is: %d us\n", i, time_spend);
    }

    // finishing counting time
    struct timespec *end = calloc(1,sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC, end);

    printf("Total time of everything is: %d us\n", time_difference(start,end));

    // closing everything and saving
    save_histogram_to_file(output_file);

    for (int i = 0; i < threads_number; i++) free(histogram[i]);
    for (int y = 0; y < image_height; y++) free(image[y]);
    free(histogram);
    free(image);
    free(all_threads);
    free(args);
    fclose(input_file);
    fclose(output_file);
}