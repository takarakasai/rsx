
#ifndef DPSERVO_H
#define DPSERVO_H

#include "dp_type.h"
#include "dp_err.h"

#include "dpservo.h"

#include "serial/hr_serial.h"

#define DPS_SERVO_ID_INVALID 0xFF

/*
#define DPSERVO_DECL(name, num_of_servo, max_data_size, PROTOCOL_DECL_MACRO) \
  struct dps_struct {                                                           \
    PROTOCOL_DECL_MACRO(child, num_of_servo);                       \
    HR_SERIAL_DECL(hrs);                                            \
    uint8_t ids  [num_of_servo];                                    \
    uint8_t buff [max_data_size];                                   \
  }name

#define DPSERVO_INIT(name, PROTOCOL_INIT_MACRO)     \
  HR_SERIAL_INIT(name.hrs);                         \
  dpservo_init(                                     \
   get_dpservo_base(&(name.child)), &(name.hrs),    \
   sizeof(name.ids) / sizeof(name.ids[0]),      \
   (name.ids),                                  \
   sizeof(name.buff) / sizeof(name.buff[0]),    \
   (name.buff));                       \
  PROTOCOL_INIT_MACRO(name.child)
*/

#define DPSERVO_DECL(name, num_of_servo, max_data_size, PROTOCOL_DECL_MACRO) \
    PROTOCOL_DECL_MACRO(name ## _child, num_of_servo);                       \
    HR_SERIAL_DECL(name ## _hrs);                                            \
    uint8_t name ## _ids  [num_of_servo];                                    \
    uint8_t name ## _buff [max_data_size];                                   \
    dpservo_base *name

#define DPSERVO_INIT(name, PROTOCOL_INIT_MACRO)         \
  HR_SERIAL_INIT((name ## _hrs));                       \
  dpservo_init(                                         \
   get_dpservo_base(&name ## _child), &(name ## _hrs),  \
   sizeof(name ## _ids) / sizeof(name ## _ids[0]),      \
   (name ## _ids),                                      \
   sizeof(name ## _buff) / sizeof(name ## _buff[0]),    \
   (name ## _buff), &(name));                           \
  PROTOCOL_INIT_MACRO(name ## _child)

typedef enum {
  kDpsServoOff,
  kDpsServoBrk,
  kDpsServoOn
} dps_servo_state;

typedef struct dps_struct dps;
typedef struct dpservo_base_struct dpservo_base;

typedef errno_t (*dps_set_state_op)(dpservo_base *dps, const uint8_t id, dps_servo_state state);
typedef errno_t (*dps_set_states_op)(dpservo_base *dps, dps_servo_state state);
typedef errno_t (*dps_set_goal_op)(dpservo_base *dps, const uint8_t id, float64_t goal);
typedef errno_t (*dps_set_goals_op)(dpservo_base *dps, const size_t num, float64_t goal[/*num*/]);
typedef errno_t (*dps_write_mem_op)(dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]);
typedef errno_t (*dps_read_mem_op)(dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]);

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

typedef struct dpservo_base_struct {
  hr_serial *hrs;

  size_t max_size;
  uint8_t *buff/*[max_size]*/;

  size_t max_num_of_servo;     /* max number of servo */
  size_t num_of_servo;         /* number of servo to be used. */
  uint8_t *servo_ids/*[num_of_servo]*/;

  /* debug */
  bool io_enabled;

  /* interfaces */
  dpservo_ops ops;
} dpservo_base;

static inline dpservo_base* get_dpservo_base(void* child) {
  return (dpservo_base*)child;
}

static inline errno_t dpservo_init (
        dpservo_base *dps, hr_serial *hrs,
        size_t num_of_servo, uint8_t ids[/*num_of_servo*/],
        //size_t max_size, uint8_t buff[/*max_size*/])
        size_t max_size, uint8_t buff[/*max_size*/], dpservo_base **p_dps)
{
  EVALUE(NULL, dps);
  EVALUE(NULL, hrs);
  EVALUE(NULL, ids);
  EVALUE(NULL, buff);
  EVALUE(NULL, p_dps);

  //ECALL(hr_serial_init(hrs));

  dps->hrs = hrs;

  dps->servo_ids = ids;
  dps->max_num_of_servo = num_of_servo;
  dps->num_of_servo     = 0;
  for (uint8_t i = 0; i < dps->max_num_of_servo; i++) {
    /* [1, 2, 3, ..., num_of_servo] */
    dps->servo_ids[i] = DPS_SERVO_ID_INVALID; /* 0xFF */
  }

  dps->buff = buff;
  dps->max_size = max_size;

  //dps->retry_count = 5;

  dps->io_enabled = true;

  *p_dps = dps;

  return EOK;
}

static inline errno_t dps_add_servo (dpservo_base *dps, uint8_t id) {
  EVALUE(NULL, dps);
  ELTGE(0, DPS_SERVO_ID_INVALID, id);
  ELTGE(0, dps->max_num_of_servo, dps->num_of_servo);

  for (size_t i = 0; i < dps->num_of_servo; i++) {
    if (dps->servo_ids[i] == id) {
      fprintf(stderr, " %s ID:%02x is already added\n", __FUNCTION__, id);
      return EINVAL;
    }
  }

  dps->servo_ids[dps->num_of_servo++] = id;

  return EOK;
}

static inline uint8_t dps_get_num_of_servo (dpservo_base *dps) {
  EVALUE(NULL, dps);
  return dps->num_of_servo;
}

static inline errno_t dps_open (dpservo_base *dps, const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, dps);
  if (dps->io_enabled) ECALL(hr_serial_open(dps->hrs, device, port, baudrate, parity));
  return EOK;
}

static inline errno_t dps_close (dpservo_base *dps) {
  EVALUE(NULL, dps);
  if (dps->io_enabled) ECALL(hr_serial_close(dps->hrs));
  return EOK;
}

static inline errno_t dps_set_serial (dpservo_base *dps, bool io_enabled) {
  EVALUE(NULL, dps);
  dps->io_enabled = io_enabled;
  return EOK;
}

static inline errno_t dps_set_state (dpservo_base *dps, const uint8_t id, dps_servo_state state) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_state(dps, id, state));
  return EOK;
}

static inline errno_t dps_set_states (dpservo_base *dps, dps_servo_state state) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_states(dps, state));
  return EOK;
}

static inline errno_t dps_set_goal (dpservo_base *dps, const uint8_t id, float64_t goal) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_goal(dps, id, goal));
  return EOK;
}

static inline errno_t dps_set_goals (dpservo_base *dps, const size_t num, float64_t goal[/*num*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.set_goals(dps, num, goal));
  return EOK;
}

static inline errno_t dps_write_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.write_mem(dps, id, start_addr, size, data));
  return EOK;
}

static inline errno_t dps_read_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  ECALL(dps->ops.read_mem(dps, id, start_addr, size, data));
  return EOK;
}

#endif

