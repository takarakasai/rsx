
#ifndef RSXPY_H
#define RSXPY_H

#include "icspp/icspp.h"

#include "boost/python.hpp"

#define kICSPY_MAX_NUM_OF_JOINTS 20
#define kICSPY_MAX_PKT_SIZE  1024
namespace bp = boost::python;

class icspy : public dp::icspp<kICSPY_MAX_NUM_OF_JOINTS, kICSPY_MAX_PKT_SIZE> {

#define GET_C_ARRAY(TYPE, size_name, array_name, bpy_list)     \
  const size_t size_name = len(bpy_list);                      \
  TYPE array_name[size_name];                                  \
  for (size_t i = 0; i < size_name; i++) {                     \
    array_name[i] = boost::python::extract<TYPE>(bpy_list[i]); \
  }

#define SET_C_ARRAY(TYPE, size, array_name, bpy_list)          \
  for (size_t i = 0; i < size; i++) {                          \
    bpy_list[i] = array_name[i];                               \
  }

#define GET_C_ARRAY_2D(TYPE, size1, size2, array_name, bpy_list)       \
  TYPE array_name[size1][size2];                                       \
  for (size_t i = 0; i < size1; i++) {                                 \
    for (size_t j = 0; j < size2; j++) {                               \
      array_name[i][j] = bp::extract<TYPE>(bpy_list[i * size2 + j]);   \
    }                                                                  \
  }

 typedef dp::icspp<kICSPY_MAX_NUM_OF_JOINTS, kICSPY_MAX_PKT_SIZE>  base;

 public:

  icspy(void) {
  }

  virtual ~icspy(void) {
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
    ECALL(base::set_stretch(id, stretch));
    return EOK;
  }

  errno_t get_stretch (uint8_t id, uint8_t *stretch) {
    ECALL(base::get_stretch(id, stretch));
    return EOK;
  }

  errno_t set_speed (uint8_t id, uint8_t  speed) {
    ECALL(base::set_speed(id, speed));
    return EOK;
  }

  errno_t get_speed (uint8_t id, uint8_t *speed) {
    ECALL(base::get_speed(id, speed));
    return EOK;
  }

  errno_t set_current_limit (uint8_t id, uint8_t cur_limit) {
    ECALL(base::set_current_limit(id, cur_limit));
    return EOK;
  }

  errno_t get_current (uint8_t id, uint8_t *cur) {
    ECALL(base::get_current(id, cur));
    return EOK;
  }

  errno_t set_temp_limit (uint8_t id, uint8_t temp_limit) {
    ECALL(base::set_temp_limit(id, temp_limit));
    return EOK;
  }

  errno_t get_temp (uint8_t id, uint8_t *temp) {
    ECALL(base::get_temp(id, temp));
    /* not implemented yet */
    return EOK;
  }

#if 0
  errno_t spkt_mem_write (uint8_t id, uint8_t start_addr, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, size, data/*[size]*/, in_data);

    ECALL(base::spkt_mem_write(id, start_addr, size, data));
    return EOK;
  }

  errno_t spkt_mem_write_int16 (uint8_t id, uint8_t start_addr, bp::list &in_data) {
    GET_C_ARRAY(int16_t, size, data/*[size]*/, in_data);

    ECALL(base::spkt_mem_write_int16(id, start_addr, size, data));
    return EOK;
  }

  //errno_t spkt_mem_read (uint8_t id, uint8_t start_addr, bp::list &in_data) {
  bp::list spkt_mem_read (uint8_t id, uint8_t start_addr, size_t num) {
    uint8_t data[num];
    //const uint8_t size = len(in_data);
    //uint8_t data[size];

    //ECALL(base::spkt_mem_read(id, start_addr, size, data));
    errno_t eno = base::spkt_mem_read(id, start_addr, num, data);
    if (eno != EOK) {
      throw eno;
    }

    boost::python::list plist;
    for (size_t i = 0; i < num; i++) {
      plist.append(data[i]);
    }

    //SET_C_ARRAY(uint8_t, size, data/*[size]*/, in_data);

    return plist;
  }

  bp::list spkt_mem_read_int16 (uint8_t id, uint8_t start_addr, size_t num/* num * int16_t */) {
    int16_t data[num];

    errno_t eno = base::spkt_mem_read_int16(id, start_addr, num, data);
    if (eno != EOK) {
      throw eno;
    }

    boost::python::list plist;
    for (size_t i = 0; i < num; i++) {
      plist.append(data[i]);
    }

    return plist;
  }

  errno_t lpkt_mem_write (bp::list &in_id, uint8_t start_addr, size_t size, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, id/*[num]*/, in_id);
    GET_C_ARRAY_2D(uint8_t, num, size, data/*[num][size]*/, in_data);

    ECALL(base::lpkt_mem_write(id, num, start_addr, size, (uint8_t**)data));
    return EOK;
  }

  errno_t lpkt_mem_write_int16 (bp::list &in_id, uint8_t start_addr, size_t size, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, id/*[num]*/, in_id);
    GET_C_ARRAY_2D(int16_t, num, size, data/*[num][size]*/, in_data);

    ECALL(base::lpkt_mem_write_int16(id, num, start_addr, size, (int16_t**)data));
    return EOK;
  }

  errno_t lpkt_mem_write_all (bp::list &in_id, uint8_t start_addr, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, id/*[num]*/, in_id);
    GET_C_ARRAY(uint8_t, size, data/*[size]*/, in_data);

    ECALL(base::lpkt_mem_write_all(id, num, start_addr, size, data));
    return EOK;
  }

  errno_t lpkt_mem_write_int16_all (bp::list &in_id, uint8_t start_addr, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, id/*[num]*/, in_id);
    GET_C_ARRAY(int16_t, size, data/*[size]*/, in_data);

    ECALL(base::lpkt_mem_write_int16_all(id, num, start_addr, size, data));
    return EOK;
  }
#endif

 protected:

 private: 
  bp::list rdata;
};

#endif

