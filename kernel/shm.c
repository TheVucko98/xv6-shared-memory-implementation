#include "shm.h"
#include "proc.h"

struct sharedTable shmTable;

void
shminit(void){
    initlock(&shmTable.lock, "sharedTable");
    e9printf("Pozvan shminit");
}

void
addObjectToProc(struct proc* pr, int index){
    e9printf("Open Proces pid = %d \n", pr->pid);
    for(int i = 0; i < MAX_SHARED_PER_PROCm;i++){
        if(pr->arrayOfObj[i] == 0){
            
            pr->arrayOfObj[i] = &shmTable.objects[index];
            pr->numOfObj++;
            return;
        }
    }

    
}


int
shmOpen(char *name, struct proc* pr){
    e9printf("shmOpen %d\n",pr->numOfObj);
    if(pr->numOfObj > 16){
        return -1;
    }
    if(strlen(name) > 19){
        name[19] = 0;
    }
    e9printf("Open ()  | %s | : ", name );
    //dal ima vec jedan objekat
    if(pr->numOfObj != 0){
        for(int i = 0; i < MAX_SHARED_PER_PROCm; i++){
            e9printf("pr-> %d   =  %p , name = %s \n", i, pr->arrayOfObj[i], pr->arrayOfObj[i]->name);
            if(pr->arrayOfObj[i] != 0  && strncmp(name,pr->arrayOfObj[i]->name,sizeof(pr->arrayOfObj[i]->name)) == 0){
                e9printf("Ima vec proces %d\n",pr->arrayOfObj[i]->indexInArray);
                return pr->arrayOfObj[i]->indexInArray;
            }
        }
    }
    if(pr->numOfObj >= 16){
        return -1;
    }
    acquire(&shmTable.lock);
    //dal je vec open
    for(int i = 0; i < MAX_OBJm; i++){
        struct sharedObj* pObj = &shmTable.objects[i];
        if(pObj->isOpen == 1 && strncmp(name,pObj->name,sizeof(name)) == 0){
            //kad se ne poklapaju indexi
            if(pObj->indexInArray != i){
                e9printf("shm.c ->   sysShmOpen -> indexInArray != i !!!!\n");
                release(&shmTable.lock);
                return -1;
            }

            pObj->refCnt++;
            e9printf("vec alociran fd = %d\n", pObj->indexInArray);
            addObjectToProc(pr,i);
            release(&shmTable.lock);
            return i;
        }
    }
    if(shmTable.nmbrSlotsTaken >= 64){
        e9printf("Nema dovoljno mesta za zauzimanje\n");
        release(&shmTable.lock);
        return -1;
    }

    for(int i =0; i < MAX_OBJm; i++){
        struct sharedObj* pObj = &shmTable.objects[i];
        if(pObj->isOpen == 0){
      
            strncpy(pObj->name,name,19);
            pObj->name[19] = '\0';
            pObj->isOpen = 1;
            pObj->indexInArray = i;
            e9printf("alociran fd = %d\n", pObj->indexInArray);

            pObj->refCnt++;
            shmTable.nmbrSlotsTaken++;
            addObjectToProc(pr,i);
            release(&shmTable.lock);
            return i;
        }
    }

    return -1;

}