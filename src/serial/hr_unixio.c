
#include "serial/hr_unixio.h"

#include "rsx_err.h"

/* for error print */
#include <stdio.h>

/* for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* for close */
#include <unistd.h>

errno_t _open (const char *path, int *fd) {
  EVALUE(NULL, path);
  EVALUE(NULL, fd);

  printf("file open : %s\n", path);
  *fd = open(path, O_RDWR | O_NOCTTY);
  if (*fd == -1) {
    printf("file can not open : %d\n", errno);
    return errno;
  }

  return EOK;
}

errno_t _write (int fd, void* data, size_t size, size_t *send_size) {
  EVALUE(0, fd);
  EVALUE(NULL, data);

  *send_size = write(fd, data, size);

  return EOK;
}

errno_t _read (int fd, void* data, size_t size, size_t *read_size) {
  EVALUE(0, fd);
  EVALUE(NULL, data);

  *read_size = read(fd, data, size);

  return EOK;
}

errno_t _close (int fd) {
  EVALUE(0, fd);

  if (close(fd) == -1) {
    printf("file can not close : %d\n", errno);
    return errno;
  }

  return EOK;
}

