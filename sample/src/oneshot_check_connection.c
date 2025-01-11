/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx.h"

#include <stdbool.h>

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

#if 1
  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  int baudrate = -1;
  uint8_t   id = -1;
  errno_t eno  = rsx_search_servo(&rsx, &hrs, &baudrate, &id);
  printf("result : %d: baudrate:%d id:%02x\n", eno, baudrate, id);
#elif 1
  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));
  ECALL(hr_serial_set_baudrate(&hrs, 230400));

  uint8_t rid;
  ECALL(rsx_oneshot_read_byte(&rsx, &hrs, 0x01, 0x04, &rid));
  printf("---> %02x\n", rid);
#else
  uint8_t id = 0x01;
  int baudrates[] = {
    // 9600,
    // 14400,
    // 19200,
    // 28800,
    // 38400,
    // 57600,
    115200,
    // 153600,
    230400
  };
  printf("===== searching collect baudrate =====\n");
  for (int i = 0; i < sizeof(baudrates)/sizeof(baudrates[0]); i++) {
    printf(" %2d baudrate:%d", i, baudrates[i]);
    errno_t eno = hr_serial_set_baudrate(&hrs, baudrates[i]);
    if (eno != EOK) {
      printf(" -- skip\n");
      continue;
    }
    printf("\n");
    run_app(argc, argv, &hrs, id);
  }
#endif
  ECALL(hr_serial_close(&hrs));

  return 0;
}
