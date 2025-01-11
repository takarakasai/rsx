#include "util.h"

#include <stdbool.h>

errno_t dump (rsx* rsx) {
  EVALUE(NULL, rsx);

  printf("wbuff:size:%04zd ::", rsx->write_size);
  for (size_t i = 0; i < rsx->write_size; i++) {
    printf(" %02x", rsx->wbuff[i]);
  }
  printf("\n");

  printf("rbuff:size:%04zd ::", rsx->read_size);
  for (size_t i = 0; i < rsx->read_size; i++) {
    printf(" %02x", rsx->rbuff[i]);
  }
  printf("\n");


  return EOK;
}

#if 0
errno_t search_servo (rsx* rsx, hr_serial* hrs, uint8_t* id) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);
  EVALUE(NULL, id);

  int baudrates[] = {
    9600,
    14400,
    19200,
    28800,
    38400,
    57600,
    115200,
    153600,
    230400
  };

  printf("===== searching servo =====\n");
  for (int i = 0; i < sizeof(baudrates)/sizeof(baudrates[0]); i++) {
    printf("== %2d baudrate:%d", i, baudrates[i]);
    errno_t eno = hr_serial_set_baudrate(hrs, baudrates[i]);
    if (eno != EOK) {
      printf(" -- skip\n");
      continue;
    }
    printf("\n");

    for (uint8_t r_id = 0; r_id < 32; r_id++) {
      printf(" %02x", r_id); fflush(stdout);
      eno = rsx_check_connection(rsx, hrs, r_id);
      if (eno == EOK) {
        printf("Found : id:%0x\n", r_id);
        *id   = r_id;
        return EOK;
      }
    }
    printf("\n");
  }

  return -1;
}
#endif

errno_t get_current_status_all (rsx* rsx, hr_serial* hrs, uint8_t* id, uint8_t num) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  static int read_count = 0;

  ssize_t retry_count[num];
  errno_t eno[num];
  uint16_t data[num][4];
  for (size_t i = 0; i < num; i++) {
    eno[i] = rsx_oneshot_read_words(rsx, hrs, id[i], 0x2A,
                      sizeof(data[i])/sizeof(data[i][0]), data[i]);
    retry_count[i] = rsx_get_retry_count(rsx);
  }

  // printf("\033[5B");

  printf("ID(err,retry):");
  for (size_t i = 0; i < num; i++) {
    printf(" %02x(%4d,%3ld)", id[i], eno[i], retry_count[i]);
  }
  printf("Count : %d\n", read_count++);

  printf("Current Pos  :");
  for (size_t i = 0; i < num; i++) {
    printf(" %+7.2lf (0x%04x)", (int16_t)(data[i][0]) / 10.0f, data[i][0]);
  }
  printf(" [deg]\n");

  printf("Current Time :");
  for (size_t i = 0; i < num; i++) {
    printf(" %+7d (0x%04x)", data[i][1] * 10, data[i][1]);
  }
  printf(" [msec]\n");

  printf("Current Vel  :");
  for (size_t i = 0; i < num; i++) {
    printf(" %+7d (0x%04x)", (int16_t)(data[i][2]), data[i][2]);
  }
  printf(" [rpm]\n");

  printf("Current Trq  :");
  for (size_t i = 0; i < num; i++) {
    printf(" %+7.2lf (0x%04x)", data[i][3] * 0.1, data[i][3]);
  }
  printf(" [%%]\n");

  // printf("\033[5A");

  return EOK;
}

errno_t get_current_status (rsx* rsx, hr_serial* hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  static int read_count = 0;

  uint16_t data[4];
  errno_t eno = rsx_oneshot_read_words(rsx, hrs, id, 0x2A, sizeof(data)/sizeof(data[0]), data);
  if (eno == EOK) {
    // printf("\033[5B"
    printf("ID: %02x, Count : %d\n", id, read_count++);
    printf("Current Pos  :%+7.2lf (0x%04x) [deg]\n", (int16_t)(data[0]) / 10.0f, data[0]);
    printf("Current Time :%+7d (0x%04x) [msec]\n", data[1] * 10, data[1]);
    printf("Current Vel  :%+7d (0x%04x) [rpm]\n", (int16_t)(data[2]), data[2]);
    printf("Current Trq  :%+7.3lf (0x%04x) [%%]\n", data[3] * 0.1, data[3]);
    // printf("\033[5A");
  } else {
    printf("Get status error\n");
  }

  return EOK;
}

int GetDeviceInfo(rsx* rsx, hr_serial* hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  uint8_t data[3];
  ECALL(rsx_oneshot_read_bytes(rsx, hrs, id, 0x00, sizeof(data)/sizeof(data[0]), data));

  printf("Model Number         : L:%02x H:%02x\n", data[0], data[1]);
  printf("Firmware Version     : %02x\n", data[2]);

  ECALL(rsx_oneshot_read_bytes(rsx, hrs, id, 0x04, sizeof(data)/sizeof(data[0]), data));
  printf("ServoID              : %02x\n", data[0]);
  printf("Reverse              : %02x\n", data[1]);
  printf("BaudRate             : %02x\n", data[2]);

  ECALL(rsx_oneshot_read_bytes(rsx, hrs, id, 0x15, sizeof(data)/sizeof(data[0]), data));
  printf("Veloc/Torque Enabled : %02x\n", data[0]);
  printf("Torque in Silence    : %02x\n", data[1]);
  printf("Wake-up Time         : %02x\n", data[2]);

  return 0;
}

int GetMemInfo(rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t size) {
  uint8_t data[size];
  ECALL(rsx_oneshot_read_bytes(rsx, hrs, id, addr, size, data));

  printf("   ");
  for (uint16_t i = 0; i < 0x10; i++) {
    printf(" %02x", i + addr);
  }
  printf("\n");
  for (uint16_t i = 0; i < 0x10; i++) {
    printf("---");
  }
  printf("\n");
  for (uint16_t i = 0; i < size; i++) {
    if (i % 0x10 == 0) {
      if (i != 0) {
        printf("\n");
      }
      printf("%02x|", i + addr);
    }
    printf(" %02x", data[i]);
  }
  printf("\n");

  return 0;
}

int GetROMInfo(rsx* rsx, hr_serial* hrs, uint8_t id) {
  /* ROM: 0x00 - 0x1D (30[B]) */
  return GetMemInfo(rsx, hrs, id, 0x00, 30);
}

int GetRAMInfo(rsx* rsx, hr_serial* hrs, uint8_t id) {
  /* ROM: 0x1E - 0x3B (30[B]) */
  return GetMemInfo(rsx, hrs, id, 0x1E, 30);
}
