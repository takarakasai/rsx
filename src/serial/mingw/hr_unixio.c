
#include "serial/hr_unixio.h"

#include "dp_err.h"

/* for error print */
//#include <stdio.h>

/* for open */
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

/* for close */
//#include <unistd.h>

#include <windows.h>

errno_t hr_unixio_init (hr_unixio *io) {
  EVALUE(NULL, io);

  io->hd = INVALID_HANDLE_VALUE;

  return EOK;
}

errno_t hr_open (hr_unixio *io, const char *path) {
  EVALUE(NULL, io);
  EVALUE(NULL, path);

  printf("file open : %s : %s\n", path, __FILE__);

  //LPCSTR portname = "COM6";
  LPCSTR portname = path;
  DWORD  direction = GENERIC_READ | GENERIC_WRITE;
  io->hd = CreateFile(
          portname, direction,
          0/*share mode*/, NULL/*security attributes*/,
          OPEN_EXISTING/*create diposition*/,
          FILE_ATTRIBUTE_NORMAL/*flag attributes*/, NULL/*template file*/);

  if (io->hd == INVALID_HANDLE_VALUE) {
    int eno = GetLastError();
    printf("file can not open : %d\n", eno);
    return eno;
  }

  return EOK;
}

errno_t hr_write (hr_unixio *io, void* data, size_t size, size_t *send_size) {
  EVALUE(NULL, io);
  EVALUE(INVALID_HANDLE_VALUE, io->hd);
  EVALUE(NULL, data);
  EVALUE(NULL, send_size);

  DWORD dwBytesRead = 0;
  if(!WriteFile(io->hd, (LPCVOID)data, (DWORD)size, &dwBytesRead, NULL /*Over Lapped*/)){
    //printLastError();
    int eno = GetLastError();
    printf("file can not write: %d\n", eno);
    return eno;
  }

  *send_size = (size_t)dwBytesRead;

  return EOK;
}

errno_t hr_read (hr_unixio *io, void* data, size_t size, size_t *read_size) {
  EVALUE(NULL, io);
  EVALUE(INVALID_HANDLE_VALUE, io->hd);
  EVALUE(NULL, data);
  EVALUE(NULL, read_size);

  DWORD dwBytesRead = 0;
  if(!ReadFile(io->hd, (LPVOID)data, (DWORD)size, &dwBytesRead, NULL /*Over Lapped*/)){
    int eno = GetLastError();
    printf("file can not read: %d\n", eno);
    return eno;
  }

  *read_size = (size_t)dwBytesRead;

  return EOK;
}

errno_t hr_close (hr_unixio *io) {
  EVALUE(NULL, io);
  EVALUE(INVALID_HANDLE_VALUE, io->hd);

  CloseHandle(io->hd);
  io->hd = INVALID_HANDLE_VALUE;

  return EOK;
}

