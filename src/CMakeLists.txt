cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0076 NEW)
target_sources(${PROJECT_NAME} PRIVATE
  rsx.c
  rsx_raw.c
  util/time.c
  serial/hr_serial.c
  serial/hr_unixio.c
)
