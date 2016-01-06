
#include "serial/hr_serial.h"

#include "dp_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

/* for usleep */
#include <unistd.h>

static const size_t timeout_usec = 200; // TODO: ICS --> 100 | RSX --> 200
static const size_t wait_usec    = 10;

errno_t hr_serial_init (hr_serial *ser) {
  EVALUE(NULL, ser);

  ECALL(hr_unixio_init(&(ser->io)));

  ser->baudrate = HR_B115200;

  ECALL(hr_tty_init(&(ser->tty)));

  return EOK;
}

errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, ser);
  EVALUE(NULL, dev);
  EVALUE(NULL, unit);

  ser->baudrate = baudrate;

#if defined(__unix)
  const char *header = "/dev/";
#elif  defined(__MINGW32__)
  const char *header = "\\\\.\\";
#endif
  size_t len_header = strlen(header);
  size_t len_dev  = strlen(dev);
  size_t len_unit = strlen(unit);

  char path[len_header + len_dev + len_unit + 1];
  memcpy(path                       , header, len_header);
  memcpy(path + len_header          , dev   , len_dev);
  memcpy(path + len_header + len_dev, unit  , len_unit);
  path[len_header + len_dev + len_unit] = '\0';

  ECALL(hr_open(&(ser->io), path));

  ECALL(hrtty_setup(&(ser->tty), &(ser->io), baudrate, parity));

  return EOK;
}

errno_t hr_serial_close (hr_serial *ser) {
  EVALUE(NULL, ser);

  // TODO:
  //if (ser->fd == 0) {
  //  return EOK;
  //}

  ECALL(hrtty_teardown(&(ser->tty), &(ser->io)));

  ECALL(hr_close(&(ser->io)));

  return EOK;
}

static errno_t serial_read (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);
  EVALUE(NULL, data);

  errno_t eno = -1; // TODO:

  const size_t max_count = (timeout_usec + wait_usec - 1) / wait_usec;
  size_t rcv_cnt = 0;
  size_t cnt;
  for (cnt = 0; cnt < max_count; cnt++) {
    usleep(wait_usec);

    size_t read_size;
    uint8_t rdata[size];
    ECALL(hr_read(&(ser->io), rdata, size, &read_size));

    // overflow TODO:
    for (size_t i = 0; i < read_size; i++) {
      ((uint8_t*)data)[rcv_cnt + i] = rdata[i];
    }
    rcv_cnt += read_size;


    if (size == rcv_cnt) {
      eno = EOK;
      break;
    }
  }

  //printf("           read latency : %zd / %zd %s (%zd/%zd)\n",
  //        cnt * wait_usec, max_count * wait_usec, size == rcv_cnt ? "OK" : "NG", rcv_cnt, size);

#if DEBUG
  printf("           size[%03zd] :", rcv_cnt);
  for (size_t i = 0; i < rcv_cnt; i++) {
    printf(" %02x", ((uint8_t*)data)[i]);
  }
  printf("\n");
#endif

  return eno;
}

errno_t hr_serial_read (hr_serial *ser, void* data, size_t size) {

#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

  ECALL2(serial_read(ser, data, size), false);

#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf("           read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif

  return EOK;
}

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);
  EVALUE(NULL, data);

  size_t send_size;
  ECALL(hr_write(&(ser->io), data, size, &send_size));

  if (size != send_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, send_size, size);
    return -1;
  }

#if defined(HR_SERIAL_AUTO_READ_ECHO_DATA)
#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

  const size_t max_count = (timeout_usec + wait_usec - 1) / wait_usec;
  size_t cnt;
  for (cnt = 0; cnt < max_count; cnt++) {
    usleep(wait_usec);
    size_t recv_size;
    uint8_t rdata[send_size];
    ECALL(hr_read(&(ser->io), rdata, send_size, &recv_size));

    if (recv_size != send_size) {
      continue;
    }

    if (memcmp(rdata, data, recv_size) == 0) {
      break;
    }
  }
#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  //printf(" auto echo read latency : %zd / %zd\n", cnt * wait_usec, max_count * wait_usec);
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf(" auto echo read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif
#endif

  return EOK;
}

