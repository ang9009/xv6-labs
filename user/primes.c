#include "kernel/types.h"
#include "user.h"
#include <stdint.h>

void child_helper(int read_fd, int write_fd) __attribute__((noreturn));


int main(int argc,  char *argv[]) {
    int p[2];
    if (pipe(p) == -1) {
        fprintf(2, "Pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid > 0) {
        for (uint32_t i = 2; i <= 280; i++) {
            uint32_t j = i;
            write(p[1], &j, sizeof(uint32_t));
        }
        close(p[1]);
        wait(&pid);
    } else {
        child_helper(p[0], p[1]);
    }

    exit(0);
}

void child_helper(int read_fd, int write_fd) {
    close(write_fd); 
    uint32_t divisor;
    read(read_fd, &divisor, sizeof(uint32_t));
    printf("%d\n", divisor);

    uint32_t nums[280];
    int j = 0;
    uint32_t next;
    while (read(read_fd, &next, sizeof(uint32_t)) != 0) {
        if (next % divisor != 0) {
            nums[j++] = next;
        }
    }
    close(read_fd);

    if (j == 0) { // No more primes, complete
        exit(0);
    }

    int p2[2];
    if (pipe(p2) == -1) {
        fprintf(2, "Pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if (pid > 0) {
        close(p2[0]);
        for (uint32_t i = 0; i <= j; i++) {
            uint32_t val = nums[i];
            write(p2[1], &val, sizeof(uint32_t));
        }
        close(p2[1]);
        wait(&pid);
    } else {
        child_helper(p2[0], p2[1]);
    }

    exit(0);
}