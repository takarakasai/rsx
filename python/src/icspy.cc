
#include "icspy.h"

extern "C" {
  #include "rsx/mmap/rs30x.h"
}

#include <boost/python.hpp>
//#include <boost/numpy.hpp>

BOOST_PYTHON_MODULE( icspy ) {
  using namespace dp;

  //boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  boost::python::class_<icspy>("icspy")
      .def("open", &icspy::open)
      .def("close", &icspy::close)
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
      .def("set_serial", &icspy::set_serial)
      ; 
}

