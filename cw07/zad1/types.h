#ifndef TYPES_H
#define TYPES_H

#define MAX_PACKAGES_COUNT 15
#define GETTERS_NUMBER 7
#define PACKERS_NUMBER 4
#define SENDERS_NUMBER 3
#define STATUS_GOT 1
#define STATUS_PACKED 2
#define STATUS_SENT 3
#define INDEX_SPACE 0
#define INDEX_GET 1
#define INDEX_PACK 2
#define INDEX_MODIFY 3
#define TOTAL_WORKERS GETTERS_NUMBER+PACKERS_NUMBER+SENDERS_NUMBER

// structure for packages from exercise
typedef struct {
    int n;
    int status;     // statuses like above
} package;

// structure for common memory space
typedef struct{
    int size;
    package packages[MAX_PACKAGES_COUNT];
    int index;
} memory;

#endif