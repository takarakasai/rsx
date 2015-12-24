
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

/* for datadump */
#include "dpservo.h"

#include "rsx/rsx_pkt.h"
#include "rsx/rsx_io.h"

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif

#include <stdbool.h>

errno_t get_current (hr_serial *hrs, rsx_pkt *rpkt, void* buff/*[size]*/, size_t size, bool use_serial) {
  EVALUE(NULL, hrs);
  EVALUE(NULL, rpkt);
  EVALUE(NULL, buff);

  ECALL(rsx_pkt_reset(rpkt));
  const size_t num_of_axis = 20;
#if PRINT_ANG
  float ang[num_of_axis];
#endif
  for (size_t i = 0; i < num_of_axis; i++) {
    size_t retry = 1;
    for (size_t j = 0; j < retry; j++) {
      ECALL(rsx_pkt_reset(rpkt));
      RSX_SPKT_SETID(*rpkt, 1 + i);
      RSX_SPKT_SETFLAG(*rpkt, 0xF); // 指定アドレス
      //RSX_SPKT_SETFLAG(*rpkt, 0x9); // No.42 - 59 : 18[byte]
      RSX_SPKT_SETADDR(*rpkt, 0x2A);
      RSX_SPKT_SETLENGTH(*rpkt, 6);
      //RSX_SPKT_SETLENGTH(*rpkt, 0);

      size_t pkt_size;
      ECALL(rsx_pkt_ser(rpkt, buff, size, &pkt_size));
      //ECALL(data_dump(buff, pkt_size));
      if (use_serial) ECALL(hr_serial_write(hrs, buff, pkt_size));
      if (use_serial) {
        errno_t eno = hr_serial_read(hrs, buff, pkt_size + 6);
        //errno_t eno = hr_serial_read(hrs, buff, pkt_size + 18);
        //ECALL(data_dump(buff, pkt_size + 12));                           // TODO: size + 12
        if (eno == EOK) {
          ECALL(rsx_pkt_deser(rpkt, buff, size, &pkt_size));

#if PRINT_ANG
          ang[i] = (int16_t)((((uint16_t)RSX_SPKT_GET_U8(*rpkt, 1)) << 8) | RSX_SPKT_GET_U8(*rpkt, 0)) / 10.0f;
          printf("%02zd[%02d] : %+8.3f FLAG:%04x : ", i + 1, RSX_SPKT_GETID(*rpkt), ang[i], RSX_SPKT_GETFLAG(*rpkt));
          //ECALL(data_dump(buff, pkt_size + 2));
          printf("\n");
#endif
          break;
        } else {
          if (j == retry - 1) {
#if PRINT_ANG
            ang[i] = -0.0;
#endif
            printf("%02zd[%02d] :    error\n", i + 1, RSX_SPKT_GETID(*rpkt));
          }
          continue;
        }
      }
    }
  }

#if PRINT_ANG
  for (size_t i = 0; i < num_of_axis; i++) {
    printf("%+8.3f\n", ang[i]);
  }
  printf("\033[%zdA", num_of_axis);
#endif

  return EOK;
}

int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial) {
  EVALUE(NULL, hrs);

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

  const size_t num_of_servo = 20;
  RSX_LPKT_DECL(pkt, num_of_servo, 10);
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

  usleep(500 * 1000);

  ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));

