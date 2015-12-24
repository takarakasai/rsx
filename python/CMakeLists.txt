cmake_minimum_required(VERSION 2.8.4)

project(rsxpy)

find_package(PythonInterp)
find_package(PythonLibs)
find_package(Boost COMPONENTS python)

include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
include_directories(${dpservo_SOURCE_DIR}/h/dpservo)

#if(${CMAKE_COMPILER_IS_GNUXX})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++11")
#endif()

link_libraries(dpservo ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}) # Deprecated but so convenient!

PYTHON_ADD_MODULE(${PROJECT_NAME} src/rsxpy.cc)

