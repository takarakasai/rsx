
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx.h"
#include "rsx_io.h"

#if 0
#include <stdio.h>
#include <errno.h>

/* for open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* for close */
#include <unistd.h>

#include <termios.h>
#include <string.h>

#include <stdint.h>

typedef int errno_t;

#define EVALUE(value, variable) \
  if ((value) == (variable)) {      \
    return EINVAL;              \
  }

#define ELTGT(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue < variable)) {  \
    printf("%d %d %d\n", (int)lvalue, (int)hvalue, (int)variable);  \
    return EINVAL;                                   \
  }

#define ELTGE(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue <= variable)) { \
    return EINVAL;                                   \
  }

#define ELEGT(lvalue, hvalue, variable)              \
  if ((variable <= lvalue) || (hvalue < variable)) { \
    return EINVAL;                                   \
  }

#define EOK 0

#define ECALL(function)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {        \
      return eno;           \
    }                       \
  } while(0)


/*
 * @length  max length of data payload
 * @count   max num of servo
 */
#define RSX_LPKT_DECL(name, count, length) \
  uint8_t name ## _data[count][length];      \
  rsx_vload name ## _vload[count];/*TODO:*/\
  rsx_pkt name;

#define RSX_LPKT_INIT(name) \
  do {                        \
    rsx_pkt_init(&name,     \
      sizeof(name ## _data[0]) / sizeof(name ## _data[0][0]), \
      sizeof(name ## _vload) / sizeof(name ## _vload[0]),/*TODO:*/\
      (uint8_t*)(name ## _data));                             \
    rsx_lpkt_init(&name);                                   \
  } while(0)

#define RSX_SPKT_DECL(name, length) \
  uint8_t name ## _data[length];    \
  rsx_pkt name;

#define RSX_SPKT_INIT(name) \
  do {                        \
    rsx_pkt_init(&name,     \
      sizeof(name ## _data) / sizeof(name ## _data[0]), \
      1,                                                \
      (name ## _data));                                 \
    rsx_lpkt_init(&name);                  /*TODO:*/  \
  } while(0)

#define RSX_PKT_GETLENGTH(pkt)          ((pkt).length)
#define RSX_PKT_GETCOUNT(pkt)           ((pkt).count)
#define RSX_PKT_SET_U8(pkt, count, idx, val) (((uint8_t*)(pkt).data)[count * (pkt).length + idx] = val)
#define RSX_PKT_GET_U8(pkt, count, idx) (((uint8_t*)(pkt).data)[count * (pkt).length + idx])

//#define RSX_LPKT_SETID(pkt, id)         do{pkt.id      = id     ;}while(0)
#define RSX_LPKT_SETID(pkt, count, id)  do{pkt.data[count * pkt.length] = id;}while(0)
#define RSX_LPKT_SETFLAG(pkt, val)      do{pkt.flag    = val    ;}while(0)
#define RSX_LPKT_SETADDR(pkt, val)      do{pkt.address = val    ;}while(0)
#define RSX_LPKT_SETLENGTH(pkt, val)    do{pkt.length  = val + sizeof(uint8_t);}while(0) // TODO:
#define RSX_LPKT_SETCOUNT(pkt, val)     do{pkt.count   = val  ;}while(0)
#define RSX_LPKT_GETLENGTH(pkt)         RSX_PKT_GETLENGTH(pkt)
#define RSX_LPKT_GETCOUNT(pkt)          RSX_PKT_GETCOUNT(pkt)
//#define RSX_LPKT_SET_U8(pkt, count, idx, data) RSX_PKT_SET_U8(pkt, count, idx, data)
#define RSX_LPKT_SET_U8(pkt, count, idx, val)   do{pkt.data[count * pkt.length + (idx + 1)]  = val;}while(0)
#define RSX_LPKT_SET_INT16(pkt, count, idx, val)                \
    do {                                                          \
        RSX_LPKT_SET_U8(pkt, count, idx    , (val     ) & 0xFF);\
        RSX_LPKT_SET_U8(pkt, count, idx + 1, (val >> 8) & 0xFF);\
    } while(0)

#define RSX_SPKT_SETID(pkt, val)        do{(pkt).id      = val;}while(0)
#define RSX_SPKT_SETFLAG(pkt, val)      do{(pkt).flag    = val;}while(0)
#define RSX_SPKT_SETADDR(pkt, val)      do{(pkt).address = val;}while(0)
#define RSX_SPKT_SETLENGTH(pkt, val)    do{(pkt).length  = val;}while(0)
#define RSX_SPKT_GETLENGTH(pkt)         RSX_PKT_GETLENGTH(pkt)
#define RSX_SPKT_GETCOUNT(pkt)          RSX_PKT_GETCOUNT(pkt)
#define RSX_SPKT_SET_U8(pkt, idx, data) RSX_PKT_SET_U8(pkt, 0, idx, data)
#define RSX_SPKT_SET_INT16(pkt, idx, data)                \
    do {                                                    \
        RSX_SPKT_SET_U8(pkt, idx    , (data     ) & 0xFF);\
        RSX_SPKT_SET_U8(pkt, idx + 1, (data >> 8) & 0xFF);\
    } while(0)
#define RSX_SPKT_GET_U8(pkt, idx)       RSX_PKT_GET_U8(pkt, 0, idx)

typedef struct {
  uint8_t vid;   /* virtual servo id */
  uint8_t *data; /* expect uint8_t data[length in rsx_pkt] */
} rsx_vload;

typedef struct {
  uint16_t    header; /* 0xFAAF for short packet. */
  uint8_t         id; /* 1 to 127 (01H to 7FH)    */
  uint8_t       flag;
  uint8_t    address;
  uint8_t     length;
  uint8_t      count;
  uint8_t      *data; // TODO: void*? /* expect uint8_t data[count][length] */
  uint8_t  check_sum; // TODO:
} rsx_pkt; // RS301/302 Short packet

errno_t rsx_pkt_get_size (rsx_pkt *pkt, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  *size = 0;
  *size += sizeof(pkt->header);
  *size += sizeof(pkt->flag);
  *size += sizeof(pkt->address);
  *size += sizeof(pkt->length);
  *size += sizeof(pkt->count);
  // pkt->flag with non-zero indicates that there are no payload.
  if (pkt->flag == 0x00) {
    *size += pkt->count * pkt->length;
  }
  *size += sizeof(pkt->check_sum);

  return EOK;
}

errno_t rsx_pkt_init (rsx_pkt *pkt, uint8_t length, uint8_t count, uint8_t *data) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, data);

  pkt->length = length;
  pkt->count  = count;
  pkt->data   = data;

  return EOK;
}

errno_t rsx_lpkt_init (rsx_pkt *pkt) {
  EVALUE(NULL, pkt);

  pkt->header = 0xFAAF;
  pkt->id     = 0x00;
  pkt->flag   = 0x00;

  return EOK;
}

#define SPKT_FLAG_WRITE_FLASH 0x40 /* 0b 0100 0000 */
#define SPKT_FLAG_RESET_SERVO 0x20 /* 0b 0010 0000 */
#define SPKT_FLAG_MEMMAP_CLR  0x10 /* 0b 0001 0000 */
#define SPKT_FLAG_RTN_PKT_ADDR(addr) (addr & 0x0F /* 0b 0000 1111 */)

errno_t rsx_spkt_init (rsx_pkt *pkt, uint8_t id, uint8_t flag) {
  EVALUE(NULL, pkt);

  pkt->header = 0xFAAF;
  pkt->id     = id;
  pkt->flag   = flag;

  return EOK;
}

errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, src);
  EVALUE(NULL, size);

  size_t idx = 0;
  pkt->header = (((uint16_t)src[idx + 1]) << 8) | (src[idx]);
  idx += 2;

  pkt->id      = src[idx++];
  pkt->flag    = src[idx++];
  pkt->address = src[idx++];
  pkt->length  = src[idx++];
  pkt->count   = src[idx++];

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

   // TODO: sub-serializer
  for (size_t i = 0; i < pkt->count; i++) {
    for (size_t j = 0; j < pkt->length; j++) {
      pkt->data[(i * pkt->length) + j] = src[idx++];
    }
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= src[i];
  }

  //src[idx] = pkt->check_sum;
  printf("recv pkt check sum: %02x vx %02x\n", pkt->check_sum, src[idx]);
  idx++;
  *size = idx;

  return EOK;
}

errno_t rsx_pkt_ser (rsx_pkt *pkt, uint8_t dst[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, dst);
  EVALUE(NULL, size);

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

  size_t idx = 0;
  dst[idx++] = (pkt->header >> 8);
  dst[idx++] = pkt->header;

  dst[idx++] = pkt->id;
  dst[idx++] = pkt->flag;
  dst[idx++] = pkt->address;
  dst[idx++] = pkt->length;

  // TODO: sub-serializer
  // pkt->flag with non-zero indicates that there are no payload.
  if (pkt->flag == 0x00) {
    dst[idx++] = pkt->count;
    for (size_t i = 0; i < pkt->count; i++) {
      for (size_t j = 0; j < pkt->length; j++) {
        dst[idx++] = pkt->data[(i * pkt->length) + j];
      }
    }
  } else {
    dst[idx++] = 0x00;
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= dst[i];
  }

  dst[idx++] = pkt->check_sum;
  *size = idx;

  return EOK;
}

static struct termios g_oldtio,g_newtio;

static errno_t setraw(struct termios *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term->c_cflag &= ~(CSIZE | PARENB);
  term->c_cflag |= CS8;

  return EOK;
}

static errno_t setspeed (struct termios *term, speed_t speed) {
  EVALUE(NULL, term);

  errno_t eno;

  eno = cfsetispeed(term, speed);
  if (eno != 0) {
    printf("file can not set input speed : %d\n", errno);
    return errno;
  }

  eno = cfsetospeed(term, speed);
  if (eno != 0) {
    printf("file can not set output speed : %d\n", errno);
    return errno;
  }

  return EOK;
}

static errno_t setattr (int fd, struct termios *term) {
  EVALUE(0, fd);
  EVALUE(NULL, term);

  int eno;
  eno = tcflush(fd, TCIFLUSH);
  if (eno != 0) {
    printf("file can not tcflush : %d\n", errno);
    return errno;
  }

  eno = tcsetattr(fd, TCSANOW, term);
  if (eno != 0) {
    printf("file can not tcsetattr : %d\n", errno);
    return errno;
  }

  return EOK;
}

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

typedef struct {
  int fd;
  struct termios prev_term;
  struct termios term;
} hr_serial;

errno_t hr_serial_init (hr_serial *ser) {
  EVALUE(NULL, ser);

  ser->fd = 0;
  memset(&(ser->prev_term), 0, sizeof(struct termios));
  memset(&(ser->term), 0, sizeof(struct termios));

  return EOK;
}

errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit) {
  EVALUE(NULL, ser);
  EVALUE(NULL, dev);
  EVALUE(NULL, unit);

#ifdef __unix
  const char *header = "/dev/";
#endif
  size_t len_header = strlen(header);
  size_t len_dev  = strlen(dev);
  size_t len_unit = strlen(unit);

  char path[len_header + len_dev + len_unit + 1];
  memcpy(path                       , header, len_header);
  memcpy(path + len_header          , dev   , len_dev);
  memcpy(path + len_header + len_dev, unit  , len_unit);
  path[len_header + len_dev + len_unit] = '\0';

  ECALL(_open(path, &(ser->fd)));

  tcgetattr(ser->fd, &(ser->prev_term)); /* 現在のポート設定を待避 */
  ECALL(setraw(&(ser->term)));
  ECALL(setspeed(&(ser->term), B115200));
  ECALL(setattr(ser->fd, &(ser->term)));

  return EOK;
}

errno_t hr_serial_close (hr_serial *ser) {
  EVALUE(NULL, ser);

  ECALL(setattr(ser->fd, &(ser->prev_term)));
  ser->fd = 0;

  ECALL(_close(ser->fd));

  return EOK;
}

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);

  size_t send_size;
  ECALL(_write(ser->fd, data, size, &send_size));

  if (size != send_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, size, send_size);
    return -1;
  }

  return EOK;
}

