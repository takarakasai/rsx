cmake_minimum_required(VERSION 2.8.4)

# project(rsx_sample)

file(GLOB SRCS "src/*.c")

set(SRCS
  "${PROJECT_SOURCE_DIR}/sample/src/oneshot_position.c"
  "${PROJECT_SOURCE_DIR}/sample/src/oneshot_torque.c"
  "${PROJECT_SOURCE_DIR}/sample/src/bulk_position.c"
)

set(COMMON_SRCS
  "${PROJECT_SOURCE_DIR}/sample/src/util.c"
)

foreach(src ${SRCS})
  GET_FILENAME_COMPONENT(name_ ${src} NAME_WE)
  message(STATUS "src -->: ${src} : ${name_}")
  set(exec_name ${PROJECT_NAME}_${name_})

  add_executable(${exec_name})
  target_sources(${exec_name} PUBLIC ${src} ${COMMON_SRCS})
  target_compile_options(${exec_name} PRIVATE "-Wall" "-Werror" "-O2" "-std=c99")

  set(PRJ_LIB "${PROJECT_NAME}")

  target_link_libraries(
    ${exec_name}
    ${PRJ_LIB}
  )
endforeach() 

# if(${CMAKE_COMPILER_IS_GNUCC})
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -std=gnu99 -D_GNU_SOURCE")
# endif()
# 
# set(APP_NAME rsx_app)
# 
# add_executable  (${APP_NAME} ${SRCS})
# 
# set(DEPENDENCIES "rsx")
# 
# add_dependencies(${APP_NAME} ${DEPENDENCIES})
# target_link_libraries(${APP_NAME} ${DEPENDENCIES})

if(${UNIX})
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
elseif(${MINGW})
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

