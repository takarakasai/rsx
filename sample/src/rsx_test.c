
#include "helper.h"

/* for printf */
#include <stdio.h>

#include <string.h>
/* for strtoul */
#include <stdlib.h>

#include "rsx/rsx.h"

#include "time/hr_unixtime.h"

errno_t print_help (int argc, char *argv[]) {
  printf(" %s device port high|mid|low id|255 min max\n", argv[0]);
  return EOK;
}

/*
 * ./build/sample/ics_test ttyUSB 0 low 255 -30 30 
 */

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, RSX_DECL);
  DPSERVO_INIT(servo, RSX_INIT);
  
  uint8_t id = 1;
  float64_t max = +90.0;
  float64_t mid = 0.0;;
  float64_t min = -90.0;

  int argc_offset;
  if (EOK != dps_setup_rsx(servo, argc, argv, &argc_offset)) {
    print_help(argc, argv);
  }

  if (argc >= 1 + argc_offset) {
    id = (uint8_t)strtoul(argv[argc_offset], NULL, 10);
  }

  if (argc >= 3 + argc_offset) {
    min = (float64_t)strtod(argv[1 + argc_offset], NULL);
    max = (float64_t)strtod(argv[2 + argc_offset], NULL);
    mid = (min + max) / 2.0;
  }

  printf("===: ID: %d %d\n", argc,  argc_offset);
  printf("===: ID: %d\n", id);
  printf("===: %lf < %lf < %lf\n", min, mid, max);

  if (id == 0xFF) {
    /* whest */
    ECALL(dps_add_servo(servo,  1)); ECALL(dps_set_offset_angle(servo,  1,   0.0)); ECALL(dps_set_direction(servo,  1, DPS_SERVO_DIR_POSITIVE)); 
    /* head */
    ECALL(dps_add_servo(servo,  2)); ECALL(dps_set_offset_angle(servo,  2,   0.0)); ECALL(dps_set_direction(servo,  2, DPS_SERVO_DIR_POSITIVE)); 

    /* right  arm */
    ECALL(dps_add_servo(servo,  3)); ECALL(dps_set_offset_angle(servo,  3,   0.0)); ECALL(dps_set_direction(servo,  3, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo,  4)); ECALL(dps_set_offset_angle(servo,  4,   0.0)); ECALL(dps_set_direction(servo,  4, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo,  5)); ECALL(dps_set_offset_angle(servo,  5,   0.0)); ECALL(dps_set_direction(servo,  5, DPS_SERVO_DIR_POSITIVE));
    /* left  arm */
    ECALL(dps_add_servo(servo,  6)); ECALL(dps_set_offset_angle(servo,  6,   0.0)); ECALL(dps_set_direction(servo,  6, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo,  7)); ECALL(dps_set_offset_angle(servo,  7,   0.0)); ECALL(dps_set_direction(servo,  7, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo,  8)); ECALL(dps_set_offset_angle(servo,  8,   0.0)); ECALL(dps_set_direction(servo,  8, DPS_SERVO_DIR_POSITIVE));

    /* right legs */
    ECALL(dps_add_servo(servo,  9)); ECALL(dps_set_offset_angle(servo,  9,   0.0)); ECALL(dps_set_direction(servo,  9, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 10)); ECALL(dps_set_offset_angle(servo, 10,   0.0)); ECALL(dps_set_direction(servo, 10, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 11)); ECALL(dps_set_offset_angle(servo, 11,   0.0)); ECALL(dps_set_direction(servo, 11, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 12)); ECALL(dps_set_offset_angle(servo, 12,   0.0)); ECALL(dps_set_direction(servo, 12, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 13)); ECALL(dps_set_offset_angle(servo, 13,   0.0)); ECALL(dps_set_direction(servo, 13, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 14)); ECALL(dps_set_offset_angle(servo, 14,   0.0)); ECALL(dps_set_direction(servo, 14, DPS_SERVO_DIR_POSITIVE));
    /* left legs */
    ECALL(dps_add_servo(servo, 15)); ECALL(dps_set_offset_angle(servo, 15,   0.0)); ECALL(dps_set_direction(servo, 15, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 16)); ECALL(dps_set_offset_angle(servo, 16,   0.0)); ECALL(dps_set_direction(servo, 16, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 17)); ECALL(dps_set_offset_angle(servo, 17,   0.0)); ECALL(dps_set_direction(servo, 17, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 18)); ECALL(dps_set_offset_angle(servo, 18,   0.0)); ECALL(dps_set_direction(servo, 18, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 19)); ECALL(dps_set_offset_angle(servo, 19,   0.0)); ECALL(dps_set_direction(servo, 19, DPS_SERVO_DIR_POSITIVE));
    ECALL(dps_add_servo(servo, 20)); ECALL(dps_set_offset_angle(servo, 20,   0.0)); ECALL(dps_set_direction(servo, 20, DPS_SERVO_DIR_POSITIVE));

    printf("===: num of servo: %d\n", dps_get_num_of_servo(servo));
    //ECALL(dps_set_goas
 
    const size_t num = dps_get_num_of_servo(servo);
    float64_t min_vec[num];
    float64_t mid_vec[num];
    float64_t max_vec[num];

    for (size_t i = 0; i < num; i++) {
      min_vec[i] = min;
      mid_vec[i] = mid;
      max_vec[i] = max;
    }
  
    hr_usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, id, mid_vec));
  
    hr_usleep(1000 * 1000);
    printf("--> max\n");
    (dps_set_goals(servo, id, max_vec));
  
    hr_usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, id, mid_vec));
  
    hr_usleep(1000 * 1000);
    printf("--> min\n");
    (dps_set_goals(servo, id, min_vec));
  
    hr_usleep(1000 * 1000);
    printf("--> 0.0\n");
    (dps_set_goals(servo, id, mid_vec));
  
    hr_usleep(1000 * 1000);
    printf("--> SERVO OFF\n");
    (dps_set_states(servo, kDpsServoOff));

  } else {
    printf("--> servo on\n");
    dps_set_state(servo, id, kDpsServoOn);
  
    hr_usleep(1000 * 1000);
    printf("--> %lf\n", mid);
    dps_set_goal(servo, id, mid);

    hr_usleep(1000 * 1000);
    printf("--> max(%lf)\n", max);
    dps_set_goal(servo, id, max);

    hr_usleep(1000 * 1000);
    printf("--> %lf\n", mid);
    dps_set_goal(servo, id, mid);

    hr_usleep(1000 * 1000);
    printf("--> min(%lf)\n", min);
    dps_set_goal(servo, id, min);

    hr_usleep(1000 * 1000);
    printf("--> %lf\n", mid);
    dps_set_goal(servo, id, mid);

    hr_usleep(1000 * 1000);
    printf("--> SERVO OFF\n");
    dps_set_state(servo, id, kDpsServoOff);
  }

  ECALL(dps_teardown(servo, argc, argv));

  return 0;
}