errno_t hr_serial_read (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);

  size_t read_size;
  ECALL(_read(ser->fd, data, size, &read_size));

  if (size != read_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, size, read_size);
    for (size_t i = 0; i < read_size; i++) {
      printf(" %02x", ((uint8_t*)data)[i]);
    }
    printf("\n");
    return -1;
  }

  return EOK;
}

static errno_t data_dump (uint8_t *data, size_t size) {
  EVALUE(NULL, data);

#if defined(DATA_DUMP)
  printf("size:%04zd ::", size);
  for (size_t i = 0; i < size; i++) {
    printf(" %02x", data[i]);
  }
  printf("\n");
#endif

  return EOK;
}

#endif

#include <stdbool.h>

errno_t get_current (hr_serial *hrs, rsx_pkt *rpkt, void* buff/*[size]*/, size_t size, bool use_serial) {
  EVALUE(NULL, hrs);
  EVALUE(NULL, rpkt);
  EVALUE(NULL, buff);

  ECALL(rsx_lpkt_init(rpkt));
  RSX_SPKT_SETID(*rpkt, 0x01);
  RSX_SPKT_SETFLAG(*rpkt, 0xF);
  RSX_SPKT_SETADDR(*rpkt, 0x2A);
  RSX_SPKT_SETLENGTH(*rpkt, 12);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(rpkt, buff, size, &pkt_size));
  ECALL(data_dump(buff, pkt_size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, pkt_size));
  usleep(5 * 1000);
  if (use_serial) ECALL(hr_serial_read(hrs, buff, pkt_size + 12)); // TODO: size + 12
  ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
  ECALL(rsx_pkt_deser(rpkt, buff, size, &pkt_size));

  //printf("\033[2J");
  printf("Current Pos :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 2), RSX_SPKT_GET_U8(*rpkt, 1));
  printf("Current Pos :%+8.3lf [deg]\n",  (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f);
  printf("Current Time:%010d [msec]\n",  ((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 3)) << 8) | RSX_SPKT_GET_U8(*rpkt, 2)));
  printf("Current Vel :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 5), RSX_SPKT_GET_U8(*rpkt, 4));
  printf("Current Vel :%+04d [deg/s]\n", (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 5)) << 8) | RSX_SPKT_GET_U8(*rpkt, 4)));
  printf("\033[6A");


  return EOK;
}

