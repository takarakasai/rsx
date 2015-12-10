
#include "rsx.h"
#include "mmap/rs30x.h"

errno_t rs30x_chg_servo_state(rsx *servo, uint8_t id, uint8_t num_of_joints, uint8_t servo_state) {
  ECALL(rsx_spkt_mem_write(servo, id, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));
  return EOK;
}

errno_t rs30x_chg_servo_state_all(rsx *servo, uint8_t num_of_joints, uint8_t servo_state) {
  ECALL(rsx_spkt_mem_write(servo, RSX_PKT_ID_ALL, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));
  return EOK;
}

errno_t rs30x_chg_servo_state_lall(rsx *servo, uint8_t start_joint_id, uint8_t num_of_joints, uint8_t servo_state) {
  EVALUE(NULL, servo);

  uint8_t id[num_of_joints];
  for (size_t i = 0; i < num_of_joints; i++) {
    id[i] = i + start_joint_id;
  }

  ECALL(rsx_lpkt_mem_write_all(servo, id, num_of_joints, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));

  return EOK;
}

errno_t rs30x_set_servo_goal(rsx *servo, uint8_t start_joint_id, uint8_t num_of_joints, int16_t goal[/*num_of_joints*/] ) {
  EVALUE(NULL, servo);

  uint8_t id[num_of_joints];
  for (size_t i = 0; i < num_of_joints; i++) {
    id[i] = i + start_joint_id;
  }

  ECALL(rsx_lpkt_mem_write_int16(servo, id, num_of_joints, RSX_RAM_GOAL_POS_L, 1, goal));
  return EOK;
}

errno_t rs30x_set_servo_goal_and_time(rsx *servo, uint8_t start_joint_id, uint8_t num_of_joints, int16_t goal[/*num_of_joints*/][2]) {
  EVALUE(NULL, servo);

  uint8_t id[num_of_joints];
  for (size_t i = 0; i < num_of_joints; i++) {
    id[i] = i + start_joint_id;
  }

  ECALL(rsx_lpkt_mem_write_int16(servo, id, num_of_joints, RSX_RAM_GOAL_POS_L, 2, goal));
  return EOK;
}

//#define RSX_RAM_GOAL_POS_L    0x1E
//#define RSX_RAM_GOAL_POS_H    0x1F
//#define RSX_RAM_GOAL_TIM_L    0x20
//#define RSX_RAM_GOAL_TIM_H    0x21
//#define RSX_RAM_MAX_TORQUE    0x23
//#define RSX_RAM_TRQ_ENABLE    0x24

