
#include "dps_proxy.h"

#include <string.h>

#include "ics/ics.h"
#include "rsx/rsx.h"

/* singleton interface */
static dpservo_base *g_target = NULL;
DPSERVO_STATIC_DECL(g_ics, 32, 1024, ICS_DECL);
DPSERVO_STATIC_DECL(g_rsx, 32, 1024, RSX_DECL);
/* 
 * additional interface should be heare
 *
DPSERVO_STATIC_DECL(g_xxx, 32, 1024, XXX_DECL);
 */

errno_t dps_proxy_assign (const char* name) {
  EXPECT_VALUE_ERRNO(NULL, g_target, -1);

  if (strcmp(name, "ics") == 0) {
    DPSERVO_INIT(g_ics, ICS_INIT);
    g_target = g_ics;
  } else if (strcmp(name, "rsx") == 0) {
    DPSERVO_INIT(g_rsx, RSX_INIT);
    g_target = g_rsx;
/* 
 * additional interface should be heare
 *
  } else if (strcmp(name, "xxx") == )) {
    DPSERVO_INIT(g_xxx, XXX_INIT);
    g_target = g_xxx;
 */
  } else {
    return EINVAL;
  }

  return EOK;
}

errno_t dps_proxy_release () {
  EVALUE(NULL, g_target);

  ECALL(dps_set_states(g_target, kDpsServoOff));
  ECALL(dps_close(g_target));
  g_target = NULL;

  return EOK;
}

errno_t dps_proxy_open (const char8_t *device, const char8_t *port, int32_t baudrate) {
  hr_baudrate hrbaudrate;
  ECALL(hr_int2baudrate(baudrate, &hrbaudrate));

  hr_parity parity;
  if (g_target == g_ics) {
    parity = HR_PAR_EVEN;
  } else if (g_target == g_rsx) {
    parity = HR_PAR_NONE;
  } else {
    return EINVAL;
  }

  ECALL(dps_open(g_target, device, port, hrbaudrate, parity));
  return EOK;
}

errno_t dps_proxy_close (void) {
  ECALL(dps_close(g_target));
  return EOK;
}

errno_t dps_proxy_set_serial (bool io_enabled) {
  ECALL(dps_set_serial(g_target, io_enabled));
  return EOK;
}

/*** servo setting ***/

errno_t dps_proxy_add_servo (uint8_t id) {
  ECALL(dps_add_servo(g_target, id));
  return EOK;
}

errno_t dps_proxy_clear_servo (void) {
  ECALL(dps_clear_servo(g_target));
  return EOK;
}

errno_t dps_proxy_set_servo (uint8_t num, uint8_t id[/*num*/]) {
  ECALL(dps_set_servo(g_target, num, id));
  return EOK;
}

uint8_t dps_proxy_get_num_of_servo (void) {
  return dps_get_num_of_servo(g_target);
}

/*** servo angle offset ***/

errno_t dps_proxy_set_offset_angle (uint8_t id, float64_t oangle) {
  ECALL(dps_set_offset_angle(g_target, id, oangle));
  return EOK;
}

errno_t dps_proxy_set_offset_angles (uint8_t num, float64_t oangle[/*num*/]) {
  ECALL(dps_set_offset_angles(g_target, num, oangle));
  return EOK;
}

/*** servo state ***/

errno_t dps_proxy_set_state (const uint8_t id, const char* state_str) {
  EVALUE(NULL, state_str);

  dps_servo_state state;
  ECALL(dps_str2servo_state(state_str, &state));
  ECALL(dps_set_state(g_target, id, state));

  return EOK;
}

errno_t dps_proxy_set_states (const char* state_str) {
  EVALUE(NULL, state_str);

  dps_servo_state state;
  ECALL(dps_str2servo_state(state_str, &state));
  ECALL(dps_set_states(g_target, state));

  return EOK;
}

/*** servo pos controll ***/

errno_t dps_proxy_set_goal (const uint8_t id, float64_t goal) {
  ECALL(dps_set_goal(g_target, id, goal));
  return EOK;
}

errno_t dps_proxy_set_goals (const size_t num, float64_t goal[/*num*/]) {
  ECALL(dps_set_goals(g_target, num, goal));
  return EOK;
}

/*** servo memory write/read ***/ 
errno_t dps_proxy_write_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option) {
  ECALL(dps_mem_write(g_target, id, start_addr, size, data, option));
  return EOK;
}

errno_t dps_proxy_read_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option) {
  ECALL(dps_mem_write(g_target, id, start_addr, size, data, option));
  return EOK;
}

