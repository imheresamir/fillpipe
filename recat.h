/* Minimal reimplementation of the venerable Cat command */
/* https://stackoverflow.com/questions/3866468/using-system-calls-to-implement-the-unix-cat-command */

#include <fcntl.h>
#include <unistd.h>

static int
cat_fd(int fd) {
  char buf[4096];
  ssize_t nread;

  while ((nread = read(fd, buf, sizeof buf)) > 0) {
    ssize_t ntotalwritten = 0;
    while (ntotalwritten < nread) {
      ssize_t nwritten = write(STDOUT_FILENO, buf + ntotalwritten, nread - ntotalwritten);
      if (nwritten < 1)
        return -1;
      ntotalwritten += nwritten;
    }
  }

  return nread == 0 ? 0 : -1;
}

static int
cat_fdio(int fd_in, int fd_out) {
  char buf[4096];
  ssize_t nread;

  while ((nread = read(fd_in, buf, sizeof buf)) > 0) {
    ssize_t ntotalwritten = 0;
    while (ntotalwritten < nread) {
      ssize_t nwritten = write(fd_out, buf + ntotalwritten, nread - ntotalwritten);
      if (nwritten < 1)
        return -1;
      ntotalwritten += nwritten;
    }
    sleep(0.5);
  }

  return nread == 0 ? 0 : -1;
}

static int
cat(const char *fname) {
  int fd, success;

  if ((fd = open(fname, O_RDONLY)) == -1)
    return -1;

  success = cat_fd(fd);

  if (close(fd) != 0)
    return -1;

  return success;
}

static int
catio(const char *fname, int fd_out) {
  int fd_in, success;

  if ((fd_in = open(fname, O_RDONLY)) == -1)
    return -1;

  success = cat_fdio(fd_in, fd_out);

  if (close(fd_in) != 0)
    return -1;

  return success;
}
