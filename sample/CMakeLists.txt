cmake_minimum_required(VERSION 2.8.4)

project(rsx_sample)

file(GLOB SRCS "src/*.c")

if(${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -std=gnu99 -D_GNU_SOURCE")
endif()

set(APP_NAME rsx_app)

set(DEPENDENCIES "dpservo")
list(APPEND DEPENDENCIES pthread rt)

foreach(SRC ${SRCS})
  message(STATUS ${SRC})
  string(REGEX REPLACE "(.+/)([^/]+)\\.c$" "\\2" STR ${SRC})
  message(STATUS ": " ${STR})

  add_executable(${STR} ${SRC})

  message(STATUS ": " ${STR} " with " ${DEPENDENCIES})

  add_dependencies(${STR} ${DEPENDENCIES})
  target_link_libraries(${STR} ${DEPENDENCIES})

  list(APPEND EXECUTABLES ${STR})

endforeach()

if(${UNIX})
    install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION bin)
elseif(${MINGW})
    install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

