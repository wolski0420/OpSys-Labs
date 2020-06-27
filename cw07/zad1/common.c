#include "common.h"

// common function to create needed ops, all workers need it
struct sembuf *create_ops(int index, int op){
    // lock or unlock memory
    struct sembuf *lu_memory = calloc(1,sizeof(struct sembuf));
    lu_memory->sem_num = INDEX_MODIFY;
    lu_memory->sem_op = op;
    lu_memory->sem_flg = 0;
    
    // decrement or increment semaphore
    struct sembuf *di_sem = calloc(1,sizeof(struct sembuf));
    di_sem->sem_num = index;
    di_sem->sem_op = op;
    di_sem->sem_flg = 0;

    // creating ops
    struct sembuf *ops = calloc(2,sizeof(struct sembuf));
    ops[0] = *lu_memory;
    ops[1] = *di_sem;

    return ops;
}
