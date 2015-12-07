
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>

#include "rsx.h"

static int run_test(int argc, char *argv[], rsx *servo, uint8_t servo_state) {
  EVALUE(NULL, servo);

  printf(" ---> %x\n", servo_state);

  const uint8_t num_of_joints = 20;
  uint8_t id[num_of_joints];
  for (size_t i = 0; i < num_of_joints; i++) {
    id[i] = i + 1;
  }

  ECALL(rsx_lpkt_mem_write_all(servo, id, num_of_joints, 0x24, 1, ((uint8_t[]){servo_state})));

  uint8_t data[num_of_joints][1];
  for (size_t i = 0; i < num_of_joints; i++) {
    usleep(20 * 1000);
    ECALL(rsx_spkt_mem_read(servo, id[i], 0x24, 1, data[i]));
  }
 
  printf("             : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02zd",      i + 1);} printf("\n");
  printf("SERVO STATUS : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02x" , data[i][0]);} printf("\n");

  return 0;
}

int main(int argc, char *argv[]) {
  RSX_DECL(servo, 20, 1024);
  RSX_INIT(servo);

  bool use_serial = false;
  uint8_t servo_state = RSX_DATA_SERVO_OFF;

  if (argc >= 4) {
    use_serial = true;
    ECALL(rsx_open(&servo, argv[1], argv[2]));

    if (strcmp(argv[3], "ON") == 0) {
      servo_state = RSX_DATA_SERVO_ON;
    } else if (strcmp(argv[3], "BRK" ) == 0) {
      servo_state = RSX_DATA_SERVO_BRK;
    } else {
      servo_state = RSX_DATA_SERVO_OFF;
    }

  } else {
    use_serial = true;
    ECALL(rsx_open(&servo, "ttyUSB", "0"));
  }

  ECALL(rsx_set_serial(&servo, use_serial));

  run_test(argc, argv, &servo, servo_state);

  ECALL(rsx_close(&servo));

  return 0;
}

