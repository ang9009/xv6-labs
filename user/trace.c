#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: trace <mask> <program-call>\n");
  }

  uint64 mask = atoi(argv[1]);
  trace(mask);
  char* file = argv[2];
  char** args = &argv[3];

  int pid = fork();
  if (pid == 0) {
    exec(file, args);
    fprintf(2, "exec failed\n");
    exit(1);
  } else {
    wait(0);
  }

  exit(0);
}