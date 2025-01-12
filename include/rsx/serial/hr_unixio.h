
#ifndef HR_UNIXIO_H
#define HR_UNIXIO_H

#include "rsx/rsx_type.h"

errno_t _open (const char *path, int *fd);
errno_t _close (int fd);

errno_t _write (int fd, void* data, size_t size, size_t *send_size);
errno_t _read (int fd, void* data, size_t size, size_t *read_size);

#endif

