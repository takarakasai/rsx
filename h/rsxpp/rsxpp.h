
#ifndef RSXPP_H
#define RSXPP_H

extern "C" {
  #include "rsx.h"
}

#include "boost/python.hpp"

namespace dp {

  template <size_t kNUM_OF_JOINTS, size_t kMAX_PKT_SIZE>
  class rsxpp
  {
   public:
    rsxpp(void) {
      RSX_INIT(servo_inst);
      servo = &servo_inst;
    }

    ~rsxpp(void) {
      errno_t eno = rsx_close(servo);
      if (eno != EOK) {
        fprintf(stderr, "rsx_close error at %s\n", __FUNCTION__);
      }
    }

    errno_t open (const char8_t *device, const char8_t *port) {
      ECALL(rsx_open(servo, device, port));
      return EOK;
    }

    errno_t close (void) {
      ECALL(rsx_close(servo));
      return EOK;
    }

    errno_t spkt_write (void) {
      ECALL(rsx_spkt_write(servo));
      return EOK;
    }

    errno_t spkt_write_read (void) {
      ECALL(rsx_spkt_write_read(servo));
      return EOK;
    }

    errno_t lpkt_write (void) {
      ECALL(rsx_lpkt_write(servo));
      return EOK;
    }

    errno_t spkt_mem_write (uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_write(servo, id, start_addr, size, data));
      return EOK;
    }

    errno_t spkt_mem_read (uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_spkt_mem_read(servo, id, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write (uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, uint8_t **data/*[num][size]*/) {
      ECALL(rsx_lpkt_mem_write(servo, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write_int16 (uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t **data/*[num][size]*/) {
      ECALL(rsx_lpkt_mem_write_int16(servo, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t lpkt_mem_write_all (uint8_t id[/*num*/], uint8_t num , uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
      ECALL(rsx_lpkt_mem_write_all(servo, id, num, start_addr, size, data));
      return EOK;
    }

    errno_t set_serial (bool use_serial) {
      ECALL(rsx_set_serial(servo, use_serial));
      return EOK;
    }
 
   protected: 
    RSX_DECL(servo_inst, kNUM_OF_JOINTS, kMAX_PKT_SIZE);
    rsx *servo;
   private:
  };
}

#endif

