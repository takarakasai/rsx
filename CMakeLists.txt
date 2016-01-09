cmake_minimum_required(VERSION 2.8.4)

project(dpservo)

if(${MINGW})
  set(INSTALL_DIR "${CMAKE_SOURCE_DIR}/out") 
endif()

option(ENABLE_IO_STUB "build with unixio stub" OFF)
option(ENABLE_SAMPLE "enable to build sample app" ON)
option(ENABLE_PYTHON_API "enable to build python api" OFF)
option(ENABLE_MATLAB_API "enable to build matlab api" OFF)

option(HR_SERIAL_AUTO_READ_ECHO_DATA "" ON)
if(${HR_SERIAL_AUTO_READ_ECHO_DATA})
    add_definitions(-DHR_SERIAL_AUTO_READ_ECHO_DATA)
endif()

option(HR_SERIAL_LATENCY_CHECK "" OFF)
if(${HR_SERIAL_LATENCY_CHECK})
    add_definitions(-DHR_SERIAL_LATENCY_CHECK)
endif()

option(HR_SERIAL_LATENCY_CHECK_DETAIL "" OFF)
if(${HR_SERIAL_LATENCY_CHECK_DETAIL})
    add_definitions(-DHR_SERIAL_LATENCY_CHECK_DETAIL)
endif()

include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME})

file(GLOB SRCS "src/*.c")
file(GLOB SERIAL_SRCS "src/serial/*.c")
if (${ENABLE_IO_STUB})
  include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME}/serial/stub)
  file(GLOB SERIAL_IMPL_SRCS "src/serial/stub/*.c")
else()
  if(${MINGW})
    #include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME}/serial/stub)
    #file(GLOB SERIAL_IMPL_SRCS "src/serial/stub/*.c")
    include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME}/serial/mingw)
    file(GLOB SERIAL_IMPL_SRCS "src/serial/mingw/*.c")
  else()
    include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME}/serial/posix)
    file(GLOB SERIAL_IMPL_SRCS "src/serial/posix/*.c")
  endif()
endif()
file(GLOB TIME_SRCS "src/time/*.c")
list(APPEND SRCS ${SERIAL_SRCS} ${SERIAL_IMPL_SRCS} ${TIME_SRCS})

if(${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "-Wall -O2 -std=gnu99 ")
endif()

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})

if(${ENABLE_TESTING})
    add_subdirectory(test)
endif()

if(${ENABLE_SAMPLE})
    add_subdirectory(sample)
endif()

if(${ENABLE_PYTHON_API})
    add_subdirectory(python)
endif()

if(${ENABLE_MATLAB_API})
    add_subdirectory(matlab)
endif()

if(${UNIX})
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME} DESTINATION include)
elseif(${MINGW})
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

