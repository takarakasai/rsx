
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx_pkt.h"
#include "rsx_io.h"

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif

#include <stdbool.h>

errno_t get_current (hr_serial *hrs, rsx_pkt *rpkt, void* buff/*[size]*/, size_t size, bool use_serial) {
  EVALUE(NULL, hrs);
  EVALUE(NULL, rpkt);
  EVALUE(NULL, buff);

  const size_t num_of_axis = 20;
  for (size_t i = 0; i < num_of_axis; i++) {
    size_t retry = 1;
    for (size_t j = 0; j < retry; j++) {
      ECALL(rsx_pkt_reset(rpkt));
      RSX_SPKT_SETID(*rpkt, 0x01);
      RSX_SPKT_SETFLAG(*rpkt, 0xF);
      RSX_SPKT_SETADDR(*rpkt, 0x2A);
      RSX_SPKT_SETLENGTH(*rpkt, 6);

      size_t pkt_size;
      ECALL(rsx_pkt_ser(rpkt, buff, size, &pkt_size));
      if (use_serial) {
        //ECALL(data_dump(buff, pkt_size));
        ECALL(hr_serial_write(hrs, buff, pkt_size));
        errno_t eno = hr_serial_read(hrs, buff, pkt_size + 2);
        //ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
        if (eno == EOK) {
          ECALL(rsx_pkt_deser(rpkt, buff, size, &pkt_size));
          break;
        } else {
          if (j == retry - 1) {
            printf("%02zd[%02d] :    error\n", i + 1, RSX_SPKT_GETID(*rpkt));
          }
          continue;
        }
      }
    }
  }

  printf("Current Pos :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 2), RSX_SPKT_GET_U8(*rpkt, 1));
  printf("Current Pos :%+8.3lf [deg]\n",  (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f);
  printf("Current Time:%010d [msec]\n",  ((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 3)) << 8) | RSX_SPKT_GET_U8(*rpkt, 2)));
  printf("Current Vel :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 5), RSX_SPKT_GET_U8(*rpkt, 4));
  printf("Current Vel :%+04d [deg/s]\n", (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 5)) << 8) | RSX_SPKT_GET_U8(*rpkt, 4)));

  return EOK;
}

int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial) {
  EVALUE(NULL, hrs);

  size_t count = 0;
  uint8_t buff[1024];
  size_t size;

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

  RSX_SPKT_SETID(rpkt, 1);
  RSX_SPKT_SETFLAG(rpkt, 0xF);
  RSX_SPKT_SETADDR(rpkt, 0x00);
  RSX_SPKT_SETLENGTH(rpkt, 4);

  ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
  if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
  ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
  ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

  printf("Model Number L:%02x H:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1));
  printf("Firmware Version:%02x\n", RSX_SPKT_GET_U8(rpkt, 2));

  ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

  RSX_SPKT_DECL(spkt, 2);
  RSX_SPKT_INIT(spkt);
  RSX_SPKT_SETID(spkt, 0x01);
  RSX_SPKT_SETADDR(spkt, 0x24);
  RSX_SPKT_SETLENGTH(spkt, 0x01);
  RSX_SPKT_SET_U8(spkt, 0, 0x01);

  ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  //if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

  usleep(5 * 1000);

  do {
    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
    //usleep(5 * 1000);

#if defined(HR_SERIAL_LATENCY_CHECK)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

    RSX_SPKT_SETID(spkt, 0x01);
    RSX_SPKT_SETADDR(spkt, 0x1e);
    RSX_SPKT_SETLENGTH(spkt, 0x02);

    // 0x0384 --> 90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, 0);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    //usleep(50 * 1000);
    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if defined(HR_SERIAL_LATENCY_CHECK)
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf("           read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif

#if defined(HR_SERIAL_LATENCY_CHECK)
  ECALL(hr_get_time(&tm_bef));
#endif

    //usleep(5 * 1000);

    // 0xFC7C --> -90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, -900);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if defined(HR_SERIAL_LATENCY_CHECK)
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf("           read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif

    //usleep(50 * 1000);
  } while(count++ < 5);

  usleep(10 * 1000);

#if 0 // TODO:
  /* servo off */
  RSX_LPKT_DECL(pkt2, num_of_servo, 2);
  RSX_LPKT_INIT(pkt2);
  RSX_LPKT_SETADDR(pkt2, 0x24);
  RSX_LPKT_SETLENGTH(pkt2, 0x01);
  for (size_t i = 0; i < 20; i++) {
    RSX_LPKT_SETID(pkt2, i, i + 1);
    RSX_LPKT_SET_U8(pkt2, i, 0, 0x00);
  }
  ECALL(rsx_pkt_ser(&pkt2, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));
#endif

  printf("----- end ----- \n");
  usleep(10 * 1000);

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
    use_serial = true;
    ECALL(hr_serial_open(&hrs, "ttyUSB", "0"));
  }

  run_test(argc, argv, &hrs, use_serial);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}
