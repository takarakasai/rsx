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
  ECALL(rsx_init(&rsx, NULL));

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
