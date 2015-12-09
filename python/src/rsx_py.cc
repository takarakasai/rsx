
#include "rsxpp.h"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( rsx_py ) {
  using namespace servo;
  boost::python::class_<rsxc>("rsxc")
     .def("open", &rsxc::open)
     .def("close", &rsxc::close)
     .def("spkt_mem_write", &rsxc::spkt_mem_write)
     .def("spkt_mem_read", &rsxc::spkt_mem_read)
     .def("lpkt_mem_write", &rsxc::lpkt_mem_write)
     .def("lpkt_mem_write_int16", &rsxc::lpkt_mem_write_int16)
     .def("lpkt_mem_write_all", &rsxc::lpkt_mem_write_all)
     .def("set_serial", &rsxc::set_serial)
     //.add_property("servo", &rsxc::servo)
     ; 
}

