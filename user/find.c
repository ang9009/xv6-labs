#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

// Returns the file at the end of the path
char* get_file(char* path) {
  static char buf[DIRSIZ + 1];
  char* p = path + strlen(path);

  while (*p != '/' && p > path) {  // Find first slash
    p--;
  }
  p++;

  if (strlen(p) >= DIRSIZ) {
    return p;
  }
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

// Attempts to find a given file by recursing into directories of a given
// directory
void find(const char* file, char* dir_path) {
  int fd;
  struct stat st;
  if ((fd = open(dir_path, O_RDONLY)) < 0) {
    fprintf(2, "find: failed to open open %s\n", dir_path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: failed to get file stat for %s\n", dir_path);
    close(fd);
    return;
  }

  if (st.type == T_FILE || st.type == T_DEVICE) {
    fprintf(2, "find: somehow navigated into file");
    close(fd);
    return;
  }

  char buf[512], *p;
  struct dirent de;
  if (strlen(dir_path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "find: path is too long\n");
    return;
  }
  memmove(buf, dir_path, strlen(dir_path));
  // Advance pointer to end of string, add a slash, put pointer behind
  // slash
  p = buf + strlen(dir_path);
  *p = '/';
  p++;

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
        strcmp(de.name, "..") == 0) {
      continue;
    }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    if (st.type == T_FILE || st.type == T_DEVICE) {
      if (strcmp(file, get_file(buf)) == 0) {
        printf("%s\n", buf);
      }
    } else if (st.type == T_DIR) {
      find(file, buf);
    }
  }

  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: find <dir> <file>\n");
    exit(0);
  }

  char* dir = argv[1];
  char* file = argv[2];
  find(file, dir);
  exit(0);
}
