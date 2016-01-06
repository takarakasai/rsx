
#include "serial/hr_unixio.h"

#include "dp_err.h"

errno_t hr_unixio_init (hr_unixio *io) {
  EVALUE(NULL, io);

  return EOK;
}

errno_t hr_open (hr_unixio *io, const char *path) {
  EVALUE(NULL, io);
  EVALUE(NULL, path);

  fprintf(stdout, "file open : %s\n", path);
  //io->fd = 0xFF;

  return EOK;
}

errno_t hr_write (hr_unixio *io, void* data, size_t size, size_t *send_size) {
  EVALUE(NULL, io);
  //EVALUE(0, io->fd);
  EVALUE(NULL, data);

  uint8_t *d = (uint8_t*)data;
  //fprintf(stdout, "fd(%d) size(%zd) :", io->fd, size);
  fprintf(stdout, "fd(--) size(%zd) :", size);
  for (size_t i = 0; i < size; i++) {
    fprintf(stdout, " %02x", d[i]);
  }
  fprintf(stdout, "\n");

  return *send_size == -1 ? errno : EOK;
}

errno_t hr_read (hr_unixio *io, void* data, size_t size, size_t *read_size) {
  EVALUE(NULL, io);
  //EVALUE(0, io->fd);
  EVALUE(NULL, data);

  uint8_t *d = (uint8_t*)data;
  //fprintf(stdout, "fd(%d) size(%zd) :", fd, size);
  fprintf(stdout, "fd(--) size(%zd) :", size);
  for (size_t i = 0; i < size; i++) {
    fprintf(stdout, " %02x", d[i]);
  }
  fprintf(stdout, "\n");

  return EOK;
}

errno_t hr_close (hr_unixio *io) {
  EVALUE(NULL, io);
  //EVALUE(0, io->fd);

  //fprintf(stdout, "file close : %d\n", fd);
  fprintf(stdout, "file close : --\n");

  return EOK;
}

