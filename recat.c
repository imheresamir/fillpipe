/* Minimal reimplementation of the venerable Cat command */
/* https://stackoverflow.com/questions/3866468/using-system-calls-to-implement-the-unix-cat-command */

#include "recat.h"

int
main(int argc, char **argv) {
  int i;

  if (argc == 1) {
    if (cat_fd(STDIN_FILENO) != 0)
      goto error;
  } else {
    for (i = 1; i < argc; i++) {
      if (cat(argv[i]) != 0)
        goto error;
    }
  }
  return 0;

error:
  write(STDOUT_FILENO, "error\n", 6);
  return 1;
}
