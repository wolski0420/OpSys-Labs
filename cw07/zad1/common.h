#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "types.h"

// common function to create needed ops, all workers need it
struct sembuf *create_ops(int index, int op);

#endif