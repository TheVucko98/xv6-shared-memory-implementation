#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd = shm_open("/execmem");
    if (fd < 0) {
        printf("shm_open failed\n");
        exit();
    }

    if (shm_trunc(fd, 4096) < 0) {
        printf("shm_trunc failed\n");
        exit();
    }

    int *p;
    if (shm_map(fd, (void **)&p, O_RDWR) < 0) {
        printf("shm_map failed\n");
        exit();
    }

    p[0] = 1234;
    p[1] = 5678;

    int pid1 = fork();
    if (pid1 == 0) {
        // child 1 - fork only
        p[0] = 1111;
        printf("[FORK 1] p[0] = %d (should be 1111)\n", p[0]);
        shm_close(fd);
        exit();
    }

    wait();

    int pid2 = fork();
    if (pid2 == 0) {
        // child 2 - fork and exec
        char *argv[] = { "shmtestexec", 0 };
        exec("/bin/shmtestexec", argv); // will open /execmem and print values
        printf("exec failed\n");
        exit();
    }

    wait();

    // Back in parent
    printf("[PARENT] p[0] = %d (should be 1111 from fork child)\n", p[0]);

    shm_close(fd);
    exit();
}
