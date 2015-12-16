
#include "rsx.h"
#include "ics.h"

#include <unistd.h>

typedef struct {
  hr_serial *hrs;
  uint8_t *buff;//[/*max_size*/];
  size_t max_size;

  size_t retry_count;

  //rsx_pkt *lpkt;
  //rsx_pkt *spkt;

  /* debug */
  bool use_serial;
} ics;

/*
 * position:
 * TX : CMD POSH POSL
 * RX :  =   =    =   CMD TCH_H TCH_L
 * 
 * CMD ==> 0 1 2 3 4 | 5 6 7
 *          SERVOID  |  CMD = 100xxxx (xxxx:servoid)
*/

//errno_t ics_ser_set_pos_cmd (rsx *x, uint8_t id, uint8_t cmdid, uint16_t pos, uint8_t max_size, uint8_t data[/*max_size*/], size_t *serialized_size) {
errno_t ics_ser_set_pos_cmd (uint8_t id, uint8_t cmdid, uint16_t pos, uint8_t max_size, uint8_t data[/*max_size*/], size_t *serialized_size) {
  EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  //TODO: max_size check

  size_t idx = 0;
  data[idx++] = ICS_SER_REQ_CMD(id, cmdid);
  data[idx++] = ICS_SER_POS_H(pos);
  data[idx++] = ICS_SER_POS_L(pos);

  *serialized_size = idx;

  //for (size_t i = 0; i < *serialized_size; i++) {
  //  printf(" %02x", data[i]);
  //}
  //printf("\n");
  //ECALL(hr_serial_write(x->hrs, data, *serialized_size));

  return EOK;
}

#define ICS_POS_CMD_REQ_PKT_SIZE 3 /* [byte] */
#define ICS_POS_CMD_REP_PKT_SIZE 3 /* [byte] */

errno_t ics_deser_set_pos_cmd (uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos, uint8_t size, uint8_t data[/*size*/], ICS_UART_RATE baudrate) {
  EVALUE(NULL, curpos);
  EVALUE(NULL, data);

  // size check
  if (size < ICS_POS_CMD_REP_PKT_SIZE) {
    return EINVAL;
  }

  size_t idx = 0;
#if 0
  /* loop back data */
  if (data[idx++] != ICS_SER_REQ_CMD(id, cmdid)) {
    return EILSEQ;
  }

  if (refpos != ICS_GET_POS(data[idx], data[idx + 1])) {
    return EILSEQ;
  }
  idx += 2;
#endif

  /* reply data */
  if (data[idx++] != ICS_SER_REP_CMD(id, cmdid, baudrate)) {
    return EILSEQ;
  }

  *curpos = ICS_GET_POS(data[idx], data[idx + 1]);

  return EOK;
}

errno_t ics_write_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos) {
  EVALUE(NULL, ics);

  size_t size;
  ECALL(ics_ser_set_pos_cmd(id, cmdid, pos, ics->max_size, ics->buff, &size));

  for (size_t i = 0; i < size; i++) {
    printf(" %02x", ics->buff[i]);
  }
  printf("\n");
  ECALL(hr_serial_write(ics->hrs, ics->buff, size));

  return EOK;
}

errno_t ics_write_read_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos) {
  EVALUE(NULL, ics);

  size_t size;
  ECALL(ics_ser_set_pos_cmd(id, cmdid, pos, ics->max_size, ics->buff, &size));

  ECALL(hr_serial_write(ics->hrs, ics->buff, size));

  ECALL(hr_serial_read(ics->hrs, ics->buff, size));

  for (size_t i = 0; i < size; i++) {
    printf(" %02x", ics->buff[i]);
  }
  printf("\n");

  return EOK;
}

errno_t ics_ser_get_param_cmd (uint8_t id, uint8_t cmdid, uint8_t scmdid, uint8_t max_size, uint8_t data[/*max_size*/], size_t *serialized_size) {
  EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  //TODO: max_size check

  size_t idx = 0;
  data[idx++] = ICS_SER_REQ_CMD(id, cmdid);
  data[idx++] = ICS_SER_REQ_SUBCMD(scmdid);

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_get_param_cmd (uint8_t id, uint8_t cmdid, uint8_t scmdid, uint8_t size, uint8_t data[/*max_size*/], uint8_t max_size, uint8_t rdata[], ICS_UART_RATE baudrate) {
  EVALUE(NULL, data);
  //EVALUE(NULL, serialized_size);

  //TODO: size / max_size check

  size_t idx = 0;

  /* request data */
  EVALUE_ERRNO(data[idx++], ICS_SER_REQ_CMD(id, cmdid), EILSEQ);
  EVALUE_ERRNO(data[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  /* reply data */
  EVALUE_ERRNO(data[idx++], ICS_SER_REP_CMD(id, cmdid, baudrate), EILSEQ);
  EVALUE_ERRNO(data[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  switch (scmdid) {
    case ICS_SCMD_EEPROM:
    {
      EVALUE(size, idx + 64);
      EVALUE(max_size, 64);
      for (size_t i = 0; i < 64; i++) {
        data[i] = data[idx++];
      }
    }
      break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:
      EVALUE(size, idx + 1);
      EVALUE(max_size, 1);
      rdata[0] = data[idx++];
      break;
    default:
      break;
  }

  //*serialized_size = idx;

  return EOK;
}

int ics_test(rsx *rsx) {
  EVALUE(NULL, rsx);

  uint8_t data[100] = {0};
  size_t size;

  //for (size_t i = 0; i < 10; i++) {
  //  ics_ser_set_pos_cmd (rsx, 0, ICS_CMD_REQ_POS, i * 100, 100, data, &size);

  //  usleep(300 * 1000);
  //}
  //
  const uint8_t id = 1;

  ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 6000, 100, data, &size);
  usleep(1000 * 1000);

  ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 7500, 100, data, &size);

  usleep(1000 * 1000);
  
  ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 6000, 100, data, &size);

  usleep(1000 * 1000);

  ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 7500, 100, data, &size);

  usleep(1000 * 1000);

//  for (size_t i = 0; i < 10; i++) {
//    ics_ser_set_pos_cmd (rsx, 0, ICS_CMD_REQ_POS, (10-i) * 100, 100, data, &size);
//
//    usleep(100 * 1000);
//  }

  for (size_t i = 0; i < 10; i++) {
    ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 0, 100, data, &size);
    usleep(100 * 1000);
  }

  return 0;
}

