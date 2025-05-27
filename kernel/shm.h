#ifndef KERNEL_SHM_H
#define KERNEL_SHM_H

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"
#include "spinlock.h"
#include "memlayout.h"



#define SHMBASE (KERNBASE/2)
#define MAX_PAGESm  32
#define MAX_OBJm 64
#define MAX_SHARED_PER_PROCm 16

struct sharedObj{
    // open
	char name[20];
	int isOpen;
    int indexInArray;
    int refCnt;
	// trunc
    int nmbrPages; // alociram ceo broj stranica
	int isAllocated; // da li je alocirano
	char* pages[MAX_PAGESm];
};

struct sharedTable{
    struct sharedObj objects[MAX_OBJm];
    struct spinlock lock;   
    int nmbrSlotsTaken;
};

void
shminit(void);
int
shmOpen(char *name, struct proc* pr);

int
shmTrunc(int shm_od, int size, struct proc* pr);

int ShmMap(int sd, void **va, int flags, struct proc* pr);

int shmClose(int fd, struct proc* pr);

#endif
