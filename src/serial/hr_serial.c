
#include "serial/hr_serial.h"

#include <assert.h>

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

#include <stdbool.h>

/* for usleep */
#include <unistd.h>

#include "rsx_err.h"
#include "rsx_common.h"

#define RSX_DEBUG_PRINT2(...) printf(__VA_ARGS__);

static errno_t setraw(struct termios *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term->c_cflag &= ~(CSIZE | PARENB);
  //term->c_cflag |= CS8 | CRTSCTS;
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

/* speed [bps] */
static errno_t convert_speed(int from, speed_t* to) {
  EVALUE(0, to);

  switch (from) {
    case   9600: *to =   B9600; break;
#if defined(B28800)
    case  14400: *to =  B14400; break;
#endif
    case  19200: *to =  B19200; break;
#if defined(B28800)
    case  28800: *to =  B28800; break;
#endif
    case  38400: *to =  B38400; break;
    case  57600: *to =  B57600; break;
#if defined(B76800)
    case  76800: *to =  B76800; break;
#endif
    case 115200: *to = B115200; break;
#if defined(B28800)
    case 153600: *to = B153600; break;
#endif
    case 230400: *to = B230400; break;
    default:
      return EINVAL;
  }

  return EOK;
}

errno_t hr_serial_init (hr_serial *ser) {
  EVALUE(NULL, ser);

  ser->fd = 0;
  ser->baudrate = B115200;

  memset(&(ser->prev_term), 0, sizeof(struct termios));
  memset(&(ser->term), 0, sizeof(struct termios));

  return EOK;
}

errno_t hr_serial_set_baudrate(hr_serial *ser, int baudrate) {
  EVALUE(NULL, ser);

  errno_t eno = convert_speed(baudrate, &(ser->baudrate));
  if (ser->fd <= 0) {
    return eno;
  }
  ECALL(setspeed(&(ser->term), ser->baudrate));
  ECALL(setattr(ser->fd, &(ser->term)));

  return eno;
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
  ECALL(setspeed(&(ser->term), ser->baudrate));
  ECALL(setattr(ser->fd, &(ser->term)));

  return EOK;
}

errno_t hr_serial_close (hr_serial *ser) {
  EVALUE(NULL, ser);
  EVALUE(0, ser->fd);

  ECALL(setattr(ser->fd, &(ser->prev_term)));

  ECALL(_close(ser->fd));

  ser->fd = 0;

  return EOK;
}

errno_t hr_serial_read (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);

  enum {
    kSearchDeliminator = 0,
    kReadHeader        = 1,
    kSkipPacket        = 2,
    kReadBody          = 3
  } state;

  /* | ==================== PACKET ===================| */
  /* | -------------- HEADER ------------ |-- Body ---| */
  /* | DLIMINATOR |                       |     |     | */
  /* | LOW | HIGH | ID  | FLG | ADR | LEN | CNT | DAT | SUM |  */
  /*    1     1      1     1     1     1     1    LEN       1  */
  /*    0     1      2     3     4     5     6 7-7+LEN-1 7+LEN */

  /* ==== FSM ==== */
  /* SearchDeliminator --> ReadHeader --> ReadBody */
  /*       ^                    |                  */
  /*       +--------------------+                  */

#if 0
  static size_t offset = 0;
  static uint8_t buff[1024] = {0x00};
  size_t rsize;
  ECALL(_read(ser->fd, buff + offset, 1024 - offset, &rsize));
  offset += rsize;
#endif

  state = kSearchDeliminator;
  size_t packet_size  = 0;
  // size_t read_size    = 0;
  static size_t read_size   = 0;
  static uint8_t buff[1024] = {0x00};
  size_t zcount       = 0;
  RSX_DEBUG_PRINT("Start=============================\n");
  do {
    RSX_DEBUG_PRINT2("state %d %zd\n", state, read_size);
    size_t siz = -1;
    if (state == kSearchDeliminator || state == kReadHeader) {
      if (read_size < 6) {
        ECALL(_read(ser->fd, buff + read_size, 1024 - read_size, &siz));
        RSX_DEBUG_PRINT2(" Dlim : %zd --> %zd\n", 6 - read_size, siz);
        read_size += siz;
      }
      // assert(read_size < 6);
      // ECALL(_read(ser->fd, data + read_size, 6 - read_size, &siz));
      // RSX_DEBUG_PRINT2(" Dlim : %zd --> %zd\n", 6 - read_size, siz);
    } else if (state == kSkipPacket) {
      if (read_size < packet_size) {
        ECALL(_read(ser->fd, buff + read_size, 1024 - read_size, &siz));
        RSX_DEBUG_PRINT2(" Skip : %zd --> %zd\n", packet_size - read_size, siz);
        read_size += siz;
      }
      // assert(read_size < packet_size);
      // ECALL(_read(ser->fd, data + read_size, packet_size - read_size, &siz));
      // RSX_DEBUG_PRINT2(" Skip : %zd --> %zd\n", packet_size - read_size, siz);
    } else {
      if (read_size < size) {
        ECALL(_read(ser->fd, buff + read_size, 1024 - read_size, &siz));
        RSX_DEBUG_PRINT2(" Other : %zd --> %zd\n", size - read_size, siz);
        read_size += siz;
      }
      // assert(read_size < size);
      // ECALL(_read(ser->fd, data + read_size, size - read_size, &siz));
      // RSX_DEBUG_PRINT2(" Other : %zd --> %zd\n", size - read_size, siz);
    }
    // read_size += siz;

    if (state == kSearchDeliminator) {
      if (read_size >= 2) {
        for (size_t i = 0; i < read_size - 1; i++) {
          if ((((uint8_t*)buff)[i]     == 0xFD) &&
              (((uint8_t*)buff)[i + 1] == 0xDF)) {
            RSX_DEBUG_PRINT2("%s : skip %ld [B]\n", __func__, i);
            memmove(buff, buff + i, read_size - i);
            read_size -= i;
            state = kReadHeader;
            // break;
          }
        }
        if (state == kSearchDeliminator) {
          read_size = 0;
        }
      }
    }
    if (state == kReadHeader) {
      if (read_size >= 5) {
        uint8_t len = ((uint8_t*)buff)[5];
        // FIXME(takara.kasai@gmail.com) : 8 to be changed to RSX_PKT_SIZE_MIN
        packet_size = len + 8;
        RSX_DEBUG_PRINT2("read header %zd > %zd (%02x + 0x08)\n", size, packet_size, len);
        for (int i = 0; i < 5; i++) {
          printf(" %02x", ((uint8_t*)buff)[i]);
        }
        printf("\n");
        if (size == packet_size) {
          state = kReadBody;
        } else {
          state = kSkipPacket;
        }
      }
    }
    if (state == kSkipPacket) {
      if (read_size >= packet_size) {
        RSX_DEBUG_PRINT2("skip packet %zd > %zd\n", read_size, packet_size);
        state = kSearchDeliminator;
        read_size = 0;
        packet_size = 0;
      }
    }
    if (state == kReadBody) {
      if (read_size >= size) {
        memcpy(data, buff, size);
        memmove(buff, buff + size, read_size - size);
        read_size -= size;
        printf("OK(%zd,remain:%zd):", size, read_size);
        for (size_t i = 0; i < size; i++) {
          printf(" %02x", ((uint8_t*)data)[i]);
        }
        break;
      }
    }

    if (siz == 0) {
      zcount++;
      usleep(100);
      if (zcount > 10) {
        printf("======================== exit : size:%zd expect:%zd state:%d\n", read_size, size, state);
        for (size_t i = 0; i < read_size; i++) {
          printf(" %02x", ((uint8_t*)data)[i]);
        }
        printf("\n");
        // assert(false);
        return -1;
      }
    } else {
      zcount = 0;
    }
  } while(1);

  return EOK;
}

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size) {

  size_t send_size;
  ECALL(_write(ser->fd, data, size, &send_size));

  if (size != send_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, size, send_size);
    return -1;
  }

// #if defined(HR_SERIAL_AUTO_READ_ECHO_DATA)
  // usleep(50);
  // usleep(1000);
#if 0
  uint8_t echo;
  size_t read_size;
  ECALL(_read(ser->fd, &echo, 1, &read_size));
  if (read_size > 0) {
    printf("%02x\n", echo); fflush(stdout);
  }
  uint8_t echo;
  size_t read_size;
  ECALL(_read(ser->fd, &echo, 1, &read_size));
  uint8_t echo;
  size_t read_size;
  int count = 0;
  do {
    printf("\r%d", count++); fflush(stdout);
    ECALL(_read(ser->fd, &echo, 1, &read_size));
  } while (read_size == 0);
  printf("echo %zd\n", echo);
 #endif
// #endif

  return EOK;
}

