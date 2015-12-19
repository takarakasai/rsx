
#include "rsx.h"
#include "ics.h"

#include <unistd.h>

/*
 * position:
 * TX : CMD POSH POSL
 * RX :  =   =    =   CMD TCH_H TCH_L
 * 
 * CMD ==> 0 1 2 3 4 | 5 6 7
 *          SERVOID  |  CMD = 100xxxx (xxxx:servoid)
*/

errno_t ics_ser_set_pos_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos, size_t *serialized_size) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  /* base class member */
  dpservo *dps = get_dpservo(ics);

  //TODO: max_size check

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_CMD(id, cmdid);
  dps->buff[idx++] = ICS_SER_POS_H(pos);
  dps->buff[idx++] = ICS_SER_POS_L(pos);

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

errno_t ics_deser_set_pos_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos, ICS_UART_RATE baudrate) {
  EVALUE(NULL, ics);
  EVALUE(NULL, curpos);

  dpservo *dps = get_dpservo(ics);

  // size check
  //if (size < ICS_POS_CMD_REP_PKT_SIZE) {
  //  return EINVAL;
  //}

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
  if (dps->buff[idx++] != ICS_SER_REP_CMD(id, cmdid, baudrate)) {
    return EILSEQ;
  }

  *curpos = ICS_GET_POS(dps->buff[idx], dps->buff[idx + 1]);

  return EOK;
}

errno_t ics_write_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos) {
  EVALUE(NULL, ics);

  dpservo *dps = get_dpservo(ics);

  size_t size;
  ECALL(ics_ser_set_pos_cmd(ics, id, cmdid, pos, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_write_read_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos) {
  EVALUE(NULL, ics);

  dpservo *dps = get_dpservo(ics);

  ECALL(ics_write_pos(ics, id, cmdid, refpos));
  //ECALL(ics_ser_set_pos_cmd(id, cmdid, refpos, ics->max_size, ics->buff, &size));
  //ECALL(hr_serial_write(ics->hrs, ics->buff, ICS_POS_CMD_REQ_PKT_SIZE));

  ECALL(hr_serial_read(dps->hrs, dps->buff, ICS_POS_CMD_REP_PKT_SIZE));
  ECALL(data_dump(dps->buff, ICS_POS_CMD_REP_PKT_SIZE));
  if (curpos) {
    ECALL(ics_deser_set_pos_cmd(ics, id, cmdid, refpos, curpos, dps->hrs->baudrate));
    //printf(" ref:%d vs cur:%d\n", refpos, *curpos);
  }

  return EOK;
}

errno_t ics_ser_get_param_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint8_t scmdid, size_t *serialized_size) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  dpservo *dps = get_dpservo(ics);

  //TODO: max_size check

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_CMD(id, cmdid);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(scmdid);

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_get_param_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint8_t scmdid, uint8_t size, uint8_t rdata[], ICS_UART_RATE baudrate) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  //EVALUE(NULL, serialized_size);

  dpservo *dps = get_dpservo(ics);

  //TODO: size / max_size check

  size_t idx = 0;

  /* request data */
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_CMD(id, cmdid), EILSEQ);
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  /* reply data */
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REP_CMD(id, cmdid, baudrate), EILSEQ);
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  switch (scmdid) {
    case ICS_SCMD_EEPROM:
    {
      EVALUE(size, idx + 64);
      //EVALUE(max_size, 64); TODO: range check
      for (size_t i = 0; i < 64; i++) {
        rdata[i] = dps->buff[idx++];
      }
    }
      break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:
      EVALUE(size, idx + 1);
      //EVALUE(max_size, 1); TODO: range check
      rdata[0] = dps->buff[idx++];
      break;
    default:
      break;
  }

  //*serialized_size = idx;

  return EOK;
}

int ics_test(ics *ics) {
  EVALUE(NULL, ics);

  //uint8_t data[100] = {0};
  //size_t size;

  //for (size_t i = 0; i < 10; i++) {
  //  ics_ser_set_pos_cmd (rsx, 0, ICS_CMD_REQ_POS, i * 100, 100, data, &size);

  //  usleep(300 * 1000);
  //}
  //
  const uint8_t id = 1;
  uint16_t refpos, curpos = 0;

  //ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 6000, 100, data, &size);
  refpos = 6000;
  ics_write_read_pos (ics, id, ICS_CMD_REQ_POS, refpos, &curpos);
  usleep(1000 * 1000);

  //ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 7500, &size);
  refpos = 7500;
  ics_write_read_pos (ics, id, ICS_CMD_REQ_POS, refpos, &curpos);

  usleep(1000 * 1000);
  
  //ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 6000, &size);
  refpos = 6000;
  ics_write_read_pos (ics, id, ICS_CMD_REQ_POS, refpos, &curpos);

  usleep(1000 * 1000);

  //ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 7500, &size);
  refpos = 7500;
  ics_write_read_pos (ics, id, ICS_CMD_REQ_POS, refpos, &curpos);

  usleep(1000 * 1000);

//  for (size_t i = 0; i < 10; i++) {
//    ics_ser_set_pos_cmd (rsx, 0, ICS_CMD_REQ_POS, (10-i) * 100, 100, data, &size);
//
//    usleep(100 * 1000);
//  }

  for (size_t i = 0; i < 10; i++) {
    //ics_ser_set_pos_cmd (id, ICS_CMD_REQ_POS, 0, &size);
    ics_write_read_pos (ics, id, ICS_CMD_REQ_POS, 0, &curpos);
    usleep(100 * 1000);
  }

  return 0;
}

errno_t ics_init (ics *ics) {
  EVALUE(NULL, ics);

  // TODO:
  //dpservo *dps = get_dpservo(ics);
  //ECALL(dpservo_ops_init(&(dps->ops), set_state, set_states, set_goal, set_goals, write_mem, read_mem));

  ics->retry_count = 3;

  return EOK;
}

