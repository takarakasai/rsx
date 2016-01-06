
/* for printf */
#include <stdio.h>

#include <string.h>

/* for datadump */
#include "dpservo.h"

#include "time/hr_unixtime.h"

#include "rsx/rsx.h"
#include "rsx/mmap/rs30x.h"

static int run_test(int argc, char *argv[], rsx *servo, uint8_t servo_state) {
  EVALUE(NULL, servo);

  printf(" ---> %x\n", servo_state);

  const uint8_t num_of_joints = 20;
  uint8_t id[num_of_joints];
  for (size_t i = 0; i < num_of_joints; i++) {
    id[i] = i + 1;
  }

  ECALL(rsx_lpkt_mem_write_all(servo, id, num_of_joints, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));

  uint8_t data[num_of_joints][1];
  for (size_t i = 0; i < num_of_joints; i++) {
    hr_usleep(20 * 1000);
    ECALL(rsx_spkt_mem_read(servo, id[i], RSX_RAM_TRQ_ENABLE, 1, data[i]));
  }
 
  printf("             : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02zd",      i + 1);} printf("\n");
  printf("SERVO STATUS : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02x" , data[i][0]);} printf("\n");

  return 0;
}

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, RSX_DECL);
  DPSERVO_INIT(servo, RSX_INIT);
  //RSX_DECL(servo, 20, 1024);
  //RSX_INIT(servo);

  rsx *x = (rsx*)(&servo);

  bool use_serial = false;
  uint8_t servo_state = RSX_DATA_SERVO_OFF;

  printf("argc: %d\n", argc);

  if (argc >= 3) {
    use_serial = true;
    ECALL(rsx_open(x, argv[1], argv[2], HR_B460800, HR_PAR_NONE));

    if (strcmp(argv[3], "ON") == 0) {
      servo_state = RSX_DATA_SERVO_ON;
    } else if (strcmp(argv[3], "BRK" ) == 0) {
      servo_state = RSX_DATA_SERVO_BRK;
    } else {
      servo_state = RSX_DATA_SERVO_OFF;
    }

  } else {
    use_serial = true;
    ECALL(rsx_open(x, "ttyUSB", "0", HR_B460800, HR_PAR_NONE));
  }

  ECALL(rsx_set_serial(x, use_serial));

  run_test(argc, argv, x, servo_state);

  ECALL(rsx_close(x));

  return 0;
}

