#include "shm.h"
#include "proc.h"

struct sharedTable shmTable;

void
shminit(void){
    initlock(&shmTable.lock, "sharedTable");
    e9printf("Pozvan shminit\n");
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
    e9printf("\n shmOpen %d\n",pr->numOfObj);
    if(pr->numOfObj > 16){
        return -1;
    }
    if(strlen(name) > 19){
        name[19] = 0;
    }
    e9printf("Open ()  | %s | : ", name );
    //dal ima vec  objekat
    if(pr->numOfObj != 0){
        for(int i = 0; i < MAX_SHARED_PER_PROCm; i++){
            e9printf("pr-> %d   =  %p , name = %s \n", i, pr->arrayOfObj[i], pr->arrayOfObj[i]->name);
            if(pr->arrayOfObj[i] != 0  && strncmp(name,pr->arrayOfObj[i]->name,sizeof(pr->arrayOfObj[i]->name)) == 0){
                e9printf("Proces ima vec objekat %d\n",pr->arrayOfObj[i]->indexInArray);
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
        if(pObj->isOpen == 1 && strncmp(name,pObj->name,sizeof(pObj->name)) == 0){
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

int
shmTrunc(int shm_od, int size, struct proc* pr){
    e9printf("\n trunc () \n !");
    if(size <= 0 || shm_od < 0 || shm_od >= MAX_OBJm) {
        return -1;
    }

    acquire(&shmTable.lock);
    //dal postoji vec objekat koji pripada procesu
    int exist = 0;
    for(int i = 0; i < MAX_SHARED_PER_PROCm; i++) {
        if(pr->arrayOfObj[i] && 
           pr->arrayOfObj[i]->indexInArray == shm_od) {
            exist = 1;
                e9printf("Nadjen objekat , fd = %d !\n", i);

            break;
        }
    }
    if(!exist) {
        release(&shmTable.lock);
        return -1;
    }

    struct sharedObj* pObj = &shmTable.objects[shm_od];
    
    if(pObj->isOpen == 0 || pObj->isAllocated){
        release(&shmTable.lock);
        e9printf("nije otvoren ili je alociran !\n");
        return -1;
    }


    int n = (size + PGSIZE - 1) / PGSIZE;;
    e9printf("brojStranica = %d  ", n);
    if(n > MAX_PAGESm || n <= 0)
    return -1;

    pObj->nmbrPages = n;

    int flagWentBad =0, indexDisaster = 0;
    for(int i = 0; i < n; i++){
        if((pObj->pages[i] = kalloc()) == 0){
            flagWentBad=1;
            indexDisaster = i;
            break;
        }
        memset(pObj->pages[i], 0, PGSIZE);
    }

    if(flagWentBad){
        for(int i = 0; i < indexDisaster;i++ ){
            kfree(pObj->pages[i]);
            pObj->pages[i] = 0;
        }
        pObj->nmbrPages = 0;
        release(&shmTable.lock);
        return -1;
    }


    pObj->isAllocated = 1;


    release(&shmTable.lock);
    return n*PGSIZE;
}


// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
	pde_t *pde;
	pte_t *pgtab;

	pde = &pgdir[PDX(va)];
	if(*pde & PTE_P){
		pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
	} else {
		if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
			return 0;
		// Make sure all those PTE_P bits are zero.
		memset(pgtab, 0, PGSIZE);
		// The permissions here are overly generous, but they can
		// be further restricted by the permissions in the page table
		// entries, if necessary.
		*pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
	}
	return &pgtab[PTX(va)];
}

// Samo 1 stranica se zauzima, posto kalloc nije njih uzimao za redom
static int
mappage(pde_t *pgdir, void *va, uint pa, int perm)
{
    // Nalazimo entry i alociramo ako nije PT alocirana
    pte_t *pte = walkpgdir(pgdir, va, 1); 

    if (*pte & PTE_P) {
        panic("remap"); 
    }

    *pte = pa | perm | PTE_P;  
    return 0;
}


void* findNextVa(struct proc* p,int pageBr){
    void* nextAddr = p->nextFreeVA + pageBr*PGSIZE;
    // Jer zelim da tik do njega alocira i tek sledeci put ce javiti gresku
    // Jer ce currAdr = KerBASE + bilo sta dace > KERNBASE
    if(nextAddr > (void*)KERNBASE){
        return 0;
    }
    void *currAddr = p->nextFreeVA;
    p->nextFreeVA = nextAddr;


    return currAddr;

}

int ShmMap(int sd, void **va, int flags, struct proc* pr) {
    e9printf("Map () :  ");
    
    // Da li je Map vec zvan
    for(int i = 0; i < MAX_SHARED_PER_PROCm; i++) {
        if(pr->arrayOfObj[i] && 
           pr->arrayOfObj[i]->indexInArray == sd && 
           pr->mapObj[i] != -1) {
            if(pr->vaObj[i] != 0) {
                *va = pr->vaObj[i]; 
                e9printf("  Objekat je vec mapiran !\n");
                return 0;
            }
            break;
        }
    }

    // Da li proces ima pristup ovom objektu, tj jel zvao open
    int found = 0;
    for(int i = 0; i < MAX_SHARED_PER_PROCm; i++) {
        if(pr->arrayOfObj[i] && pr->arrayOfObj[i]->indexInArray == sd) {
            found = 1;
            break;
        }
    }
    
    if(!found) {
        e9printf("Neuspesno mapiranje (): Nije zvat open za proces za objekat\n");
        return -1;
    }

    acquire(&shmTable.lock);
    struct sharedObj* pObj = &shmTable.objects[sd];
    
    // Da li je open i trunc zvan
    if(pObj->isOpen == 0 || pObj->isAllocated == 0) {
        e9printf("Neuspesno mapiranje (): Open ili Trunc nije uradjen za objekat !\n");
        release(&shmTable.lock);
        return -1;
    }

    // Nadji gde mapirati za usera
    *va = findNextVa(pr, pObj->nmbrPages);
    if(*va == 0) {
        e9printf("Neuspesno mapiranje (): Nema vise memorije, dosli do KERNBASE !\n");
        release(&shmTable.lock);
        return -1;
    }

    // Map pages
    for(int i = 0; i < pObj->nmbrPages; i++) {
        void *userVa = *va + i * PGSIZE;
        uint pa = V2P(pObj->pages[i]);
        e9printf("\n %d. stranica: pa = %p, va = %p\n", i, pa, userVa);
        if(mappage(pr->pgdir, userVa, pa, flags) < 0) {
            // ciscenje mape ako nesto ode po zlu
            for(int j = 0; j < i; j++) {
                void *uv = *va + j * PGSIZE;
                pte_t *pte = walkpgdir(pr->pgdir, uv, 0);
                if(pte && (*pte & PTE_P)) {
                    *pte = 0;
                }
            }
            release(&shmTable.lock);
            return -1;
        }
    }

    // Update map info
    for(int i = 0; i < MAX_SHARED_PER_PROCm; i++) {
        if(pr->arrayOfObj[i] && pr->arrayOfObj[i]->indexInArray == sd) {
            pr->mapObj[i] = 1;
            pr->vaObj[i] = *va;
            pObj->flags = flags;
            break;
        }
    }


    release(&shmTable.lock);
    return 0;
}




static int unMapPage(pde_t *pgdir, void *va){
    // Nadji entry
    pte_t *pte = walkpgdir(pgdir, va, 0); 

    if (pte == 0 || !(*pte & PTE_P)) {
        return -1;  // Stranica nije mapirana
    }

    *pte = 0;
    
    return 0;
}


void unMaping(struct proc* pr,int ind){
    /// Odmapiranje bez free u kernelu
    int nmbrPages = shmTable.objects[pr->arrayOfObj[ind]->indexInArray].nmbrPages;
    for (int i = 0; i < nmbrPages; i++){   
        
            unMapPage(pr->pgdir,pr->vaObj[ind]+i*PGSIZE);
    }
    pr->mapObj[ind] = -1;
    pr->vaObj[ind] = 0;
}
void unOpening(struct proc* pr,int ind, struct sharedObj* pObj, int clear){
        pr->arrayOfObj[ind] = 0;
        pr->numOfObj--;
        pObj->refCnt--;
        if(clear){
            memset(pObj,0,sizeof(struct sharedObj));
        }
}
// Dodati provere ako trebaju za Dalije MAP ili TRUNC
int shmClose(int fd, struct proc* pr){
    e9printf("Close()  : ",fd);
    //
    int ind = -1;
    
    // Provera da li proces  ima ovaj obj
    for (int i = 0; i < MAX_SHARED_PER_PROCm; i++)
      {
          if(pr->arrayOfObj[i] && pr->arrayOfObj[i]->indexInArray == fd ){
              ind = i;
              break;
          }
      }
    if(ind == -1){
        e9printf("Close() proces nema ovaj objekat  fd = %d \n",fd);
        return -7;
    }
    

    acquire(&shmTable.lock);
    struct sharedObj* pObj = &shmTable.objects[fd];
    

    
    if(pObj->refCnt > 1){
        e9printf("fd = %d, rfcnt = %d \n", pObj->indexInArray,pObj->refCnt);
        
        if(pr->mapObj[ind] != -1){
        unMaping(pr,ind);
        }

        unOpening(pr,ind,pObj,0);
    }else if(pObj->refCnt <= 0){// Ne treba da se desi
        release(&shmTable.lock);
        e9printf("Close() :  \n  RfcNt < 0 !!!!    fd = %d \n",fd);
        return -5;
    }
    else if(pObj->refCnt == 1){/// Ako je poslednji
        e9printf("fd = %d, rfcnt = %d, ind = %d, pr->mapObj[ind] = %d \n", fd,pObj->refCnt,ind,pr->mapObj[ind] );

        if(pr->mapObj[ind] != -1){
            unMaping(pr,ind);
        }

        // Free pages
        if(pObj->isAllocated){
            for (int i = 0; i < pObj->nmbrPages; i++){   
                kfree(pObj->pages[i]);
            }
        }
        
        
        unOpening(pr,ind,pObj,1);
        // umanji broj zauzetih obj
        shmTable.nmbrSlotsTaken--;
        
    }
 
    release(&shmTable.lock);

    return 0;
}
