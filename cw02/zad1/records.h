#ifndef records_functionality_h
#define records_functionality_h

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int generate(char *filename,  int number_of_records, int length_of_record);

int sort(char *filename, int number_of_records, int length_of_record, char *variant);

int copy(char *filename1, char *filename2,  int number_of_records, int length_of_record, char *variant);

#endif