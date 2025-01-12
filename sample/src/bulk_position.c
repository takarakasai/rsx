/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include <stdbool.h>

// #define DATA_DUMP
#include "rsx/rsx.h"

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs) {
  EVALUE(NULL, hrs);

  uint8_t ids[] = {0x01, 0x02};
  float goals[][2] = {
    /* unit: [-3270.0, +3270.0][deg] */
    {-90.0, -90.0},
    {+90.0, +90.0}
  };
 
  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  usleep(200*1000);

  for (size_t i = 0; i < sizeof(ids)/sizeof(ids[0]); i++) {
    ECALL(rsx_servo_set_control_mode(&rsx, hrs, ids[i], RSX_POS_CONTROL));
  }
  for (size_t i = 0; i < sizeof(ids)/sizeof(ids[0]); i++) {
    ECALL(rsx_servo_on(&rsx, hrs, ids[i]));
  }

  printf("start control\n");
  size_t count = 0;
  do {
    rsx_set_goal_positions(&rsx, hrs, ids, goals[0], 2);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current_status_all(&rsx, hrs, ids, 2));
      usleep(100 * 1000);
    }

    rsx_set_goal_positions(&rsx, hrs, ids, goals[1], 2);
    for (size_t i = 0; i < 5; i++) {
      ECALL(get_current_status_all(&rsx, hrs, ids, 2));
      usleep(100 * 1000);
    }
  } while(count++ < 5);
  printf("finish control\n");

  for (size_t i = 0; i < sizeof(ids)/sizeof(ids[0]); i++) {
    ECALL(rsx_servo_off(&rsx, hrs, ids[i]));
  }

  printf("\033[5B");

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  ECALL(hr_serial_set_baudrate(&hrs, 230400));

  char* dev  = "ttyUSB";
  char* unit = "0";
  if (argc >= 3) {
    dev  = argv[1];
    unit = argv[2];
  }
  printf("open %s %s\n", dev, unit);
  ECALL(hr_serial_open(&hrs, dev, unit));

  run_app(argc, argv, &hrs);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
