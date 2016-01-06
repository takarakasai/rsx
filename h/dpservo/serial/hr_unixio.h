
#ifndef HR_UNIXIO_H
#define HR_UNIXIO_H

#include "dp_type.h"

#include "hr_unixio_impl.h"

errno_t hr_unixio_init (hr_unixio *io);

errno_t hr_open (hr_unixio *io, const char *path);
errno_t hr_close (hr_unixio *io);

errno_t hr_write (hr_unixio *io, void* data, size_t size, size_t *send_size);
errno_t hr_read (hr_unixio *io, void* data, size_t size, size_t *read_size);

#endif

