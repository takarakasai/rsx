
#ifndef DP_SERVO_H
#define DP_SERVO_H

extern "C" {
  #include "dpservo.h"
  #include "serial/hr_serial.h"
}

//typedef struct dpservo_base_struct dpservo_base;
struct dpservo_base_struct;

namespace dp {

  class servo
  {
   public:
    /*** constructor / destructor ***/

    servo(void) {
      pservo = nullptr;
    }

    virtual ~servo(void) {
    }

    /*** com ***/

    errno_t open (const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity) {
      ECALL(dps_open(pservo, device, port, baudrate, parity));
      return EOK;
    }

    errno_t close (void) {
      ECALL(dps_close(pservo));
      return EOK;
    }
 
    errno_t set_serial (bool io_enabled) {
      ECALL(dps_set_serial(pservo, io_enabled));
      return EOK;
    }

    /*** servo setting ***/
    
    errno_t add_servo (uint8_t id) {
      ECALL(dps_add_servo(pservo, id));
      return EOK;
    }

    errno_t clear_servo (void) {
      ECALL(dps_clear_servo(pservo));
      return EOK;
    }

    errno_t set_servo (uint8_t num, uint8_t id[/*num*/]) {
      ECALL(dps_set_servo(pservo, num, id));
      return EOK;
    }

    uint8_t get_num_of_servo (void) {
      return dps_get_num_of_servo(pservo);
    }
    
    /*** servo angle offset ***/

    errno_t set_offset_angle (uint8_t id, float64_t oangle) {
      ECALL(dps_set_offset_angle(pservo, id, oangle));
      return EOK;
    }

    errno_t set_offset_angles (uint8_t num, float64_t oangle[/*num*/]) {
      ECALL(dps_set_offset_angles(pservo, num, oangle));
      return EOK;
    }
    
    /*** servo state ***/

    errno_t set_state (const uint8_t id, dps_servo_state state) {
      ECALL(dps_set_state(pservo, id, state));
      return EOK;
    }

    errno_t set_states (dps_servo_state state) {
      ECALL(dps_set_states(pservo, state));
      return EOK;
    }

    /*** servo pos controll ***/
    
    errno_t set_goal (const uint8_t id, float64_t goal) {
      ECALL(dps_set_goal(pservo, id, goal));
      return EOK;
    }

    errno_t set_goals (const size_t num, float64_t goal[/*num*/]) {
      ECALL(dps_set_goals(pservo, num, goal));
      return EOK;
    }
    
    /*** servo memory write/read ***/

    errno_t write_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option) {
      ECALL(dps_mem_write(pservo, id, start_addr, size, data, option));
      return EOK;
    }
    
    errno_t read_mem (const uint8_t id, uint8_t start_addr, size_t size, uint8_t data[/*size*/], dps_opt_t option) {
      ECALL(dps_mem_write(pservo, id, start_addr, size, data, option));
      return EOK;
    }

   protected: 
    dpservo_base_struct *pservo;

   private:
  };
}

#endif


