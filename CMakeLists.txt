cmake_minimum_required(VERSION 3.13)

project(rsx VERSION 0.1.0 LANGUAGES C)

option(ENABLE_SAMPLE "enable to build sample app" ON)

option(HR_SERIAL_AUTO_READ_ECHO_DATA "" OFF)
if(${HR_SERIAL_AUTO_READ_ECHO_DATA})
    add_definitions(-DHR_SERIAL_AUTO_READ_ECHO_DATA)
endif()

#include_directories(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED ${SRCS})
add_library(${PACKAGE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_subdirectory(src)
include(GenerateExportHeader)
set(${PROJECT_NAME}_EXPORT_FILE_NAME export.h)
generate_export_header(${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Werror" "-O2" "-std=gnu99")
target_compile_definitions(
  ${PROJECT_NAME}
    PRIVATE)

# target_compile_features(${PROJECT_NAME}
#   PUBLIC c_std_17
# )
set_target_properties(${PROJECT_NAME} PROPERTIES
  C_EXTENSIONS OFF
)

##target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION "0.1"
  SOVERSION "0.1"
  PUBLIC_HEADER ${PROJECT_BINARY_DIR}/$<LOWER_CASE:${PROJECT_NAME}>_export.h
)
if(${PROJECT_NAME}_DEPENDENCIES)
  #add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_DEPENDENCIES})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_DEPENDENCIES})
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PACKAGE_NAME}Config
  LIBRARY
   DESTINATION lib
   COMPONENT runtime
  PUBLIC_HEADER
   DESTINATION include/${PROJECT_NAME}
   COMPONENT development
  ARCHIVE
   DESTINATION lib
   COMPONENT runtime
  RUNTIME
   DESTINATION bin
   COMPONENT runtime
)
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION include
  COMPONENT development
)
install(
  EXPORT ${PACKAGE_NAME}Config
  NAMESPACE ${PACKAGE_NAME}::
  DESTINATION share/cmake/${PACKAGE_NAME}
  COMPONENT development
  EXPORT_LINK_INTERFACE_LIBRARIES
)
install(
  FILES ${PROJECT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
  DESTINATION share/cmake/${PACKAGE_NAME}
  COMPONENT development
)

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

