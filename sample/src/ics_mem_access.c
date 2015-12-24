
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>
/* for strtoul */
#include <stdlib.h>

#include "ics.h"

#include "mmap/ics3x.h"

/*
 * ./build/sample/ics_test ttyUSB 0 low 255 -30 30 
 */

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  
  bool use_serial = true;

  char *device = "ttyUSB";
  char *port   = "0";
  hr_baudrate baudrate = HR_B115200;
  bool is_write = false;
  uint8_t id = 1;
  uint8_t start_addr = 0x00;
  uint8_t rwsize = 0;

  if (argc >= 8) {
    device = argv[1];
    port   = argv[2];

    if (strcmp(argv[3], "high") == 0) {
      baudrate = HR_B1250000;
    } else if(strcmp(argv[3], "mid") == 0) {
      baudrate =  HR_B625000;
    } else {
      baudrate =  HR_B115200;
    }

    if (strcmp(argv[4], "write") == 0) {
      is_write = true;
    }

    id = (uint8_t)strtoul(argv[5], NULL, 10);

    start_addr = (uint8_t)strtoul(argv[6], NULL, 16);

    rwsize = (uint8_t)strtoul(argv[7], NULL, 10);

  } else {
    fprintf(stdout, "[USAGE] %s dev port high|mid|low write|read id start_addr size [data] \n", argv[0]);
    return 0;
  }

  uint8_t data[rwsize];
  memset(data, 0x00, rwsize);
  if (is_write) {
    if (argc == 8) {
      fprintf(stdout, "[USAGE] %s dev port high|mid|low write|read id start_addr size [data] \n", argv[0]);
      return 0;
    }
    size_t idx = 0;
    for (size_t i = 8; i < argc && idx < rwsize; i++) {
      data[idx++] = (uint8_t)strtoul(argv[i], NULL, 16);
    }
  }

  printf("===: %s/%s with %s(%s)\n", device, port, hr_baudrate2str(baudrate), argv[3]);
  printf("===:   ID: %d\n", id);
  printf("===: ADDR: 0x%02x\n", start_addr);
  printf("===: SIZE: %d\n", rwsize);
  if (is_write) {
    printf("===: DATA:");
    for (size_t i = 0; i < rwsize; i++) {
      printf(" %02x", data[i]);
    }
    printf("\n");
  }

  ECALL(dps_open(servo, device, port, baudrate, HR_PAR_EVEN));

  ECALL(dps_set_serial(servo, use_serial));

  if (is_write) {
    ECALL(dps_mem_write(servo, id, start_addr, rwsize, data, ICS_VER_30));
    printf("----- write done -----\n");
  }

  ECALL(dps_mem_read(servo, id, start_addr, rwsize, data, ICS_VER_30));
  for (size_t i = 0; i < rwsize; i++) {
    printf(" %02zd", start_addr + i);
  }
  printf("\n");

  for (size_t i = 0; i < rwsize; i++) {
    printf(" --");
  }
  printf("\n");

  for (size_t i = 0; i < rwsize; i++) {
    printf(" %02x", data[i]);
  }
  printf("\n");

  ECALL(dps_close(servo));

  return 0;
}

