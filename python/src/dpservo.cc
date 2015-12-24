
#include "servopy.h"
#include "rsxpy.h"
#include "icspy.h"

extern "C" {
  #include "rsx/mmap/rs30x.h"
  #include "ics/mmap/ics3x.h"
}

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( dpspy ) {
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

  boost::python::enum_<ICS_VERSION>("ICS_VERSION")
      .value("v20" , ICS_V20 )
      .value("v30" , ICS_V30 )
      .value("v35" , ICS_V35 )
      ;

  boost::python::enum_<ICS_MEM_ADDR>("ICS_MEM_ADDR")
      .value("PUNCH"         , ICS_ROM_PUNCH         )
      .value("DEAD_BAND"     , ICS_ROM_DEAD_BAND     )
      .value("DAMPING"       , ICS_ROM_DAMPING       )
      .value("SAFE_TIMER"    , ICS_ROM_SAFE_TIMER    )
      .value("FLAG"          , ICS_ROM_FLAG          )
      .value("PULSE_LLIMIT_H", ICS_ROM_PULSE_LLIMIT_H)
      .value("PULSE_LLIMIT_L", ICS_ROM_PULSE_LLIMIT_L)
      .value("PULSE_HLIMIT_H", ICS_ROM_PULSE_HLIMIT_H)
      .value("PULSE_HLIMIT_L", ICS_ROM_PULSE_HLIMIT_L)

      .value("BAUDRATE"      , ICS_ROM_BAUDRATE      )
      .value("TEMP_LIMIT"    , ICS_ROM_TEMP_LIMIT    )
      .value("CUR_LIMIT"     , ICS_ROM_CUR_LIMIT     )

      .value("ID"            , ICS_ROM_ID            )
      ;

  boost::python::enum_<ICS30_MEM_ADDR>("ICS30_MEM_ADDR")
      .value("PUNCH"         , ICS30_ROM_PUNCH         )
      .value("DEAD_BAND"     , ICS30_ROM_DEAD_BAND     )
      .value("DAMPING"       , ICS30_ROM_DAMPING       )
      .value("SAFE_TIMER"    , ICS30_ROM_SAFE_TIMER    )
      .value("FLAG"          , ICS30_ROM_FLAG          )
      .value("PULSE_LLIMIT_H", ICS30_ROM_PULSE_LLIMIT_H)
      .value("PULSE_LLIMIT_L", ICS30_ROM_PULSE_LLIMIT_L)
      .value("PULSE_HLIMIT_H", ICS30_ROM_PULSE_HLIMIT_H)
      .value("PULSE_HLIMIT_L", ICS30_ROM_PULSE_HLIMIT_L)

      .value("BAUDRATE"      , ICS30_ROM_BAUDRATE      )
      .value("TEMP_LIMIT"    , ICS30_ROM_TEMP_LIMIT    )
      .value("CUR_LIMIT"     , ICS30_ROM_CUR_LIMIT     )

      .value("STRETCH_GAIN"  , ICS30_ROM_STRETCH_GAIN  )
      .value("SPEED"         , ICS30_ROM_SPEED         )

      .value("OFFSET"        , ICS30_ROM_OFFSET        )
      .value("ID"            , ICS30_ROM_ID            )
      .value("SIZE"          , ICS30_ROM_SIZE          )
      ;

  boost::python::enum_<ICS35_MEM_ADDR>("ICS35_MEM_ADDR")
      .value("STRETCH_GAIN"  , ICS35_ROM_STRETCH_GAIN  ) 
      .value("SPEED"         , ICS35_ROM_SPEED         ) 

      .value("PUNCH"         , ICS35_ROM_PUNCH         ) 
      .value("DEAD_BAND"     , ICS35_ROM_DEAD_BAND     ) 
      .value("DAMPING"       , ICS35_ROM_DAMPING       ) 
      .value("SAFE_TIMER"    , ICS35_ROM_SAFE_TIMER    ) 
      .value("FLAG"          , ICS35_ROM_FLAG          ) 
      .value("PULSE_LLIMIT_H", ICS35_ROM_PULSE_LLIMIT_H) 
      .value("PULSE_LLIMIT_L", ICS35_ROM_PULSE_LLIMIT_L) 
      .value("PULSE_HLIMIT_H", ICS35_ROM_PULSE_HLIMIT_H) 
      .value("PULSE_HLIMIT_L", ICS35_ROM_PULSE_HLIMIT_L) 

      .value("BAUDRATE"      , ICS35_ROM_BAUDRATE      ) 
      .value("TEMP_LIMIT"    , ICS35_ROM_TEMP_LIMIT    ) 
      .value("CUR_LIMIT"     , ICS35_ROM_CUR_LIMIT     ) 

      .value("RESPONSE"      , ICS35_ROM_RESPONSE      ) 
      .value("USER_OFFSET"   , ICS35_ROM_USER_OFFSET   ) 

      .value("ID"            , ICS35_ROM_ID            ) 
      .value("C_CHG_STRETCH1", ICS35_ROM_C_CHG_STRETCH1) 
      .value("C_CHG_STRETCH2", ICS35_ROM_C_CHG_STRETCH2) 
      .value("C_CHG_STRETCH3", ICS35_ROM_C_CHG_STRETCH3) 
      .value("SIZE"          , ICS35_ROM_SIZE          ) 
      ;

  boost::python::enum_<dps_servo_state>("dps_servo_state")
      .value("off"           , kDpsServoOff)
      .value("break"         , kDpsServoBrk)
      .value("on"            , kDpsServoOn)
      ;

  boost::python::class_<servopy>("servopy")
      /* uart api */
      .def("open"              , &servo::open             )
      .def("close"             , &servo::close            )
      .def("set_serial"        , &servo::set_serial       )
      /* servo api */
      .def("add_servo"         , &servo::add_servo        )
      .def("clear_servo"       , &servo::clear_servo      )
      .def("set_servo "        , &servo::set_servo        )
      .def("set_offset_angle"  , &servo::set_offset_angle )
      .def("set_offset_angles" , &servo::set_offset_angles)
      .def("set_state"         , &servo::set_state        )
      .def("set_states"        , &servo::set_states       )
      .def("set_goal"          , &servo::set_goal         )
      .def("set_goals"         , &servo::set_goals        )
      .def("write_mem"         , &servo::write_mem        )
      .def("read_mem"          , &servo::read_mem         )
      ; 
 
  boost::python::class_<rsxpy, boost::python::bases<servopy>>("rsxpy")
      /* uart api */
      .def("open"              , &servo::open             )
      .def("close"             , &servo::close            )
      .def("set_serial"        , &servo::set_serial       )
      /* servo api */
      .def("add_servo"         , &servo::add_servo        )
      .def("clear_servo"       , &servo::clear_servo      )
      .def("set_servo "        , &servo::set_servo        )
      .def("set_offset_angle"  , &servo::set_offset_angle )
      .def("set_offset_angles" , &servo::set_offset_angles)
      .def("set_state"         , &servo::set_state        )
      .def("set_states"        , &servo::set_states       )
      .def("set_goal"          , &servo::set_goal         )
      .def("set_goals"         , &servo::set_goals        )
      .def("write_mem"         , &servo::write_mem        )
      .def("read_mem"          , &servo::read_mem         )
      /* specific api */
      .def("spkt_mem_write", &rsxpy::spkt_mem_write)
      .def("spkt_mem_write_int16", &rsxpy::spkt_mem_write_int16)
      .def("spkt_mem_read", &rsxpy::spkt_mem_read)
      .def("spkt_mem_read_int16", &rsxpy::spkt_mem_read_int16)
      .def("lpkt_mem_write", &rsxpy::lpkt_mem_write)
      .def("lpkt_mem_write_int16", &rsxpy::lpkt_mem_write_int16)
      .def("lpkt_mem_write_all", (errno_t (rsxpy::*) (boost::python::list &in_id, uint8_t start_addr, boost::python::list &in_data))&rsxpy::lpkt_mem_write_all)
      .def("lpkt_mem_write_int16_all", &rsxpy::lpkt_mem_write_int16_all)
      ; 

  boost::python::class_<icspy, boost::python::bases<servopy>>("icspy")
      /* uart api */
      .def("open"              , &servo::open             )
      .def("close"             , &servo::close            )
      .def("set_serial"        , &servo::set_serial       )
      /* servo api */
      .def("add_servo"         , &servo::add_servo        )
      .def("clear_servo"       , &servo::clear_servo      )
      .def("set_servo "        , &servo::set_servo        )
      .def("set_offset_angle"  , &servo::set_offset_angle )
      .def("set_offset_angles" , &servo::set_offset_angles)
      .def("set_state"         , &servo::set_state        )
      .def("set_states"        , &servo::set_states       )
      .def("set_goal"          , &servo::set_goal         )
      .def("set_goals"         , &servo::set_goals        )
      .def("write_mem"         , &servo::write_mem        )
      .def("read_mem"          , &servo::read_mem         )
      /* specific api */
      .def("set_id", &icspy::set_id)
      .def("get_id", &icspy::get_id)
      .def("set_stretch", &icspy::set_stretch)
      .def("get_stretch", &icspy::get_stretch)
      .def("set_speed", &icspy::set_speed)
      .def("get_speed", &icspy::get_speed)
      .def("set_current_limit", &icspy::set_current_limit)
      .def("get_current", &icspy::get_current)
      .def("set_temp_limit", &icspy::set_temp_limit)
      .def("get_temp", &icspy::get_temp)
      ; 

}

