#include "shm.h"
#include "proc.h"

struct sharedTable shmTable;

void
shminit(void){
    initlock(&shmTable.lock, "sharedTable");
    e9printf("Pozvan shminit");
}