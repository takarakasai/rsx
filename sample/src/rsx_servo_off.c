
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>

#include "rsx_pkt.h"
#include "rsx_io.h"

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif

#include <stdbool.h>

static int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial, uint8_t servo_state) {
  EVALUE(NULL, hrs);

  printf(" ---> %x\n", servo_state);

  //size_t count = 0;
  uint8_t buff[1024];
  size_t size;

  const size_t num_of_servo = 20;
  RSX_LPKT_DECL(pkt, num_of_servo, 20);
  RSX_LPKT_INIT(pkt);
  RSX_LPKT_SETADDR(pkt, 0x24);
  RSX_LPKT_SETLENGTH(pkt, 0x01);
  for (size_t i = 0; i < 20; i++) {
    RSX_LPKT_SETVID(pkt, i, i + 1);
    RSX_LPKT_SET_U8(pkt, i, 0, servo_state);
    //RSX_LPKT_SET_U8(pkt, i, 0, RSX_DATA_SERVO_OFF); // servo off
    //RSX_LPKT_SET_U8(pkt, i, 0, RSX_DATA_SERVO_ON); // servo on
    //RSX_LPKT_SET_U8(pkt, i, 0, RSX_DATA_SERVO_BRK); // servo break
  }
  ECALL(rsx_pkt_ser(&pkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if(use_serial) ECALL(hr_serial_write(hrs, buff, size));

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

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
    usleep(10 * 1000);
    if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 1)); // TODO: size + 2
    ECALL(data_dump(buff, size + 1));                           // TODO: size + 2
    ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

    printf(" |%02x", RSX_SPKT_GET_U8(rpkt, 0));

    usleep(20 * 1000);
  }
  printf("\n");

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  bool use_serial = false;
  uint8_t servo_state = RSX_DATA_SERVO_OFF;

  if (argc >= 4) {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, argv[1], argv[2]));
    if (strcmp(argv[3], "ON") == 0) {
      servo_state = RSX_DATA_SERVO_ON;
    } else if (strcmp(argv[3], "BRK" ) == 0) {
      servo_state = RSX_DATA_SERVO_BRK;
    } else {
      servo_state = RSX_DATA_SERVO_OFF;
    }
  } else {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, "ttyUSB", "0"));
  }

  run_test(argc, argv, &hrs, use_serial, servo_state);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}

