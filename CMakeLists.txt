cmake_minimum_required(VERSION 2.8.4)

project(rsx)

option(ENABLE_SAMPLE "enable to build sample app" ON)

option(HR_SERIAL_AUTO_READ_ECHO_DATA "" OFF)
if(${HR_SERIAL_AUTO_READ_ECHO_DATA})
    add_definitions(-DHR_SERIAL_AUTO_READ_ECHO_DATA)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})

file(GLOB SRCS "src/*.c")
file(GLOB SERIAL_SRCS "src/serial/*.c")
list(APPEND SRCS ${SERIAL_SRCS})

if(${CMAKE_COMPILER_IS_GNUCC})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -std=gnu99 -D_GNU_SOURCE")
endif()

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})

if(${ENABLE_TESTING})
    add_subdirectory(test)
endif()

if(${ENABLE_SAMPLE})
    add_subdirectory(sample)
endif()

if(${UNIX})
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/h/${PROJECT_NAME} DESTINATION include)
elseif(${MINGW})
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DIR})
endif()

