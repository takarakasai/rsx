
/* for printf */
#include <stdio.h>

/* for strto* */
#include <stdlib.h>

/* for datadump */
#include "dpservo.h"

//#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
//endif

#include "rsx/rsx_pkt.h"
#include "rsx/rsx_io.h"

#include <stdbool.h>

static const uint8_t k_min_id =   1;
static const uint8_t k_max_id = 255;

static const uint8_t g_baudrate_val[] = {
     0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,   0x07,   0x08,   0x09,   0x0A
};

static const uint32_t g_baudrate[] = {
    9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200, 153600, 230400, 460800
};

errno_t get_baudrate_val(uint32_t baudrate, uint8_t *val) {
  EVALUE(NULL, val);

  errno_t ret = EINVAL;

  for (size_t i = 0; i < sizeof(g_baudrate) / sizeof(g_baudrate[0]); i++) {
    if (baudrate == g_baudrate[i]) {
      *val = g_baudrate_val[i];
      printf("  baudrate:%ud --> %02x\n", baudrate, *val);
      ret = EOK;
      break;
    }
  }

  return ret;
}

errno_t get_baudrate(uint8_t val, uint32_t *baudrate) {
  EVALUE(NULL, baudrate);

  errno_t ret = EINVAL;

  for (size_t i = 0; i < sizeof(g_baudrate_val) / sizeof(g_baudrate_val[0]); i++) {
    if (val == g_baudrate_val[i]) {
      *baudrate = g_baudrate[i];
      //printf("  baudrate:%ud --> %02x\n", baudrate, *val);
      ret = EOK;
      break;
    }
  }

  return ret;
}



int run_test(int argc, char *argv[], hr_serial *hrs,
  bool use_serial, uint8_t servo_id, uint8_t write_val) {

  EVALUE(NULL, hrs);

  //size_t count = 0;
  uint8_t buff[1024];
  size_t size;

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

  RSX_SPKT_SETID(rpkt, servo_id);
  RSX_SPKT_SETFLAG(rpkt, 0xF);
  RSX_SPKT_SETADDR(rpkt, 0x00);
  RSX_SPKT_SETLENGTH(rpkt, 4);

  ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
  hr_usleep(100 * 1000);
  if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
  ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
  ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

  printf("Model Number L:%02x H:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1));
  printf("Firmware Version:%02x\n", RSX_SPKT_GET_U8(rpkt, 2));

  RSX_SPKT_DECL(spkt, 1);
  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, servo_id);
  RSX_SPKT_SETFLAG(spkt, 0x00);
  RSX_SPKT_SETADDR(spkt, 0x06);
  RSX_SPKT_SETLENGTH(spkt, 0x01);
  RSX_SPKT_SET_U8(spkt, 0, write_val);

  ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

  ///////////// save rom ////////////////////////////////

  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, servo_id);
  RSX_SPKT_SETFLAG(spkt, 0x40);
  RSX_SPKT_SETADDR(spkt, 0xFF);
  RSX_SPKT_SETLENGTH(spkt, 0x00);

  ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

  hr_usleep(1000 * 1000);

  ///////////// read rom ////////////////////////////////

  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, 0x01);
  RSX_SPKT_SETFLAG(spkt, 0xF); /* read from indicated address */
  RSX_SPKT_SETADDR(spkt, 0x06); /* baudrate area */
  RSX_SPKT_SETLENGTH(spkt, 1);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&spkt, buff, size, &pkt_size));
  if (use_serial) {
    ECALL(data_dump(buff, pkt_size));
    ECALL(hr_serial_write(hrs, buff, pkt_size));
    errno_t eno = hr_serial_read(hrs, buff, pkt_size + 1);
    ECALL(data_dump(buff, pkt_size + 1));                           // TODO: size + 12
    if (eno == EOK) {
      ECALL(rsx_pkt_deser(&spkt, buff, size, &pkt_size));
      uint32_t baudrate;
      ECALL(get_baudrate(RSX_SPKT_GET_U8(spkt, 0), &baudrate));
      printf("[id:%02d] : %02x --> %d\n", RSX_SPKT_GETID(spkt), RSX_SPKT_GET_U8(spkt, 0), baudrate);
    } else {
      printf("[id:%02d] : error\n", RSX_SPKT_GETID(spkt));
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 5) {
    printf("    usage : cmd device port servo_id baudrate\n");
    printf(" servo_id : 1-255\n");
    printf(" baudrate :");
    for (size_t i = 0; i < sizeof(g_baudrate) / sizeof(g_baudrate[0]); i++) {
      printf(" %d", g_baudrate[i]);
    }
    printf("\n");
    printf("       ex : cmd ttyUSB 0 115200\n");

    return 0;
  }

  bool use_serial = false;

  uint8_t servo_id = strtoul(argv[3], NULL, 10);
  ELTGT(k_min_id, k_max_id, servo_id)

  uint8_t val;
  uint32_t baudrate = strtod(argv[4], NULL);
  ECALL(get_baudrate_val(baudrate, &val));

  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  use_serial = true;
  ECALL(hr_serial_open(&hrs, argv[1], argv[2], HR_B115200, HR_PAR_NONE));

  run_test(argc, argv, &hrs, use_serial, servo_id, val);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}
