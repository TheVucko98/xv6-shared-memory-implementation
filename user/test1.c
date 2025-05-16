#include "user.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

void sysPoziv(char** args){
    if(fork() == 0) { 
        char path[100];  

        strcpy(path, "/bin/");
        memmove(path + strlen(path), args[0], strlen(args[0]) + 1); 
        printf("\nPozivam : \n", path);
        for (int i = 0; args[i] != 0; i++) {
            printf("  %s", args[i]);  
        }
        printf("\n");
        sleep(300);
        //
        exec(path, args); 
        // 
        printf("exec failed: %s\n", path);
        exit();  
    }
    wait(); 
}

int main(int argc, char *argv[]) {  
    shm_open("a");
    shm_open("b");
    shm_open("c");
    int od = shm_open("milos!");
    printf("od = %d\n",od);
    int sz = shm_trunc(od,143360);
    sz = shm_trunc(od,20000);
    printf("sz = %d\n",sz);
    char* va;
    shm_map(od,&va,O_RDWR);
    char arr[] = "aca baca faca caca!\n";
    int br = sizeof(arr)/sizeof(arr[0])-1;
    int i;
    for ( i = 0; i + br< sz; )
    {
        strncpy(va+i, arr,br);
        i += br;
    }
    printf("br = %d, i = %d\n",br, i);
    va[sz - 1] = '\0';
    
    char *argsd[] = {"test1",  0}; 
    sysPoziv(argsd);

    sleep(300);
    printf("string text  = %s, sz = %d \n",va,strlen(va));

    shm_close(od);
    // int fd1 = shm_open("aca");
    // int fd = shm_open("aca");
    // printf("fd0 = %d\n", fd);
    // printf("fd1 = %d\n", fd1);
    // shm_trunc(fd,131072);
    // char * va;
    // shm_map(fd,&va,O_RDWR);


    // va[13107] = 'l';
    // printf("va = %p,  ", va);
    // printf("va[0] = %c\n", va[13107]);
    
    
    // int fd2 = shm_open("baca");
    // printf("fd2 = %d\n", fd2);
    // shm_trunc(fd2,23);
    // char * va2;
    // shm_map(fd2,&va2,O_RDWR);
    // va2[0]= 'f';
    // printf("va2 = %p,  ", va2);
    // printf("va2[0] = %c\n", va2[0]);
     
    // shm_close(fd2);

    // sysPoziv(argsd);
    // sysPoziv(argsd);
    // sysPoziv(argsd);
    // sysPoziv(argsd);
    // printf("va[13107] = %c\n", va[13107]);    
    // //printf("va2[0] = %c\n", va2[0]);
    
  
    
   
    // printf("dosao ovde !\n");
    



    // char *args1[] = {"cat", "milos.aca", 0};  
    // sysPoziv(args1);

    // char *args2[] = {"setkey", "5", 0};  
    // sysPoziv(args2);

    // char *args3[] = {"encr", "milos.aca", 0};  
    // sysPoziv(args3);

    // char *args4[] = {"cat", "milos.aca", 0};  
    // sysPoziv(args4);
  
    exit();
}
