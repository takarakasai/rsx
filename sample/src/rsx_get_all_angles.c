
/* for printf */
#include <stdio.h>

/* for datadump */
#include "dpservo.h"

#include "rsx/rsx_pkt.h"
#include "rsx/rsx_io.h"

//#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
//#endif

#include <stdbool.h>

errno_t get_current (hr_serial *hrs, rsx_pkt *rpkt, void* buff/*[size]*/, size_t size, bool use_serial) {
  EVALUE(NULL, hrs);
  EVALUE(NULL, rpkt);
  EVALUE(NULL, buff);

  ECALL(rsx_pkt_reset(rpkt));
  const size_t num_of_axis = 20;
  float ang[num_of_axis];
  for (size_t i = 0; i < num_of_axis; i++) {
    size_t retry = 5;
    for (size_t j = 0; j < retry; j++) {
      ECALL(rsx_pkt_reset(rpkt));
      //RSX_SPKT_SETID(*rpkt, 0x01);
      //RSX_SPKT_SETID(*rpkt, 12);
      RSX_SPKT_SETID(*rpkt, 1 + i);
      RSX_SPKT_SETFLAG(*rpkt, 0xF);
      RSX_SPKT_SETADDR(*rpkt, 0x2A);
      RSX_SPKT_SETLENGTH(*rpkt, 2);

      size_t pkt_size;
      ECALL(rsx_pkt_ser(rpkt, buff, size, &pkt_size));
      if (use_serial) {
        //ECALL(data_dump(buff, pkt_size));
        ECALL(hr_serial_write(hrs, buff, pkt_size));
        errno_t eno = hr_serial_read(hrs, buff, pkt_size + 2);
        //ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
        if (eno == EOK) {
          ECALL(rsx_pkt_deser(rpkt, buff, size, &pkt_size));


          ang[i] = (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f;
          printf("%02zd[%02d] : %+8.3f FLAG:%04x try:%zd", i + 1, RSX_SPKT_GETID(*rpkt), ang[i], RSX_SPKT_GETFLAG(*rpkt), j);
          printf("\n");
          break;
        } else {
          if (j == retry - 1) {
            ang[i] = -0.0;
            printf("%02zd[%02d] :    error\n", i + 1, RSX_SPKT_GETID(*rpkt));
          }
          continue;
        }
      }
    }
  }

  //for (size_t i = 0; i < num_of_axis; i++) {
  //  printf("%+8.3f\n", ang[i]);
  //}
  printf("\033[%zdA", num_of_axis);

  return EOK;
}

int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial) {
  EVALUE(NULL, hrs);

  //size_t count = 0;
  uint8_t buff[1024];
  size_t size;

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

  for (size_t i = 0; i < 3; i++) {
    ECALL(rsx_pkt_reset(&rpkt));

    RSX_SPKT_SETID(rpkt, i + 1);
    RSX_SPKT_SETFLAG(rpkt, 0xF);
    RSX_SPKT_SETADDR(rpkt, 0x00);
    RSX_SPKT_SETLENGTH(rpkt, 4);

    ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
    if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
    ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
    ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

    printf("Model Number L:%02x H:%02x FW-ver:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1), RSX_SPKT_GET_U8(rpkt, 2));
    hr_usleep(100 * 1000);
  }

  //ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

  const size_t num_of_servo = 20;
  RSX_LPKT_DECL(pkt, num_of_servo, 20);
  RSX_LPKT_INIT(pkt);
  RSX_LPKT_SETADDR(pkt, 0x24);
  RSX_LPKT_SETLENGTH(pkt, 0x01);
  for (size_t i = 0; i < 20; i++) {
    RSX_LPKT_SETVID(pkt, i, i + 1);
    //RSX_LPKT_SET_U8(pkt, i, 0, 0x00); // servo off
    RSX_LPKT_SET_U8(pkt, i, 0, 0x01); // servo on
    //RSX_LPKT_SET_U8(pkt, i, 0, 0x02); // servo break
  }
  ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

  printf("SERVO STATUS : ");
  for (size_t i = 0; i < 20; i++) {
    printf(" |%02zd", i + 1);
  }
  printf("\n");
  printf("             : ");
  for (size_t i = 0; i < 20; i++) {
    ECALL(rsx_pkt_reset(&rpkt));

    RSX_SPKT_SETID(rpkt, i + 1);
    RSX_SPKT_SETFLAG(rpkt, 0xF);
    RSX_SPKT_SETADDR(rpkt, 0x24);
    RSX_SPKT_SETLENGTH(rpkt, 1);

    ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
    hr_usleep(10 * 1000);
    if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 1)); // TODO: size + 2
    ECALL(data_dump(buff, size + 1));                           // TODO: size + 2
    ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

    printf(" |%02x", RSX_SPKT_GET_U8(rpkt, 0));

    hr_usleep(20 * 1000);
  }
  printf("\n");

  ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if 1
  int pose[20][20] = {
    /*        1     2  |   3     4     5  |   6     7     8  |   9     10    11     12     13    14  |  15     16    17     18     19    20*/
    /* 1*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   0,  0, 0,  0,  0, 0,  0,  0, 0},
    /* 2*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  1, 0,-  2,- 2, 0,  0,- 1, 0,  2,  1, 0},
    /* 3*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  3, 0,-  6,- 6, 0,  0,- 3, 0,  6,  3, 0},
    /* 4*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 12,-12, 0,  0,- 6, 0, 12,  6, 0},
    /* 5*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 10, 0,- 20,-10, 0,  0,-10, 0, 20, 10, 0},
    /* 6*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 15, 0,- 30,-15, 0,  0,-15, 0, 30, 15, 0},
    /* 7*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 19, 0,- 38,-19, 0,  0,-19, 0, 38, 19, 0},
    /* 8*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 22, 0,- 44,-22, 0,  0,-22, 0, 44, 22, 0},
    /* 9*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 24, 0,- 48,-24, 0,  0,-24, 0, 48, 24, 0},
    /*10*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 25, 0,- 50,-25, 0,  0,-25, 0, 50, 25, 0},
    /*10*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 25, 0,- 50,-25, 0,  0,-25, 0, 50, 25, 0},
    /* 9*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 24, 0,- 48,-24, 0,  0,-24, 0, 48, 24, 0},
    /* 8*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 22, 0,- 44,-22, 0,  0,-22, 0, 44, 22, 0},
    /* 7*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 19, 0,- 38,-19, 0,  0,-19, 0, 38, 19, 0},
    /* 6*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 15, 0,- 30,-15, 0,  0,-15, 0, 30, 15, 0},
    /* 5*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0, 10, 0,- 20,-10, 0,  0,-10, 0, 20, 10, 0},
    /* 4*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 12,- 6, 0,  0,- 6, 0, 12,  6, 0},
    /* 3*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  3, 0,-  6,- 3, 0,  0,- 3, 0,  6,  3, 0},
    /* 2*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  1, 0,-  2,- 1, 0,  0,- 1, 0,  2,  1, 0},
    /* 1*/{ -30, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   0,  0, 0,  0,  0, 0,  0,  0, 0},
  };
  size_t num_of_pose = sizeof(pose) / sizeof(pose[0]);
