cmake_minimum_required(VERSION 2.8.4)

project(rsx_sample)

file(GLOB SRCS "src/*.c")

if(${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -std=gnu99 -D_GNU_SOURCE")
endif()

set(APP_NAME rsx_app)

add_executable  (${APP_NAME} ${SRCS})

set(DEPENDENCIES "rsx")

add_dependencies(${APP_NAME} ${DEPENDENCIES})
target_link_libraries(${APP_NAME} ${DEPENDENCIES})

if(${UNIX})
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
elseif(${MINGW})
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

