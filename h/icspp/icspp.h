
#ifndef ICSPP_H
#define ICSPP_H

#include "servo.h" /* base class */

extern "C" {
  #include "ics.h"
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
      errno_t eno = rsx_close(pics);
      if (eno != EOK) {
        fprintf(stderr, "rsx_close error at %s\n", __FUNCTION__);
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

#if 0
    errno_t spkt_write (void) {
      ECALL(rsx_spkt_write(pics));
      return EOK;
    }

    errno_t spkt_write_read (void) {
      ECALL(rsx_spkt_write_read(pics));
      return EOK;
    }

    errno_t lpkt_write (void) {
      ECALL(rsx_lpkt_write(pics));
      return EOK;
    }

    errno_t spkt_mem_write (uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_write(pics, id, start_addr, size, data));
      return EOK;
    }

    errno_t spkt_mem_write_int16 (uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_write_int16(pics, id, start_addr, size, data));
      return EOK;
    }

    errno_t spkt_mem_read (uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_read(pics, id, start_addr, size, data));
      return EOK;
    }

    errno_t spkt_mem_read_int16 (uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_read_int16(pics, id, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write (uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, uint8_t **data/*[num][size]*/) {
      ECALL(rsx_lpkt_mem_write(pics, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write_int16 (uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t **data/*[num][size]*/) {
      ECALL(rsx_lpkt_mem_write_int16(pics, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write_all (uint8_t id[/*num*/], uint8_t num , uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_lpkt_mem_write_all(pics, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write_int16_all (uint8_t id[/*num*/], uint8_t num , uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
      ECALL(rsx_lpkt_mem_write_int16_all(pics, id, num, start_addr, size, data));
      return EOK;
    }
#endif

    errno_t set_serial (bool use_serial) {
      ECALL(rsx_set_serial(pics, use_serial));
      return EOK;
    }
 
   protected: 
    DPSERVO_DECL(servo_inst, kNUM_OF_JOINTS, kMAX_PKT_SIZE, ICS_DECL);
    ics *pics;
   private:
  };
}

#endif

