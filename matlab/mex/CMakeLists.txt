cmake_minimum_required(VERSION 2.8.4)

project(dpsmat)

set(CMAKE_C_COMPILER mex)
set(CMAKE_C_FLAGS "")

file(GLOB SRCS "src/*.c")

set(DEPENDENCIES "dps_proxy dpservo")

execute_process(COMMAND echo "mexext"
                COMMAND matlab -nojvm
                COMMAND grep "^mex"
                COMMAND tr -d "\n"
                OUTPUT_VARIABLE MEXEXT)
message(STATUS "mex file extension is : " ${MEXEXT})

foreach(SRC ${SRCS})
  message(STATUS ${SRC})
  string(REGEX REPLACE "(.+/)([^/]+)\\.c$" "\\2" name ${SRC})
  message(STATUS ": " ${name})

  set(TARGET ${name}.${MEXEXT})
  
  # cmake recognize ${TARGET} as a output of custom_command.
  # so we should set ${TARGET} correct name same as output of mex command.
  # for it, we get mex file extension with mexext command in matlab, then use it in ${TARGET}.
  add_custom_command(OUTPUT ${TARGET}
    COMMAND mex CFLAGS='$$CFLAGS -std=gnu99 -Wall -O2 -D_GNU_SOURCE' ${SRC}
                -I${dpservo_SOURCE_DIR}/h/dpservo
                -I${dps_proxy_SOURCE_DIR}/h/dps_proxy
                -ldps_proxy -L${dps_proxy_BINARY_DIR}
                -ldpservo -L${dpservo_BINARY_DIR}
  )

  add_custom_target(${name} ALL DEPENDS ${TARGET})

  message(STATUS ": " ${name} " with " ${DEPENDENCIES})

  add_dependencies(${name} ${DEPENDENCIES})
  target_link_libraries(${name} ${DEPENDENCIES})

  list(APPEND MEX_FILES ${name}.${MEXEXT})

endforeach()

if(${UNIX})
    install(FILES ${MEX_FILES} DESTINATION mex)
elseif(${MINGW})
    install(FILES ${MEX_FILES} DESTINATION ${INSTALL_DIR})
endif()

