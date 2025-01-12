/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include <stdbool.h>

#include "rsx/rsx.h"

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs, uint8_t id) {
  EVALUE(NULL, hrs);

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  errno_t eno = rsx_check_connection(&rsx, hrs, id);

  if (eno == EOK) {
    printf("OK\n");
  } else {
    printf("NG\n");
  }

  // ECALL(rsx_reboot(&rsx, hrs, id));
  // ECALL(dump(&rsx));

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  
  char* dev  = "ttyUSB";
  char* unit = "0";
  if (argc >= 3) {
    dev  = argv[1];
    unit = argv[2];
  }
  printf("open %s %s\n", dev, unit);
  ECALL(hr_serial_open(&hrs, dev, unit));

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  rsx_device_info info;
  errno_t eno  = rsx_search_servo(&rsx, &hrs, &info);
  printf("result : %d: num_of_devices:%ld\n", eno, info.group[0].num_of_devices);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
