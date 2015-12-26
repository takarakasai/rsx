
#include "helper.h"

/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>
#include <string.h>
/* for strtoul */
#include <stdlib.h>

#include "ics/ics.h"

errno_t print_help (int argc, char *argv[]) {
  printf(" %s device port [ID]\n", argv[0]);
  printf("     : get id\n");
  printf("  ID : set id\n");
  return EOK;
}

/*
 * ./build/sample/ics_test ttyUSB 0 low 255 -30 30 
 */

int main(int argc, char *argv[]) {
  DPSERVO_DECL(servo, 20, 1024, ICS_DECL);
  DPSERVO_INIT(servo, ICS_INIT);
  
  uint8_t id = 1;

  int argc_offset;
  if (EOK != dps_setup(servo, argc, argv, &argc_offset)) {
    print_help(argc, argv);
    return 0;
  }

  if (argc >= 1 + argc_offset) {
    id = (uint8_t)strtoul(argv[argc_offset], NULL, 10);

    printf(" SET ID: %d\n", id);

    ECALL(dps_set_id(servo, id));
  } else {
    uint8_t id;

    ECALL(dps_get_id(servo, &id));

    printf(" GET ID: %d\n", id);
  }

  ECALL(dps_teardown(servo, argc, argv));

  return 0;
}

