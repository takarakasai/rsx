
#ifndef DPSERVO_H
#define DPSERVO_H

#include "dp_type.h"
#include "dp_err.h"

#include "dpservo.h"

#include "serial/hr_serial.h"

#define DPSERVO_DECL(name, num_of_servo, max_data_size, PROTOCOL_DECL_MACRO) \
  PROTOCOL_DECL_MACRO(name, num_of_servo);                    \
  hr_serial name ## _hrs;                                     \
  uint8_t name ## _buff [max_data_size];                      \

#define DPSERVO_INIT(name, PROTOCOL_INIT_MACRO)         \
  PROTOCOL_INIT_MACRO(name);                            \
  dpservo_init(                                         \
   get_dpservo(&name), &(name ## _hrs),                 \
   sizeof(name ## _buff) / sizeof(name ## _buff[0]),    \
   (name ## _buff))

typedef enum {
  kDpsServoOff,
  kDpsServoBrk,
  kDpsServoOn
} dps_servo_state;

typedef struct dpservo_struct dpservo;

typedef errno_t (*dps_set_state_op)(dpservo *dps, const uint8_t id, dps_servo_state state);
typedef errno_t (*dps_set_states_op)(dpservo *dps, dps_servo_state state);
typedef errno_t (*dps_set_goal_op)(dpservo *dps, const uint8_t id, float64_t goal);
typedef errno_t (*dps_set_goals_op)(dpservo *dps, const size_t num, float64_t goal[/*num*/]);
typedef errno_t (*dps_write_mem_op)(dpservo *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]);
typedef errno_t (*dps_read_mem_op)(dpservo *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]);

typedef struct {
  dps_set_state_op set_state;
  dps_set_states_op set_states;
  dps_set_goal_op set_goal;
  dps_set_goals_op set_goals;
  dps_write_mem_op write_mem;
  dps_read_mem_op read_mem;
} dpservo_ops;

inline errno_t dpservo_ops_init (
        dpservo_ops *ops,
        dps_set_state_op set_state,
        dps_set_states_op set_states,
        dps_set_goal_op set_goal,
        dps_set_goals_op set_goals,
        dps_write_mem_op write_mem,
        dps_read_mem_op read_mem)
{
    EVALUE(NULL, ops);

    EVALUE(NULL, set_state);
    EVALUE(NULL, set_states);
    EVALUE(NULL, set_goal);
    EVALUE(NULL, set_goals);
    EVALUE(NULL, write_mem);
    EVALUE(NULL, read_mem);

    ops->set_state  = set_state;
    ops->set_states = set_states;
    ops->set_goal   = set_goal;
    ops->set_goals  = set_goals;
    ops->write_mem  = write_mem;
    ops->read_mem   = read_mem;

    return EOK;
}

typedef struct dpservo_struct {
  hr_serial *hrs;

  size_t max_size;
  uint8_t *buff/*[max_size]*/;

  size_t num_of_servo;         /* number of servo to be used. */
  uint8_t *servo_ids/*[num_of_servo]*/;

  /* debug */
  bool io_enabled;

  /* interfaces */
  dpservo_ops ops;
} dpservo;

static inline dpservo* get_dpservo(void* child) {
  return (dpservo*)child;
}

static inline errno_t dpservo_init (dpservo *dps, hr_serial *hrs, size_t max_size, uint8_t buff[/*max_size*/]) {
  EVALUE(NULL, dps);
  EVALUE(NULL, hrs);
  EVALUE(NULL, buff);

  ECALL(hr_serial_init(hrs));

  dps->hrs = hrs;
  dps->buff = buff;
  dps->max_size = max_size;

  //dps->retry_count = 5;

  dps->io_enabled = true;

  return EOK;
}

static inline errno_t dps_set_state (dpservo *dps, const uint8_t id, dps_servo_state state) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_state(dps, id, state));
  return EOK;
}

static inline errno_t dps_set_states (dpservo *dps, dps_servo_state state) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_states(dps, state));
  return EOK;
}

static inline errno_t dps_set_goal (dpservo *dps, const uint8_t id, float64_t goal) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_goal(dps, id, goal));
  return EOK;
}

static inline errno_t dps_set_goals (dpservo *dps, const size_t num, float64_t goal[/*num*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_goals(dps, num, goal));
  return EOK;
}

static inline errno_t dps_write_mem (dpservo *dps, const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.write_mem(dps, id, start_addr, size, data));
  return EOK;
}

static inline errno_t dps_read_mem (dpservo *dps, const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.read_mem(dps, id, start_addr, size, data));
  return EOK;
}

#endif

