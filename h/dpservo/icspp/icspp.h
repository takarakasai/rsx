
#ifndef ICSPP_H
#define ICSPP_H

#include "servo.h" /* base class */

extern "C" {
  #include "ics/ics.h"
  #include "serial/hr_serial.h"
}

namespace dp {

  template <size_t kNUM_OF_JOINTS, size_t kMAX_PKT_SIZE>
  class icspp : public servo
  {
   public:
    icspp(void) {
      DPSERVO_INIT(servo_inst, ICS_INIT);

      pics = (ics*)(servo_inst);
      pservo = servo_inst;
    }

    ~icspp(void) {
      errno_t eno = ics_close(pics);
      if (eno != EOK) {
        fprintf(stderr, "ics_close error at %s\n", __FUNCTION__);
      }
    }

    errno_t open (const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity) {
      ECALL(ics_open(pics, device, port, baudrate, parity));
      return EOK;
    }

    errno_t close (void) {
      ECALL(ics_close(pics));
      return EOK;
    }

    errno_t set_serial (bool use_serial) {
      ECALL(ics_set_serial(pics, use_serial));
      return EOK;
    }

    errno_t set_id (uint8_t  id) {
      ECALL(ics_set_id(pics, id));
      return EOK;
    }

    errno_t get_id (uint8_t *id) {
      ECALL(ics_get_id(pics, id));
      return EOK;
    }

    errno_t set_stretch (uint8_t id, uint8_t stretch) {
      /* not implemented yet */
      return EOK;
    }

    errno_t get_stretch (uint8_t id, uint8_t *stretch) {
      /* not implemented yet */
      return EOK;
    }

    errno_t set_speed (uint8_t id, uint8_t  speed) {
      /* not implemented yet */
      return EOK;
    }

    errno_t get_speed (uint8_t id, uint8_t *speed) {
      /* not implemented yet */
      return EOK;
    }

    errno_t set_current_limit (uint8_t id, uint8_t cur_limit) {
      /* not implemented yet */
      return EOK;
    }

    errno_t get_current (uint8_t id, uint8_t *cur) {
      /* not implemented yet */
      return EOK;
    }

    errno_t set_temp_limit (uint8_t id, uint8_t temp_limit) {
      /* not implemented yet */
      return EOK;
    }

    errno_t get_temp (uint8_t id, uint8_t *temp) {
      /* not implemented yet */
      return EOK;
    }

   protected: 
    DPSERVO_DECL(servo_inst, kNUM_OF_JOINTS, kMAX_PKT_SIZE, ICS_DECL);
    ics *pics;
   private:
  };
}

#endif

