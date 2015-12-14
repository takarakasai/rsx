
/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx_pkt.h"
#include "rsx_io.h"

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif

#include <stdbool.h>

int run_test(int argc, char *argv[], hr_serial *hrs, bool use_serial) {
  EVALUE(NULL, hrs);

  uint8_t buff[1024];
  size_t size;

  /////// get Model Number & Firmware Virsion ///////

  RSX_SPKT_DECL(rpkt, 32);
  RSX_SPKT_INIT(rpkt);

  RSX_SPKT_SETID(rpkt, 1);
  RSX_SPKT_SETFLAG(rpkt, 0xF);
  RSX_SPKT_SETADDR(rpkt, 0x00);
  RSX_SPKT_SETLENGTH(rpkt, 4);

  ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
  ECALL(data_dump(buff, size));
  if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
  if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 4)); // TODO: size + 2
  ECALL(data_dump(buff, size + 4));                           // TODO: size + 2
  ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

  printf("Model Number L:%02x H:%02x\n", RSX_SPKT_GET_U8(rpkt, 0), RSX_SPKT_GET_U8(rpkt, 1));
  printf("Firmware Version:%02x\n", RSX_SPKT_GET_U8(rpkt, 2));

  /////// get mmap ////

  for (size_t id = 1; id <= 20; id++) {
    RSX_SPKT_INIT(rpkt);

    RSX_SPKT_SETID(rpkt, id);
    RSX_SPKT_SETFLAG(rpkt, 0xF);
    RSX_SPKT_SETADDR(rpkt, 0x06);
    //RSX_SPKT_SETADDR(rpkt, 0x07);
    RSX_SPKT_SETLENGTH(rpkt, 1);

    ECALL(rsx_pkt_ser(&rpkt, buff, sizeof(buff), &size));
    ECALL(data_dump(buff, size));
    if (use_serial) ECALL(hr_serial_write(hrs, buff, size));
    if (use_serial) ECALL(hr_serial_read(hrs, buff, size + 1)); // TODO: size + 2
    ECALL(data_dump(buff, size + 1));                           // TODO: size + 2
    ECALL(rsx_pkt_deser(&rpkt, buff, sizeof(buff), &size));

    printf("Recv Data [%02zd] :%02x\n", id, RSX_SPKT_GET_U8(rpkt, 0));
  }

  printf("----- end ----- \n");
  usleep(10 * 1000);

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  bool use_serial = false;

  if (argc >= 3) {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, argv[1], argv[2], HR_B460800, HR_PAR_NONE));
  } else {
    use_serial = true;
    ECALL(hr_serial_open(&hrs, "ttyUSB", "0", HR_B460800, HR_PAR_NONE));
  }

  run_test(argc, argv, &hrs, use_serial);

  if (use_serial) ECALL(hr_serial_close(&hrs));

  return 0;
}
