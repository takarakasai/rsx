
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
  uint8_t id = 1;
  uint8_t start_addr = 0x00;

  hr_baudrate baudrate = HR_B115200;

  char *device = "ttyUSB";
  char *port   = "0";

  if (argc >= 6) {
    device = argv[1];
    port   = argv[2];

    if (strcmp(argv[3], "high") == 0) {
      baudrate = HR_B1250000;
    } else if(strcmp(argv[3], "mid") == 0) {
      baudrate =  HR_B625000;
    } else {
      baudrate =  HR_B115200;
    }

    id = (uint8_t)strtoul(argv[4], NULL, 10);

    start_addr = (uint8_t)strtoul(argv[5], NULL, 16);
  }

  printf("===: %s/%s with %s(%s)\n", device, port, hr_baudrate2str(baudrate), argv[3]);
  printf("===:   ID: %d\n", id);
  printf("===: ADDR: 0x%02x\n", start_addr);
  ECALL(dps_open(servo, device, port, baudrate, HR_PAR_EVEN));

  ECALL(dps_set_serial(servo, use_serial));

  uint8_t data[1];
  ECALL(dps_mem_read(servo, 2, start_addr, 1, data));
  printf("=====> data[0] : %02x\n", data[0]);

  //ECALL(ics_mem_write

  ECALL(dps_close(servo));

  return 0;
}

