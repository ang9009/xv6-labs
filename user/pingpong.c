#include "kernel/types.h"
#include <stdint.h>
#include <stddef.h>
#include "user.h"

int main(int argc, char *argv[]) {
    int p1[2];
    int p2[2];

    if (pipe(p1) == -1 || pipe(p2) == -1) {
        fprintf(2, "Pipe failed");
        exit(1);
    }   
    int pid = fork();
    
    uint8_t byte = 0;
    if (pid > 0) {
        close(p1[0]);
        close(p2[1]);

        write(p1[1], &byte, 1);
        close(p1[1]);

        read(p2[0], NULL, 1);
        printf("%d: received pong\n", pid);
        close(p2[0]);
    } else if (pid == 0) {
        close(p1[1]);
        close(p2[0]);

        read(p1[0], NULL, 1);
        printf("%d: received ping\n", pid);
        close(p1[0]);

        write(p2[1], &byte, 1);
        close(p2[1]);
        exit(0);    
    } else {
        fprintf(2, "Fork failed");
        exit(1);
    }

  exit(0);
}