#if 0
  int pose[20][20] = {
    /*        1     2  |   3     4     5  |   6     7     8  |   9     10    11     12     13    14  |  15     16    17     18     19    20*/
    /* 1*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   0,  0, 0,  0,  0, 0,  0,  0, 0},
    /* 2*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  1, 0,-  2,- 1, 0,  0,- 1, 0,  2,  1, 0},
    /* 3*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  3, 0,-  6,- 3, 0,  0,- 3, 0,  6,  3, 0},
    /* 4*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 12,- 6, 0,  0,- 6, 0, 12,  6, 0},
    /* 5*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 10, 0,- 20,-10, 0,  0,-10, 0, 20, 10, 0},
    /* 6*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 15, 0,- 30,-15, 0,  0,-15, 0, 30, 15, 0},
    /* 7*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 19, 0,- 38,-19, 0,  0,-19, 0, 38, 19, 0},
    /* 8*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 22, 0,- 44,-22, 0,  0,-22, 0, 44, 22, 0},
    /* 9*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 24, 0,- 48,-24, 0,  0,-24, 0, 48, 24, 0},
    /*10*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 25, 0,- 50,-25, 0,  0,-25, 0, 50, 25, 0},
    /*10*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 25, 0,- 50,-25, 0,  0,-25, 0, 50, 25, 0},
    /* 9*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 24, 0,- 48,-24, 0,  0,-24, 0, 48, 24, 0},
    /* 8*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 22, 0,- 44,-22, 0,  0,-22, 0, 44, 22, 0},
    /* 7*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 19, 0,- 38,-19, 0,  0,-19, 0, 38, 19, 0},
    /* 6*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 15, 0,- 30,-15, 0,  0,-15, 0, 30, 15, 0},
    /* 5*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 10, 0,- 20,-10, 0,  0,-10, 0, 20, 10, 0},
    /* 4*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 12,- 6, 0,  0,- 6, 0, 12,  6, 0},
    /* 3*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  3, 0,-  6,- 3, 0,  0,- 3, 0,  6,  3, 0},
    /* 2*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  1, 0,-  2,- 1, 0,  0,- 1, 0,  2,  1, 0},
    /* 1*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   0,  0, 0,  0,  0, 0,  0,  0, 0},
  };
#endif
#if 0
  int pose[20][20] = {
    /*      1  2 | 3  4  5 | 6  7  8 | 9  10 11   12  13 14 |15  16 17  18  19 20*/
    /* 1*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   4,  0, 0,  0,  0, 0,  4,  0, 0},
    /* 2*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  2, 0,-  4,- 2, 0,  0,- 2, 0,  4,  2, 0},
    /* 3*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 11,- 6, 0,  0,- 6, 0, 11,  6, 0},
    /* 4*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 12, 0,- 22,-12, 0,  0,-12, 0, 22, 12, 0},
    /* 5*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 20, 0,- 36,-20, 0,  0,-20, 0, 36, 20, 0},
    /* 6*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 30, 0,- 54,-30, 0,  0,-30, 0, 54, 30, 0},
    /* 7*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 38, 0,- 68,-38, 0,  0,-38, 0, 68, 38, 0},
    /* 8*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 44, 0,- 78,-44, 0,  0,-44, 0, 78, 44, 0},
    /* 9*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 48, 0,- 86,-48, 0,  0,-48, 0, 86, 48, 0},
    /*10*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 50, 0,- 90,-50, 0,  0,-50, 0, 90, 50, 0},
    /*10*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 50, 0,- 90,-50, 0,  0,-50, 0, 90, 50, 0},
    /* 9*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 48, 0,- 86,-48, 0,  0,-48, 0, 86, 48, 0},
    /* 8*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 44, 0,- 78,-44, 0,  0,-44, 0, 78, 44, 0},
    /* 7*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 38, 0,- 68,-38, 0,  0,-38, 0, 68, 38, 0},
    /* 6*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 30, 0,- 54,-30, 0,  0,-30, 0, 54, 30, 0},
    /* 5*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 20, 0,- 36,-20, 0,  0,-20, 0, 36, 20, 0},
    /* 4*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 12, 0,- 22,-12, 0,  0,-12, 0, 22, 12, 0},
    /* 3*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  6, 0,- 11,- 6, 0,  0,- 6, 0, 11,  6, 0},
    /* 2*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  2, 0,-  4,- 2, 0,  0,- 2, 0,  4,  2, 0},
    /* 1*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   4,  0, 0,  0,  0, 0,  4,  0, 0},
  };
#endif
#if 0
  int pose[2][20] = {
    /*      1  2 | 3  4  5 | 6  7  8 | 9  10 11   12  13 14 |15  16 17  18  19 20*/
    /* 1*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0,  0, 0,   4,  0, 0,  0,  0, 0,  4,  0, 0},
    /*10*/{ 0, 0,  0, 0, 0,  0, 0, 0,  0, 50, 0,- 90,-50, 0,  0,-50, 0, 90, 50, 0},
  };
#endif

  for (size_t idx = 0; idx < 20; idx++) {
#if defined(HR_SERIAL_LATENCY_CHECK)
    hr_time tm_bef, tm_aft, tm_diff;
    ECALL(hr_get_time(&tm_bef));
#endif

#ifdef ENABLE_SERVO_DRIVE
    RSX_LPKT_SETADDR(pkt, 0x1e);
    RSX_LPKT_SETLENGTH(pkt, 0x02);
    for (size_t i = 0; i < 20; i++) {
      RSX_LPKT_SETID(pkt, i, i + 1);
      RSX_LPKT_SET_INT16(pkt, i, 0, (int)(pose[idx][i] * 10));
    }

    ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
#endif

    ECALL(get_current(hrs, &rpkt, buff, sizeof(buff), use_serial));
 
#if defined(HR_SERIAL_LATENCY_CHECK)
    ECALL(hr_get_time(&tm_aft));
    ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

    printf("     write read latency : ");
    ECALL(hr_dump_time(&tm_diff));
#endif

    usleep(200 * 1000);

  }

#ifdef PRINT_ANG
  printf("\033[20B");
#endif

  usleep(10 * 1000);

  /* servo off */
  RSX_LPKT_DECL(pkt2, num_of_servo, 2);
  RSX_LPKT_INIT(pkt2);
  RSX_LPKT_SETADDR(pkt2, 0x24);
  RSX_LPKT_SETLENGTH(pkt2, 0x01);
  for (size_t i = 0; i < 20; i++) {
    RSX_LPKT_SETVID(pkt2, i, i + 1);
    RSX_LPKT_SET_U8(pkt2, i, 0, 0x00);
  }
  ECALL(rsx_pkt_ser(&pkt2, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

  printf("----- end ----- \n");

  usleep(10 * 1000);

  return 0;
}

#include <sched.h>

int main(int argc, char *argv[]) {
#ifdef ENABLE_SHED
  int eno;
  struct sched_param sp;
  //int policy = SCHED_FIFO;
  int policy = SCHED_RR;
  //eno = sched_getparam(0, &sp);
  //printf("--> %d\n", eno);
  int priority_max = sched_get_priority_max(policy);
  printf("--> %d\n", priority_max);
  sp.sched_priority = priority_max; // MAX
  eno = sched_setscheduler(0, policy, &sp);
  printf("--> %d\n", eno);
#endif

  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  bool use_serial = false;

  if (argc >= 3) {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, argv[1], argv[2], HR_B460800, HR_PAR_NONE));
  } else {
    //use_serial = true;
    //ECALL(hr_serial_open(hrs, "ttyUSB", "0"));
  }

  run_test(argc, argv, &hrs, use_serial);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}
