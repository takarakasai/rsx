
#ifndef DPS_PROXY
#define DPS_PROXY

#include "dp_type.h"

/*** proxy specific API ***/
errno_t dps_proxy_assign (const char* name);
errno_t dps_proxy_release ();

/*** uart ***/
errno_t dps_proxy_open (const char8_t *device, const char8_t *port, int32_t baudrate);
errno_t dps_proxy_close (void);
errno_t dps_proxy_set_serial (bool io_enabled);

/*** servo setting ***/

errno_t dps_proxy_add_servo (uint8_t id);
errno_t dps_proxy_clear_servo (void);
errno_t dps_proxy_set_servo (uint8_t num, uint8_t id[/*num*/]);
uint8_t dps_proxy_get_num_of_servo (void);

/*** servo angle offset ***/

errno_t dps_proxy_set_offset_angle (uint8_t id, float64_t oangle);
errno_t dps_proxy_set_offset_angles (uint8_t num, float64_t oangle[/*num*/]);

/*** servo state ***/

errno_t dps_proxy_set_state (const uint8_t id, const char* state_str);
errno_t dps_proxy_set_states (const char* state_str);

/*** servo pos controll ***/

errno_t dps_proxy_set_goal (const uint8_t id, float64_t goal);
errno_t dps_proxy_set_goals (const size_t num, float64_t goal[/*num*/]);

/*** servo memory write/read ***/ 

errno_t dps_proxy_write_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option);
errno_t dps_proxy_read_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option);
 
#endif