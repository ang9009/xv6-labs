#include "kernel/types.h"
#include "user.h"
#include "kernel/param.h"
#include "stddef.h"
#include <stdbool.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("xargs: usage: xargs <cmd> <args>\n");
    exit(1);
  }

  char* args[MAXARG];
  for (int i = 0; i < argc - 1; i++) {
    args[i] = argv[i + 1];  // Skip "xargs"
  }

  char char_buf;
  char arg_buf[2048];
  char* last_arg_p = arg_buf;  // Pointer to last argument
  int arg_off = 0;             // Offset for argument buffer
  int args_idx = argc - 1;     // Index for args array
  int blanks = 0;
  while (read(0, &char_buf, 1) > 0) {
    if (arg_off >= sizeof(arg_buf)) {
      fprintf(2, "xargs: too many arguments");
      exit(1);
    }

    if (char_buf == ' ') {
      blanks++;
    } else if (char_buf == '\n') {
      // Add null terminator to current argument, add to args
      arg_buf[arg_off++] = '\0';
      args[args_idx++] = last_arg_p;
      last_arg_p = &arg_buf[arg_off];  // Update for next argument

      args[args_idx] = NULL;  // Add null terminator
      args_idx = argc - 1;    // Reset args array index

      int pid = fork();
      if (pid == 0) {
        char* cmd = argv[1];
        exec(cmd, args);
        fprintf(2, "xargs: exec failed\n");
        exit(1);
      } else {
        wait(0);
      }
    } else {
      if (blanks) {
        arg_buf[arg_off++] = '\0';
        args[args_idx++] = last_arg_p;

        last_arg_p = &arg_buf[arg_off];
        blanks = 0;
      }

      arg_buf[arg_off++] = char_buf;
    }
  }

  exit(0);
}