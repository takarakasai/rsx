/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

// #define DATA_DUMP
#include "rsx.h"

#include <stdbool.h>

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs) {
  EVALUE(NULL, hrs);

  size_t count = 0;

  uint8_t id = 0x01;

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  usleep(200*1000);

  ECALL(rsx_servo_set_control_mode(&rsx, hrs, id, RSX_MIX_CONTROL));

  ECALL(rsx_servo_on(&rsx, hrs, id));

  do {
    rsx_set_goal_torque(&rsx, hrs, id, +6.0 /* [%] */);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current_status(&rsx, hrs, id));
      usleep(100 * 1000);
    }

    rsx_set_goal_torque(&rsx, hrs, id, -6.0 /* [%] */);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current_status(&rsx, hrs, id));
      usleep(100 * 1000);
    }
  } while(count++ < 5);

  ECALL(rsx_servo_off(&rsx, hrs, id));

  printf("\033[5B");

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  ECALL(hr_serial_set_baudrate(&hrs, 230400));

  if (argc >= 3) {
    ECALL(hr_serial_open(&hrs, argv[1], argv[2]));
  } else {
    ECALL(hr_serial_open(&hrs, "ttyUSB", "1"));
  }

  run_app(argc, argv, &hrs);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
