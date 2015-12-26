cmake_minimum_required(VERSION 2.8.4)

project(dps_proxy)

set(DEPENDENCIES "dpservo")

include_directories(${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME})

file(GLOB SRCS "src/*.c")

if(${CMAKE_COMPILER_IS_GNUCC})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -std=gnu99 -D_GNU_SOURCE")
endif()

add_library(${PROJECT_NAME} SHARED ${SRCS})

add_dependencies(${PROJECT_NAME} ${DEPENDENCIES})
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})

if(${UNIX})
  install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME} DESTINATION include)
elseif(${MINGW})
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

