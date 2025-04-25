#include "user.h"



void testOpen(){
    // Test vise puta isto ime
    int x = shm_open("aca");
    printf("id = %d\n",x);
    x = shm_open("aca");
     printf("id = %d\n",x);
     x = shm_open("aca");
     printf("id = %d\n",x);
     // Test razlicito ime
    x = shm_open("baca");
    printf("id = %d\n",x);
    x = shm_open("MilosFacaASDASDASDASDASbaca");
    printf("id = %d\n",x);

    // Test za MAX obj = 16, vec gore neki zauzeti
    for (int i = 0; i < 17; i++)
    {
        printf("i = %d   :   ", i);
        char name[] = {'a' + i % 10, 'a' + (i/10 + 1) % 10, '\0' };
        x = shm_open(name);
        printf(" name :  %s,  id  = %d\n",name ,x);
    }

    sleep(500);
    // DETE
    if(fork()==0){
        printf("\n Dete : \n");
        x = shm_open("baca");
        printf("id = %d\n",x);
        x = shm_open("MilosFacaASDASDASDASDASbaca");
        printf("id = %d\n",x);
        for (int i = 0; i < 15; i++)
        {
            printf("i = %d   :   ", i);
            char name[] = {'a' + i % 10, 'a' + (i/10 + 1) % 10, '\0' };
            x = shm_open(name);
        printf(" name :  %s,  id  = %d\n",name ,x);
        }
        exit();
    }
    wait();
}

int main(int argc, char *argv[]){
    
    testOpen();


  
    exit();
}