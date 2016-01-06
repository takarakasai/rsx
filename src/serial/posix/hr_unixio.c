
#include "serial/hr_unixio.h"

#include "dp_err.h"

/* for error print */
#include <stdio.h>

/* for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* for close */
#include <unistd.h>

errno_t hr_unixio_init (hr_unixio *io) {
  EVALUE(NULL, io);

  io->fd = -1;

  return EOK;
}

errno_t hr_open (hr_unixio *io, const char *path) {
  EVALUE(NULL, io);
  EVALUE(NULL, path);

  printf("file open : %s\n", path);
  io->fd = open(path, O_RDWR | O_NOCTTY);
  if (io->fd == -1) {
    printf("file can not open : %d\n", errno);
    return errno;
  }

  return EOK;
}

errno_t hr_write (hr_unixio *io, void* data, size_t size, size_t *send_size) {
  EVALUE(NULL, io);
  EVALUE(0, io->fd);
  EVALUE(NULL, data);

  *send_size = write(io->fd, data, size);

  return *send_size == -1 ? errno : EOK;
}

errno_t hr_read (hr_unixio *io, void* data, size_t size, size_t *read_size) {
  EVALUE(NULL, io);
  EVALUE(0, io->fd);
  EVALUE(NULL, data);

  *read_size = read(io->fd, data, size);

  return EOK;
}

errno_t hr_close (hr_unixio *io) {
  EVALUE(NULL, io);
  EVALUE(0, io->fd);

  if (close(io->fd) == -1) {
    printf("file can not close : %d\n", errno);
    return errno;
  }
  io->fd = -1;

  return EOK;
}

