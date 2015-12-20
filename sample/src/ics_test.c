
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>

#include "ics.h"

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  
  bool use_serial = false;

  if (argc >= 4) {
    use_serial = true;
    //ECALL(dps_open(servo, argv[1], argv[2], HR_B460800, HR_PAR_NONE));
    if (strcmp(argv[3], "high") == 0) {
      ECALL(dps_open(servo, argv[1], argv[2], HR_B1152000, HR_PAR_EVEN));
    } else {
      ECALL(dps_open(servo, argv[1], argv[2], HR_B115200, HR_PAR_EVEN));
    }
  } else {
    use_serial = true;
    ECALL(dps_open(servo, "ttyUSB", "0", HR_B115200, HR_PAR_EVEN));
  }

  ECALL(dps_set_serial(servo, use_serial));

  uint8_t id = 2;
  usleep(1000 * 1000);
  dps_set_goal(servo, id, 0.0);
  usleep(2500 * 1000);
  dps_set_goal(servo, id, 90.0);
  //dps_set_goal(servo, id, 135.0);
  usleep(2500 * 1000);
  dps_set_goal(servo, id, 0.0);
  usleep(2500 * 1000);
  dps_set_goal(servo, id, -90.0);
  //dps_set_goal(servo, id, -135.0);
  usleep(2500 * 1000);
  dps_set_goal(servo, id, 0.0);
  usleep(2500 * 1000);
  dps_set_state(servo, id, kDpsServoOff);

  ECALL(dps_close(servo));

  return 0;
}

