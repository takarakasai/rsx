
#include "helper.h"

/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>
/* for strtoul */
#include <stdlib.h>

#include "ics/ics.h"

errno_t print_help (int argc, char *argv[]) {
  printf(" %s device port high|mid|low id|255 min max\n", argv[0]);
  return EOK;
}

/*
 * ./build/sample/ics_test ttyUSB 0 low 255 -30 30 
 */

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  
  uint8_t id = 1;
  float64_t max = +90.0;
  float64_t min = -90.0;

  int argc_offset;
  if (EOK != dps_setup(servo, argc, argv, &argc_offset)) {
    print_help(argc, argv);
  }

  if (argc >= 3 + argc_offset) {
    id = (uint8_t)strtoul(argv[argc_offset], NULL, 10);
    min = (float64_t)strtod(argv[1 + argc_offset], NULL);
    max = (float64_t)strtod(argv[2 + argc_offset], NULL);
  }

  printf("===: ID: %d\n", id);

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
    (dps_set_goals(servo, id, zero_vec));
  
    usleep(1000 * 1000);
    printf("--> max\n");
    (dps_set_goals(servo, id, max_vec));
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, id, zero_vec));
  
    usleep(1000 * 1000);
    printf("--> min\n");
    (dps_set_goals(servo, id, min_vec));
  
    usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, id, zero_vec));
  
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

  ECALL(dps_teardown(servo, argc, argv));

  return 0;
}

