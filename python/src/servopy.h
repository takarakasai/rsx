
#ifndef SERVOPY_H
#define SERVOPY_H

#include "rsxpp/rsxpp.h"

#include "boost/python.hpp"

#define kRSXPY_NUM_OF_JOINTS 20
#define kRSXPY_MAX_PKT_SIZE  1024
namespace bp = boost::python;

class servopy : public dp::servo {

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

 typedef dp::servo base;

 public:

  servopy(void) {
  }

  virtual ~servopy(void) {
  }

  errno_t set_servo (bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, ids/*[num]*/, in_data);

    ECALL(base::set_servo(num, ids));
    return EOK;
  }

  errno_t set_offset_angles (bp::list &in_data) {
    GET_C_ARRAY(float64_t, num, oangle/*[num]*/, in_data);

    ECALL(base::set_offset_angles(num, oangle));
    return EOK;
  }

  errno_t set_goals (bp::list &in_data) {
    GET_C_ARRAY(float64_t, num, goal/*[num]*/, in_data);

    ECALL(base::set_goals(num, goal));
    return EOK;
  }

  errno_t write_mem (uint8_t id, uint8_t start_addr, bp::list &in_data, dps_opt_t option) {
    GET_C_ARRAY(uint8_t, wsize, wdata/*[wsize]*/, in_data);

    ECALL(base::write_mem(id, start_addr, wsize, wdata, option));

    return EOK;
  }

  bp::list read_mem (uint8_t id, uint8_t start_addr, size_t rsize, dps_opt_t option) {
    uint8_t rdata[rsize];

    ECALL_THROW(base::read_mem(id, start_addr, rsize, rdata, option));

    boost::python::list plist;
    for (size_t i = 0; i < rsize; i++) {
      plist.append(rdata[i]);
    }

    return plist;
  }

 protected:

 private: 

  bp::list rdata;
};

#endif

