#include "rsx/serial/hr_unixio.h"

#include "rsx/rsx_err.h"

#include <assert.h>

/* for error print */
#include <stdio.h>

/* for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* for close */
#include <unistd.h>

#include "rsx/rsx_common.h"

errno_t _open (const char *path, int *fd) {
  EVALUE(NULL, path);
  EVALUE(NULL, fd);

  RSX_DEBUG_PRINT("file open : %s\n", path);
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

  int res = read(fd, data, size);

  if (res == -1) {
    return errno;
  }

  *read_size = res;

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