#endif

  for (size_t idx = 0; idx < 200; idx++) {

#if 1
    RSX_LPKT_SETADDR(pkt, 0x1e);
    RSX_LPKT_SETLENGTH(pkt, 0x02);
    for (size_t i = 0; i < 20; i++) {
      size_t pidx = idx % num_of_pose;
      RSX_LPKT_SETVID(pkt, i, i + 1);
      //RSX_LPKT_SET_INT16(pkt, i, 0, (int)(0 * 10));
      RSX_LPKT_SET_INT16(pkt, i, 0, (int)(pose[pidx][i] * 10));
    }

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
#endif

    //ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
 
    hr_usleep(15 * 1000);
  }

  printf("\033[20B");

  hr_usleep(100 * 1000);

  for (size_t i=0;i<10;i++) {
    /* servo off */
    RSX_LPKT_DECL(pkt2, num_of_servo, 2);
    RSX_LPKT_INIT(pkt2);
    RSX_LPKT_SETADDR(pkt2, 0x24);
    RSX_LPKT_SETLENGTH(pkt2, 0x01);
    for (size_t i = 0; i < 20; i++) {
      RSX_LPKT_SETVID(pkt2, i, i + 1);
      RSX_LPKT_SET_U8(pkt2, i, 0, 0x00); // servo off
      //RSX_LPKT_SET_U8(pkt2, i, 0, 0x01); // servo on
      //RSX_LPKT_SET_U8(pkt, i, 0, 0x02); // servo break
    }
    ECALL(rsx_pkt_ser(&pkt2, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

    hr_usleep(100 * 1000);
  }

  printf("----- end ----- \n");

  hr_usleep(500 * 1000);

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  bool use_serial = false;

  if (argc >= 3) {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, argv[1], argv[2], HR_B460800, HR_PAR_NONE));
  } else {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, "ttyUSB", "0", HR_B460800, HR_PAR_NONE));
  }

  run_test(argc, argv, &hrs, use_serial);
  printf("\n");

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}

