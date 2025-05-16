#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd = shm_open("/execmem");
    if (fd < 0) {
        printf("[EXEC] shm_open failed\n");
        exit();
    }

    int *p;
    if (shm_map(fd, (void **)&p, O_RDWR) < 0) {
        printf("[EXEC] shm_map failed\n");
        exit();
    }

    printf("[EXEC] p[0] = %d (expected 1234 or 1111?)\n", p[0]);
    printf("[EXEC] p[1] = %d (expected 5678)\n", p[1]);

    shm_close(fd);
    exit();
}
