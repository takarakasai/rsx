
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>

//#include "rsx.h"
#include "ics.h"
#include "mmap/rs30x.h"

/*
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
    usleep(20 * 1000);
    ECALL(rsx_spkt_mem_read(servo, id[i], RSX_RAM_TRQ_ENABLE, 1, data[i]));
  }
 
  printf("             : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02zd",      i + 1);} printf("\n");
  printf("SERVO STATUS : "); for (size_t i = 0; i < num_of_joints; i++) { printf(" |%02x" , data[i][0]);} printf("\n");

  return 0;
}
*/

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  //RSX_DECL(servo, 20, 1024);
  //RSX_INIT(servo);
  
  bool use_serial = false;

  if (argc >= 3) {
    use_serial = true;
    //ECALL(dps_open(servo, argv[1], argv[2], HR_B460800, HR_PAR_NONE));
    ECALL(dps_open(servo, argv[1], argv[2], HR_B115200, HR_PAR_EVEN));
  } else {
    use_serial = true;
    ECALL(dps_open(servo, "ttyUSB", "0", HR_B1152000, HR_PAR_EVEN));
  }

  ECALL(dps_set_serial(servo, use_serial));

  ics *x = (ics*)(servo);
  ics_test(x);
  //run_test(argc, argv, servo, servo_state);

  ECALL(dps_close(servo));

  return 0;
}

