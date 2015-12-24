
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>
/* for strtoul */
#include <stdlib.h>

#include "ics/ics.h"

/*
 * ./build/sample/ics_test ttyUSB 0 low 255 -30 30 
 */

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  
  bool use_serial = true;
  uint8_t id = 1;

  hr_baudrate baudrate = HR_B115200;

  char *device = "ttyUSB";
  char *port   = "0";
  float64_t max = +90.0;
  float64_t min = -90.0;

  if (argc >= 7) {
    device = argv[1];
    port   = argv[2];

    if (strcmp(argv[3], "high") == 0) {
      baudrate = HR_B1250000;
    } else if(strcmp(argv[3], "mid") == 0) {
      baudrate =  HR_B625000;
    } else {
      baudrate =  HR_B115200;
    }

    id = (uint8_t)strtoul(argv[4], NULL, 10);

    min = (float64_t)strtod(argv[5], NULL);
    max = (float64_t)strtod(argv[6], NULL);
  }

  printf("===: %s/%s with %s(%s)\n", device, port, hr_baudrate2str(baudrate), argv[3]);
  printf("===: ID: %d\n", id);
  ECALL(dps_open(servo, device, port, baudrate, HR_PAR_EVEN));

  ECALL(dps_set_serial(servo, use_serial));

  if (id == 0xFF) {
    ECALL(dps_add_servo(servo,  1));
    ECALL(dps_add_servo(servo,  2));
    /* left/right legs */
    ECALL(dps_add_servo(servo,  6));
    ECALL(dps_add_servo(servo,  7));
    ECALL(dps_add_servo(servo,  8));
    ECALL(dps_add_servo(servo,  9));
    ECALL(dps_add_servo(servo, 10));
    printf("===: num of servo: %d\n", dps_get_num_of_servo(servo));
    //ECALL(dps_set_goas
 
    const size_t num = dps_get_num_of_servo(servo);
    float64_t min_vec[num];
    float64_t zero_vec[num];
    float64_t max_vec[num];

    for (size_t i = 0; i < num; i++) {
      min_vec[i] = min;
      zero_vec[i] = 0.0;
      max_vec[i] = max;
    }
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, 2, zero_vec));
  
    usleep(1000 * 1000);
    printf("--> max\n");
    (dps_set_goals(servo, 2, max_vec));
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, 2, zero_vec));
  
    usleep(1000 * 1000);
    printf("--> min\n");
    (dps_set_goals(servo, 2, min_vec));
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, 2, zero_vec));
  
    usleep(1000 * 1000);
    printf("--> SERVO OFF\n");
    (dps_set_states(servo, kDpsServoOff));

  } else {
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    dps_set_goal(servo, id, 0.0);

    usleep(1000 * 1000);
    printf("--> max\n");
    dps_set_goal(servo, id, max);

    usleep(1000 * 1000);
    printf("--> 0.0\n");
    dps_set_goal(servo, id, 0.0);

    usleep(1000 * 1000);
    printf("--> min\n");
    dps_set_goal(servo, id, min);

    usleep(1000 * 1000);
    printf("--> 0.0\n");
    dps_set_goal(servo, id, 0.0);

    usleep(1000 * 1000);
    printf("--> SERVO OFF\n");
    dps_set_state(servo, id, kDpsServoOff);
  }

  ECALL(dps_close(servo));

  return 0;
}

