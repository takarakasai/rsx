
#ifndef ICSPY_H
#define ICSPY_H

#include "icspp/icspp.h"

#include "boost/python.hpp"

#define kICSPY_MAX_NUM_OF_JOINTS 20
#define kICSPY_MAX_PKT_SIZE  1024

class icspy : public dp::icspp<kICSPY_MAX_NUM_OF_JOINTS, kICSPY_MAX_PKT_SIZE> {

 typedef dp::icspp<kICSPY_MAX_NUM_OF_JOINTS, kICSPY_MAX_PKT_SIZE>  base;

 public:

  icspy(void) {
  }

  virtual ~icspy(void) {
  }

  uint8_t get_id (void) {
    uint8_t id;
    ECALL_THROW(base::get_id(&id));
    return id;
  }

  uint8_t get_stretch (uint8_t id) {
    uint8_t stretch = 0; // TODO:
    ECALL_THROW(base::get_stretch(id, &stretch));
    return stretch;
  }

  uint8_t get_speed (uint8_t id) {
    uint8_t speed = 0;  // TODO:
    ECALL(base::get_speed(id, &speed));
    return EOK;
  }

  uint8_t get_current (uint8_t id) {
    uint8_t cur = 0;  // TODO:
    ECALL(base::get_current(id, &cur));
    return cur;
  }

  uint8_t get_temp (uint8_t id) {
    uint8_t temp = 0;  // TODO:
    ECALL(base::get_temp(id, &temp));
    return temp;
  }

 protected:

 private: 
};

#endif

