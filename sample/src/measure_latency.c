/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

// #define DATA_DUMP
#include <rsx/rsx.h>

#include <stdbool.h>

#include "util.h"

#define NUM_OF_COMMANDS (100)

#define REQ_PKT_SIZE (8)
#define REP_PKT_SIZE (16)

// #define WO_RSX

int measure_wo_rsx(hr_serial* hrs) {
  EVALUE(NULL, hrs);

  printf("================== W/O RSX ================\n");

  uint8_t wdata[REQ_PKT_SIZE] = {0xFA, 0xAF, 0x01 ,0x0F, 0x2A, 0x08, 0x00, 0x2c};
  uint8_t rdata[REP_PKT_SIZE] = {0};
  // size:0016 :: fd df 01 00 2a 08 01 28 ff 00 00 3e 00 0c 00 c7

  double diff_usec_send = 0.0;
  double diff_usec_recv = 0.0;
  double max_usec_send = 0.0;
  double max_usec_recv = 0.0;

  rsx_timespec time1, time2, time3;
  rsx_timespec diff1, diff2, diff3;

  for (size_t i = 0; i < NUM_OF_COMMANDS; i++) {
    int eno = -1;
    ECALL(rsx_gettime(&time1));
    ECALL(hr_serial_write(hrs, wdata, REQ_PKT_SIZE));
    ECALL(rsx_gettime(&time2));
    do {
      eno = hr_serial_read(hrs, rdata, REP_PKT_SIZE);
    } while(eno != 0);
    ECALL(rsx_gettime(&time3));

    printf("Count        : %zd\n", i);
    printf("Before Send  : %ld.%09ld [sec]\n", time1.sec, time1.nsec);
    printf("Before Recv  : %ld.%09ld [sec]\n", time2.sec, time2.nsec);
    printf("After  Recv  : %ld.%09ld [sec]\n", time3.sec, time3.nsec);

    ECALL(rsx_difftime(&time2, &time1, &diff1));
    ECALL(rsx_difftime(&time3, &time2, &diff2));
    ECALL(rsx_difftime(&time3, &time1, &diff3));

    printf("Latency     Send : %ld.%09ld [sec]\n", diff1.sec, diff1.nsec);
    printf("Latency     Recv : %ld.%09ld [sec]\n", diff2.sec, diff2.nsec);
    printf("Latency SendRecv : %ld.%09ld [sec]\n", diff3.sec, diff3.nsec);

    double diff_send = (diff1.sec * 1000 * 1000 + diff1.nsec / 1000.0);
    double diff_recv = (diff2.sec * 1000 * 1000 + diff2.nsec / 1000.0);
    diff_usec_send += diff_send;
    diff_usec_recv += diff_recv;

    if (diff_send > max_usec_send) {
      max_usec_send = diff_send;
    }
    if (diff_recv > max_usec_recv) {
      max_usec_recv = diff_recv;
    }

    if (i != NUM_OF_COMMANDS - 1 ) {
      printf("\033[7A");
    }
  }

  double result_send = (diff_usec_send / NUM_OF_COMMANDS / 1000);
  double result_recv = (diff_usec_recv / NUM_OF_COMMANDS / 1000);
  printf("\n");
  printf("Latency     Send: AVG:%10.5lf  MAX:%10.5lf [msec]\n", result_send, max_usec_send / 1000);
  printf("Latency     Recv: AVG:%10.5lf  MAX:%10.5lf [msec]\n", result_recv, max_usec_recv / 1000);
  printf("Latency SendRecv: AVG:%10.5lf  MAX:%10.5lf [msec]\n",  //
         result_send + result_recv, (max_usec_send + max_usec_recv) / 1000);

  return 0;
}

int measure_w_rsx(hr_serial* hrs) {
  EVALUE(NULL, hrs);

  printf("================== W/  RSX ================\n");

  rsx rsx;
  ECALL(rsx_init(&rsx, NULL));
  uint16_t data[4];

  double diff_usec_sendrecv = 0.0;
  double max_usec_sendrecv = 0.0;

  rsx_timespec time1, time2;
  rsx_timespec diff1;

  for (size_t i = 0; i < NUM_OF_COMMANDS; i++) {
#if 1
    ECALL(rsx_gettime(&time1));
    ECALL(rsx_oneshot_read_words(&rsx, hrs, 0x01, 0x2A, sizeof(data)/sizeof(data[0]), data));
    // ssize_t retry_count = rsx_get_retry_count(rsx);
    ECALL(rsx_gettime(&time2));
#else
    ECALL(rsx_gettime(&time1));
    ECALL(rsx_oneshot_read_words(&rsx, hrs, 0x01, 0x2A, sizeof(data)/sizeof(data[0]), data));
    ECALL(rsx_oneshot_read_words(&rsx, hrs, 0x02, 0x2A, sizeof(data)/sizeof(data[0]), data));
    ECALL(rsx_gettime(&time2));
#endif

    printf("Count        : %zd\n", i);
    printf("Before Send  : %ld.%09ld [sec]\n", time1.sec, time1.nsec);
    printf("Before Recv  : n/a\n");
    printf("After  Recv  : %ld.%09ld [sec]\n", time2.sec, time2.nsec);

    ECALL(rsx_difftime(&time2, &time1, &diff1));

    printf("Latency Send     : n/a\n");
    printf("Latency Recv     : n/a\n");
    printf("Latency SendRecv : %ld.%09ld [sec]\n", diff1.sec, diff1.nsec);

    double diff_sendrecv = (diff1.sec * 1000 * 1000 + diff1.nsec / 1000.0);
    diff_usec_sendrecv += diff_sendrecv;

    if (diff_sendrecv > max_usec_sendrecv) {
      max_usec_sendrecv = diff_sendrecv;
    }

    if (i != NUM_OF_COMMANDS - 1 ) {
      printf("\033[7A");
    }
  }

  printf("-- result\n");
  printf("Latency Send: n/a\n");
  printf("Latency Recv: n/a\n");
  printf("Latency Recv: AVG:%10.5lf  MAX:%10.5lf [msec]\n",  //
         (diff_usec_sendrecv / NUM_OF_COMMANDS / 1000), max_usec_sendrecv / 1000);

  return 0;
}

int run_app(int argc, char *argv[], hr_serial *hrs) {
  EVALUE(NULL, hrs);

  measure_wo_rsx(hrs);
  measure_w_rsx(hrs);

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
