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

  uint8_t ids[] = {0x01, 0x02};

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  size_t count = 0;
  do {
    ECALL(get_current_status_all(&rsx, hrs, ids, 2));
    usleep(100 * 1000);
  } while(count++ < 100);

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  if (argc >= 3) {
    ECALL(hr_serial_open(&hrs, argv[1], argv[2]));
  } else {
    ECALL(hr_serial_open(&hrs, "ttyUSB", "1"));
  }

  run_app(argc, argv, &hrs);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
