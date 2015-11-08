
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx.h"
#include "rsx_io.h"

#include <stdbool.h>

errno_t get_current (hr_serial *hrs, rsx_pkt *rpkt, void* buff/*[size]*/, size_t size, bool use_serial) {
  EVALUE(NULL, hrs);
  EVALUE(NULL, rpkt);
  EVALUE(NULL, buff);

  ECALL(rsx_lpkt_init(rpkt));
  const size_t num_of_axis = 20;
  float ang[num_of_axis];
  for (size_t i = 0; i < num_of_axis; i++) {
    size_t retry = 1;
    for (size_t j = 0; j < retry; j++) {
      ECALL(rsx_lpkt_init(rpkt));
      //RSX_SPKT_SETID(*rpkt, 0x01);
      //RSX_SPKT_SETID(*rpkt, 12);
      RSX_SPKT_SETID(*rpkt, 1 + i);
      RSX_SPKT_SETFLAG(*rpkt, 0xF);
      RSX_SPKT_SETADDR(*rpkt, 0x2A);
      RSX_SPKT_SETLENGTH(*rpkt, 2);

      size_t pkt_size;
      ECALL(rsx_pkt_ser(rpkt, buff, size, &pkt_size));
      //ECALL(data_dump(buff, pkt_size));
      if (use_serial) ECALL(hr_serial_write(hrs, buff, pkt_size));
      usleep(15 * 1000);
      //if (use_serial) ECALL(hr_serial_read(hrs, buff, pkt_size + 12)); // TODO: size + 12
      if (use_serial) {
        errno_t eno = hr_serial_read(hrs, buff, pkt_size + 2);
        //ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
        //usleep(10 * 1000);
        //if (eno != EOK) eno = hr_serial_read(hrs, buff, pkt_size + 12);
        //usleep(10 * 1000);
        //if (eno != EOK) eno = hr_serial_read(hrs, buff, pkt_size + 12);
        //ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
        if (eno == EOK) {
          ECALL(rsx_pkt_deser(rpkt, buff, size, &pkt_size));

          ang[i] = (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f;
          printf("%02zd[%02d] : %+8.3f FLAG:%04x : ", i + 1, RSX_SPKT_GETID(*rpkt), ang[i], RSX_SPKT_GETFLAG(*rpkt));
          ECALL(data_dump(buff, pkt_size + 2));
          printf("\n");
          break;
        } else {
          //ang[i] = -0.0;
          //printf("%02zd :    error\n", i + 1);
          if (j == retry - 1) {
            ang[i] = -0.0;
            printf("%02zd[%02d] :    error\n", i + 1, RSX_SPKT_GETID(*rpkt));
          }
          continue;
        }
      }
      usleep(15 * 1000);
    }
  }

#if 0
  printf("Current Pos :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 2), RSX_SPKT_GET_U8(*rpkt, 1));
  printf("Current Pos :%+8.3lf [deg]\n",  (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f);
  printf("Current Time:%010d [msec]\n",  ((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 3)) << 8) | RSX_SPKT_GET_U8(*rpkt, 2)));
  printf("Current Vel :%02x %02x\n",  RSX_SPKT_GET_U8(*rpkt, 5), RSX_SPKT_GET_U8(*rpkt, 4));
  printf("Current Vel :%+04d [deg/s]\n", (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 5)) << 8) | RSX_SPKT_GET_U8(*rpkt, 4)));
#endif
  //for (size_t i = 0; i < num_of_axis; i++) {
  //  printf("%+8.3f\n", ang[i]);
  //}
  printf("\033[%zdA", num_of_axis);

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
  usleep(10 * 1000);
  if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
  ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
  ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

  printf("Model Number L:%02x H:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1));
  printf("Firmware Version:%02x\n", RSX_SPKT_GET_U8(rpkt, 2));

  //ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if defined(TEST_SPKT)
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
    usleep(5 * 1000);

    RSX_SPKT_SETID(spkt, 0x01);
    RSX_SPKT_SETADDR(spkt, 0x1e);
    RSX_SPKT_SETLENGTH(spkt, 0x02);

    // 0x0384 --> 90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, 0);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    //if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(50 * 1000);

    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
    usleep(5 * 1000);

    // 0xFC7C --> -90[deg]
    RSX_SPKT_SET_INT16(spkt, 0, -900);

    ECALL(rsx_pkt_ser(&spkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    //if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(50 * 1000);
  } while(count++ < 5);
#else
  const size_t num_of_servo = 32;
  RSX_LPKT_DECL(pkt, num_of_servo, 2);
  RSX_LPKT_INIT(pkt);
  RSX_LPKT_SETADDR(pkt, 0x24);
  RSX_LPKT_SETLENGTH(pkt, 0x01);
  for (size_t i = 0; i < 20; i++) {
    RSX_LPKT_SETID(pkt, i, i + 1);
    //RSX_LPKT_SET_U8(pkt, i, 0, 0x00); // servo off
    RSX_LPKT_SET_U8(pkt, i, 0, 0x01); // servo on
    //RSX_LPKT_SET_U8(pkt, i, 0, 0x02); // servo break
  }
  ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

  usleep(500 * 1000);

  float pose[2][20] = {
    /*  1     2  |   3     4     5  |   6     7     8  |   9     10    11     12     13    14  |  15     16    17     18     19    20*/
    { 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 0.0f,  0.0f,  0.0f, 0.0f},
    { 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 20.0f, 0.0f,-40.0f,-20.0f, 0.0f,  0.0f,-20.0f, 0.0f, 40.0f, 20.0f, 0.0f},
  };

  do {
    RSX_LPKT_SETADDR(pkt, 0x1e);
    RSX_LPKT_SETLENGTH(pkt, 0x02);
    for (size_t i = 0; i < 20; i++) {
      RSX_LPKT_SETID(pkt, i, i + 1);
      RSX_LPKT_SET_INT16(pkt, i, 0, (int)(pose[0][i] * 10));
    }

    // 0x0384 --> 90[deg]
    //RSX_LPKT_SET_INT16(pkt, 0, 0, 900);
    //RSX_LPKT_SET_INT16(pkt, 0, 0, 0);

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(200 * 1000);

    usleep(15 * 1000);
    //for (size_t i = 0; i < 5; i++) {
      ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
      //usleep(100 * 1000);
    //}

    for (size_t i = 0; i < 20; i++) {
      // 0xFC7C --> -90[deg]
      //RSX_LPKT_SET_INT16(pkt, 0, 0, -900);
      RSX_LPKT_SET_INT16(pkt, i, 0, (int)(pose[1][i] * 10));
    }

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));

    usleep(200 * 1000);

    usleep(15 * 1000);
    //for (size_t i = 0; i < 5; i++) {
      ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
      //usleep(100 * 1000);
    //}
    //
    usleep(500 * 1000);

  } while(count++ < 1);
#endif

  printf("\033[20B");

  usleep(10 * 1000);

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

  printf("----- end ----- \n");
  usleep(10 * 1000);

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