int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial) {
  EVALUE(NULL, hrs);

  size_t count = 0;
  uint8_t buff[1024];
  size_t size;

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

  RSX_SPKT_SETID(rpkt, 0x01);
  RSX_SPKT_SETFLAG(rpkt, 0xF);
  RSX_SPKT_SETADDR(rpkt, 0x00);
  RSX_SPKT_SETLENGTH(rpkt, 4);

  ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
  usleep(10 * 1000);
  if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
  ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
  ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

  printf("Model Number L:%02x H:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1));
  printf("Firmware Version:%02x\n", RSX_SPKT_GET_U8(rpkt, 2));

  ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if defined(TEST_SPKT)
  RSX_SPKT_DECL(spkt, 2);
  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, 0x01);
  RSX_SPKT_SETADDR(spkt, 0x24);
  RSX_SPKT_SETLENGTH(spkt, 0x01);
  RSX_SPKT_SET_U8(spkt, 0, 0x01);

  ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

  usleep(5 * 1000);

  do {
    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
    usleep(5 * 1000);

    RSX_SPKT_SETID(spkt, 0x01);
    RSX_SPKT_SETADDR(spkt, 0x1e);
    RSX_SPKT_SETLENGTH(spkt, 0x02);

    // 0x0384 --> 90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, 900);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(500 * 1000);

    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
    usleep(5 * 1000);

    // 0xFC7C --> -90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, -900);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(500 * 1000);
  } while(count++ < 5);
#else
  const size_t num_of_servo = 32;
  RSX_LPKT_DECL(pkt, num_of_servo, 2);
  RSX_LPKT_INIT(pkt);
  RSX_LPKT_SETADDR(pkt, 0x24);
  RSX_LPKT_SETLENGTH(pkt, 0x01);
  for (size_t i = 0; i < 16; i++) {
    RSX_LPKT_SETID(pkt, i, i + 1);
  }
  RSX_LPKT_SET_U8(pkt, 0, 0, 0x01);

  ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

  usleep(500 * 1000);

  do {
    RSX_LPKT_SETADDR(pkt, 0x1e);
    RSX_LPKT_SETLENGTH(pkt, 0x02);
    for (size_t i = 0; i < 16; i++) {
      RSX_LPKT_SETID(pkt, i, i + 1);
    }

    // 0x0384 --> 90[deg]
    RSX_LPKT_SET_INT16(pkt, 0, 0, 900);

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(10 * 1000);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
      usleep(100 * 1000);
    }

    // 0xFC7C --> -90[deg]
    RSX_LPKT_SET_INT16(pkt, 0, 0, -900);

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(10 * 1000);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
      usleep(100 * 1000);
    }
  } while(count++ < 5);
#endif

  printf("\033[6B");

  if (use_serial) ECALL(hr_serial_close(hrs));

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  bool use_serial = false;

  if (argc >= 3) {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, argv[1], argv[2]));
  } else {
    //use_serial = true;
    //ECALL(hr_serial_open(hrs, "ttyUSB", "0"));
  }

  run_test(argc, argv, &hrs, use_serial);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}
