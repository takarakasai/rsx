
#ifndef RSXPY_H
#define RSXPY_H

#include "rsxpp.h"

#include "boost/python.hpp"

#define kRSXPY_NUM_OF_JOINTS 20
#define kRSXPY_MAX_PKT_SIZE  1024
namespace bp = boost::python;

class rsxpy : public dp::rsxpp<kRSXPY_NUM_OF_JOINTS, kRSXPY_MAX_PKT_SIZE> {

#define GET_C_ARRAY(TYPE, size_name, array_name, bpy_list)     \
  const TYPE size_name = len(bpy_list);                        \
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

 typedef dp::rsxpp<kRSXPY_NUM_OF_JOINTS, kRSXPY_MAX_PKT_SIZE>  base;

 public:

  rsxpy(void) {
  }

  virtual ~rsxpy(void) {
  }

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

    printf("============> %d %d\n", num, size);
    ECALL(base::lpkt_mem_write_all(id, num, start_addr, size, data));
    return EOK;
  }

  errno_t lpkt_mem_write_int16_all (bp::list &in_id, uint8_t start_addr, bp::list &in_data) {
    GET_C_ARRAY(uint8_t, num, id/*[num]*/, in_id);
    GET_C_ARRAY(int16_t, size, data/*[size]*/, in_data);

    ECALL(base::lpkt_mem_write_int16_all(id, num, start_addr, size, data));
    return EOK;
  }

 protected:

 private: 
  bp::list rdata;
};

#endif

