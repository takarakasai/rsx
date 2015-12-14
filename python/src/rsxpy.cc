
#include "rsxpy.h"

#include "mmap/rs30x.h"

#include <boost/python.hpp>
//#include <boost/numpy.hpp>

BOOST_PYTHON_MODULE( rsxpy ) {
  using namespace dp;

  //boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
  
  boost::python::enum_<hr_baudrate>("HR_BAUDRATE")
      .value("HR_B9600"   , HR_B9600    )
      .value("HR_B19200"  , HR_B19200   )
      .value("HR_B38400"  , HR_B38400   )
      .value("HR_B57600"  , HR_B57600   )
      .value("HR_B115200" , HR_B115200  )
      .value("HR_B230400" , HR_B230400  )
      .value("HR_B460800" , HR_B460800  )
      .value("HR_B576000" , HR_B576000  )
      .value("HR_B1152000", HR_B1152000 )
      ;

  boost::python::enum_<hr_parity>("HR_PARITY")
      .value("HR_PAR_NONE" , HR_PAR_NONE )
      .value("HR_PAR_EVEN" , HR_PAR_EVEN )
      .value("HR_PAR_ODD"  , HR_PAR_ODD  )
      ;

  boost::python::enum_<RSX_RS30X_MEM_ADDR>("RSX_RS30X_MEM_ADDR")
      .value("RSX_ROM_MODEL_NUM_L"     ,RSX_ROM_MODEL_NUM_L  ) 
      .value("RSX_ROM_MODEL_NUM_H"     ,RSX_ROM_MODEL_NUM_H  ) 
      .value("RSX_ROM_FW_VERSION"      ,RSX_ROM_FW_VERSION   ) 

      .value("RSX_ROM_SERVO_ID"        ,RSX_ROM_SERVO_ID     ) 
      .value("RSX_ROM_REVERSE"         ,RSX_ROM_REVERSE      ) 
      .value("RSX_ROM_BAUDRATE"        ,RSX_ROM_BAUDRATE     ) 
      .value("RSX_ROM_RTN_DELAY"       ,RSX_ROM_RTN_DELAY    ) 
      .value("RSX_ROM_CW_ANG_LIM_L"    ,RSX_ROM_CW_ANG_LIM_L ) 
      .value("RSX_ROM_CW_ANG_LIM_H"    ,RSX_ROM_CW_ANG_LIM_H ) 
      .value("RSX_ROM_CCW_ANG_LIM_L"   ,RSX_ROM_CCW_ANG_LIM_L) 
      .value("RSX_ROM_CCW_ANG_LIM_H"   ,RSX_ROM_CCW_ANG_LIM_H) 
      .value("RSX_ROM_TEMP_LIM_L"      ,RSX_ROM_TEMP_LIM_L   ) 
      .value("RSX_ROM_TEMP_LIM_H"      ,RSX_ROM_TEMP_LIM_H   ) 
      .value("RSX_ROM_CW_CMP_MARGN"    ,RSX_ROM_CW_CMP_MARGN ) 
      .value("RSX_ROM_CCW_CMP_MARGN"   ,RSX_ROM_CCW_CMP_MARGN) 
      .value("RSX_ROM_CW_CMP_SLOPE"    ,RSX_ROM_CW_CMP_SLOPE ) 
      .value("RSX_ROM_CCW_CMP_SLOPE"   ,RSX_ROM_CCW_CMP_SLOPE) 
      .value("RSX_ROM_PUNCH_L"         ,RSX_ROM_PUNCH_L      ) 
      .value("RSX_ROM_PUNCH_H"         ,RSX_ROM_PUNCH_H      ) 

      .value("RSX_RAM_GOAL_POS_L"      ,RSX_RAM_GOAL_POS_L   ) 
      .value("RSX_RAM_GOAL_POS_H"      ,RSX_RAM_GOAL_POS_H   ) 
      .value("RSX_RAM_GOAL_TIM_L"      ,RSX_RAM_GOAL_TIM_L   ) 
      .value("RSX_RAM_GOAL_TIM_H"      ,RSX_RAM_GOAL_TIM_H   ) 
      .value("RSX_RAM_MAX_TORQUE"      ,RSX_RAM_MAX_TORQUE   ) 
      .value("RSX_RAM_TRQ_ENABLE"      ,RSX_RAM_TRQ_ENABLE   ) 

      .value("RSX_RAM_PRESENT_POS_L"   ,RSX_RAM_PRESENT_POS_L) 
      .value("RSX_RAM_PRESENT_POS_H"   ,RSX_RAM_PRESENT_POS_H) 
      .value("RSX_RAM_PRESENT_TIM_L"   ,RSX_RAM_PRESENT_TIM_L) 
      .value("RSX_RAM_PRESENT_TIM_H"   ,RSX_RAM_PRESENT_TIM_H) 
      .value("RSX_RAM_PRESENT_SPD_L"   ,RSX_RAM_PRESENT_SPD_L) 
      .value("RSX_RAM_PRESENT_SPD_H"   ,RSX_RAM_PRESENT_SPD_H) 
      .value("RSX_RAM_PRESENT_CUR_L"   ,RSX_RAM_PRESENT_CUR_L) 
      .value("RSX_RAM_PRESENT_CUR_H"   ,RSX_RAM_PRESENT_CUR_H) 
      .value("RSX_RAM_PRESENT_TMP_L"   ,RSX_RAM_PRESENT_TMP_L) 
      .value("RSX_RAM_PRESENT_TMP_H"   ,RSX_RAM_PRESENT_TMP_H) 
      .value("RSX_RAM_PRESENT_VLT_L"   ,RSX_RAM_PRESENT_VLT_L) 
      .value("RSX_RAM_PRESENT_VLT_H"   ,RSX_RAM_PRESENT_VLT_H) 
      ;

  boost::python::class_<rsxpy>("rsxpy")
      .def("open", &rsxpy::open)
      .def("close", &rsxpy::close)
      .def("spkt_mem_write", &rsxpy::spkt_mem_write)
      .def("spkt_mem_write_int16", &rsxpy::spkt_mem_write_int16)
      .def("spkt_mem_read", &rsxpy::spkt_mem_read)
      .def("spkt_mem_read_int16", &rsxpy::spkt_mem_read_int16)
      .def("lpkt_mem_write", &rsxpy::lpkt_mem_write)
      .def("lpkt_mem_write_int16", &rsxpy::lpkt_mem_write_int16)
      .def("lpkt_mem_write_all", (errno_t (rsxpy::*) (boost::python::list &in_id, uint8_t start_addr, boost::python::list &in_data))&rsxpy::lpkt_mem_write_all)
      .def("lpkt_mem_write_int16_all", &rsxpy::lpkt_mem_write_int16_all)
      .def("set_serial", &rsxpy::set_serial)
      //.add_property("servo", &rsxc::servo)
      ; 
}

