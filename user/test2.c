#include "user.h"

int main() {
    if (fork() == 0) {
        // Proces 1
        printf("\n pid = %d \n", getpid());
        for (int i = 0; i < 40; i++) {
            char name[10] = {'a', 'c', i/10 + 'a', i %10 + 'a', 2+'a', '\0'};
               printf("name = %s, i = %d \n", name, i);
            int fd = shm_open(name);
            if (fd == -1 && i == 16) {
                printf("Max  objects test PASSED (failed at 17th)\n");
                break;
            } else if (fd == -1) {
                printf("Max  objects test FAILED (early error)\n");
                break;
            } else {
                printf("fd = %d\n", fd);
            }

        }
        sleep(1000);
        exit();
    }
   sleep(300);

    if (fork() == 0) {
        // Proces 2
       printf("\n pid = %d \n", getpid());
        for (int i = 0; i < 40; i++) {
            char name[10] = {'a', 'c', i/10 + 'a', i %10 + 'a', 2+'a', '\0'};
            printf("name = %s, i = %d \n", name, i);
            int fd = shm_open(name);
            if (fd == -1 && i == 16) {
                printf("Max  objects test PASSED (failed at 17th)\n");
                break;
            } else if (fd == -1) {
                printf("Max  objects test FAILED (early error)\n");
                break;
            } else {
                printf("fd = %d\n", fd);
            }
                           sleep(50);


        }
        sleep(100);
        exit();
    }
     wait();
   if (fork() == 0) {
        // Proces 3
        printf("\n pid = %d \n", getpid());
        for (int i = 0; i < 16; i++) {
            char name[10] = {'a', 'c', i/10 + 'a', i %10 + 'a', getpid() +'a', '\0'};
            printf("name = %s, i = %d \n", name, i);
            int fd = shm_open(name);
            if (fd == -1) {
                printf("Max global objects test PASSED (failed at 64th), %d\n", i);
            } else if (fd == -1) {
                printf("Max global objects test FAILED (early error)\n");
                 sleep(50);
                break;
            } else {
                printf("fd = %d\n", fd);
            }
  
        }
        sleep(100);
        exit();
    }
    wait();

    if (fork() == 0) {
        // Proces 4
        printf("\n pid = %d \n", getpid());
        for (int i = 0; i < 16; i++) {
            char name[10] = {'a', 'c', i/10 + 'a', i %10 + 'a', getpid() +'a', '\0'};
            printf("name = %s, i = %d \n", name, i);
            int fd = shm_open(name);
            if (fd == -1) {
                printf("Max global objects test PASSED (failed at 64th), %d\n", i);
            } else if (fd == -1) {
                printf("Max global objects test FAILED (early error)\n");
                 sleep(50);
                break;
            } else {
                printf("fd = %d\n", fd);
            }
 
        }
        sleep(100);
        exit();
    }
    wait();
    sleep(10);
        if (fork() == 0) {
        // Proces 4
        printf("\n pid = %d \n", getpid());
        for (int i = 0; i < 50; i++) {
            char name[10] = {'a', 'c', i/10 + 'a', i %10 + 'a', getpid()%10 +'a', (getpid()/10+1)%10 +'a', '\0'};
            printf("name = %s, i = %d \n", name, i);
            int fd = shm_open(name);
            if (fd == -1) {
                printf("Max global objects test PASSED (failed at 64th)\n");
                if(i == 3){
                    sleep(600);
                }
                
            }else {
                printf("fd = %d\n", fd);
            }
          
        }
        sleep(100);
        exit();
    }
    sleep(1000);
    for (int i = 0; i < 5; i++) {
        wait(); 
    }
    printf("END!\n");
    exit();
}