
#include "ics/ics.h"

#include "ics/ics_type.h"

#include "ics/mmap/ics3x.h"

#include <unistd.h>
#include <string.h>

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif

#ifdef __GNUC__
#define NOT_USED_FUNC __attribute__ ((unused))
#else
#define NOT_USED_FUNC
#endif

/*
 * position:
 * TX : CMD POSH POSL
 * RX :  =   =    =   CMD TCH_H TCH_L
 * 
 * CMD ==> 0 1 2 3 4 | 5 6 7
 *          SERVOID  |  CMD = 100xxxx (xxxx:servoid)
*/

#define ICS_POS_MAX       (135.0) /* [deg] */
#define ICS_POS_MIN      (-135.0) /* [deg] */
                                /* ICS spec   |  DP spec   | decimal       */
#define ICS_POS_MAX_HEX  0x2CEC /* 270[deg]  --> +135[deg] | 11500(0x2cec) */ /* 0x3FFF? */
#define ICS_POS_MID_HEX  0x1D4C /* 135[deg]  -->    0[deg] |  7500(0x1d4c) */ /* 0x2000? */
#define ICS_POS_MIN_HEX  0x0DAC /*   0[deg]  --> -135[deg] |  3500(0x0dac) */ /* 0x0001? */
#define ICS_POS_FREE     0x0000 /* servo free              */

#define ICS_POS2HEX(pos) ((pos) / ICS_POS_MAX * (ICS_POS_MAX_HEX - ICS_POS_MID_HEX) + ICS_POS_MID_HEX)
#define ICS_HEX2POS(hex) (((hex) - ICS_POS_MID_HEX) * ICS_POS_MAX / (ICS_POS_MAX_HEX - ICS_POS_MID_HEX))

#define ICS30
#if defined(ICS35)
#define ICS_EEPROM_SIZE 64
#elif defined(ICS30)
#define ICS_EEPROM_SIZE 58
#else
#error you should set ICS30 or ICS35
#endif


/*********************************************************************************************************************/

errno_t ics_ser_set_pos_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos, size_t *serialized_size) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  /* base class member */
  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_POS_CMD(id, cmdid);
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

  dpservo_base *dps = get_dpservo_base(ics);

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
  if (dps->buff[idx++] != ICS_SER_REP_POS_CMD(id, cmdid, baudrate)) {
    return EILSEQ;
  }

  *curpos = ICS_GET_POS(dps->buff[idx], dps->buff[idx + 1]);

  return EOK;
}

errno_t ics_write_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;
  ECALL(ics_ser_set_pos_cmd(ics, id, cmdid, pos, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t _ics_write_read_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  ECALL(ics_write_pos(ics, id, cmdid, refpos));

  ECALL2(hr_serial_read(dps->hrs, dps->buff, ICS_POS_CMD_REP_PKT_SIZE), false);
  ECALL(data_dump(dps->buff, ICS_POS_CMD_REP_PKT_SIZE));
  if (curpos) {
    //ECALL(ics_deser_set_pos_cmd(ics, id, cmdid, refpos, curpos, dps->hrs->baudrate)); // TODO:
    (ics_deser_set_pos_cmd(ics, id, cmdid, refpos, curpos, dps->hrs->baudrate)); // TODO:
  }

  return EOK;
}

errno_t ics_write_read_pos (ics *ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_write_read_pos(ics, id, cmdid, refpos, curpos);
    if (eno == EOK) {
      break;
    }
    printf(" pos--- id:%02d pos:%04x fpos:%lf %zd/%zd --failed\n", id, refpos, ICS_HEX2POS(refpos), i, max_count);
  }

  return EOK;
}

/*********************************************************************************************************************/

#define ICS_MEM_CMD_REP_PKT_HEADER_SIZE 4 /* [byte] */

static inline uint8_t mem_read_cmd_reppkt_size (uint8_t size) {
  return (ICS_MEM_CMD_REP_PKT_HEADER_SIZE + ICS_VMEM_PAYLOAD_SIZE(size));
}

errno_t ics_ser_mem_read_cmd (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize, size_t *serialized_size) {
  EVALUE(NULL, ics);
  ELTGT(ICS_VMEM_ADDR_MIN, ICS_VMEM_ADDR_MAX, start_addr);
  ELEGT(0, ICS_VMEM_MAX_SIZE(start_addr), rsize);
  EVALUE(NULL, serialized_size);

  /* base class member */
  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  /*
   * | _CMD | _SC_ | ADDR | BYTE |
   */

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_GPCMD(id, ICS_CMD_GP_READ); /* _CMD */
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_VIRTRAM);   /* _SC_ */
  dps->buff[idx++] = start_addr;
  dps->buff[idx++] = rsize;

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_mem_read_cmd (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize, uint8_t data[/*rsize*/]) {
  EVALUE(NULL, ics);
  EVALUE(NULL, data);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t idx = 0;

  /* reply data */
  if (dps->buff[idx++] != ICS_SER_REQ_GPCMD(id, ICS_CMD_GP_READ) ||
      dps->buff[idx++] != ICS_SER_REQ_SUBCMD(ICS_SCMD_VIRTRAM)   ||
      dps->buff[idx++] != start_addr ||
      dps->buff[idx++] != rsize) {
    return EILSEQ;
  }

  for (size_t i = 0; i < rsize; i++) {
    ICS_VMEM_DESER_PAYLOAD(dps->buff, idx, data, i);
  }

  return EOK;
}

errno_t ics_write_mem_read_req (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_mem_read_cmd(ics, id, start_addr, rsize, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_mem_read_rep (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize, uint8_t data[/*rsize*/]) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t expected_pkt_size = mem_read_cmd_reppkt_size(rsize);

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), true);
  //ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), false);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_mem_read_cmd(ics, id, start_addr, rsize, data));

  return EOK;
}

errno_t _ics_mem_read (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize, uint8_t data[/*rsize*/]) {
  EVALUE(NULL, ics);

  ECALL(ics_write_mem_read_req(ics, id, start_addr, rsize));
  ECALL2(ics_read_mem_read_rep(ics, id, start_addr, rsize, data), false);

  return EOK;
}

errno_t ics_mem_read (ics *ics, uint8_t id, uint8_t start_addr, uint8_t rsize, uint8_t data[/*rsize*/]) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_mem_read(ics, id, start_addr, rsize, data);
    if (eno == EOK) {
      break;
    }
    printf(" mem_read %zd/%zd --- id:%02d size:%d data[0]:%04x \n", i, max_count, id, rsize, data[0]);
  }

  return EOK;
}

/*********************************************************************************************************************/

static inline uint8_t mem_write_cmd_reppkt_size (void) {
  return (ICS_MEM_CMD_REP_PKT_HEADER_SIZE);
}

errno_t ics_ser_mem_write_cmd (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize, uint8_t data[/*wsize*/], size_t *serialized_size) {
  EVALUE(NULL, ics);
  ELTGT(ICS_VMEM_ADDR_MIN, ICS_VMEM_ADDR_MAX, start_addr);
  ELEGT(0, ICS_VMEM_MAX_SIZE(start_addr), wsize);
  EVALUE(NULL, serialized_size);
  EVALUE(NULL, data);

  /* base class member */
  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  /*
   * | _CMD | _SC_ | ADDR | BYTE | PAYLOAD (2 * BYTE [byte]) |
   */

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_GPCMD(id, ICS_CMD_GP_WRITE); /* _CMD */
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_VIRTRAM);    /* _SC_ */
  dps->buff[idx++] = start_addr;
  dps->buff[idx++] = wsize;

  for (size_t i = 0; i < wsize; i++) {
    ICS_VMEM_SER_PAYLOAD(dps->buff, idx, data, i);
  }

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_mem_write_cmd (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize) {
  EVALUE(NULL, ics);
  ELTGT(ICS_VMEM_ADDR_MIN, ICS_VMEM_ADDR_MAX, start_addr);
  ELEGT(0, ICS_VMEM_MAX_SIZE(start_addr), wsize);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t idx = 0;

  /* reply data */
  if (dps->buff[idx++] != ICS_SER_REQ_GPCMD(id, ICS_CMD_GP_WRITE) ||
      dps->buff[idx++] != ICS_SER_REQ_SUBCMD(ICS_SCMD_VIRTRAM)    ||
      dps->buff[idx++] != start_addr ||
      dps->buff[idx++] != wsize) {
    return EILSEQ;
  }

  return EOK;
}

errno_t ics_write_mem_write_req (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize, uint8_t data[/*wsize*/]) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_mem_write_cmd(ics, id, start_addr, wsize, data, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_mem_write_rep (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t expected_pkt_size = mem_write_cmd_reppkt_size();

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), false);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_mem_write_cmd(ics, id, start_addr, wsize));

  return EOK;
}

errno_t _ics_mem_write (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize, uint8_t data[/*wsize*/]) {
  EVALUE(NULL, ics);

  ECALL(ics_write_mem_write_req(ics, id, start_addr, wsize, data));
  ECALL2(ics_read_mem_write_rep(ics, id, start_addr, wsize), false);

  return EOK;
}

errno_t ics_mem_write (ics *ics, uint8_t id, uint8_t start_addr, uint8_t wsize, uint8_t data[/*wsize*/]) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_mem_write(ics, id, start_addr, wsize, data);
    if (eno == EOK) {
      break;
    }
    printf(" mem_write --- id:%02d size:%d data[0]:%04x \n", id, wsize, data[0]);
  }

  return EOK;
}

/*********************************************************************************************************************/

#define ICS_GET_PARAM_CMD_REP_PKT_HEADER_SIZE 2 /* [byte] */

static inline uint8_t get_param_reppkt_size (uint8_t scmdid, ics_opt_t option) {

  uint8_t size = ICS_GET_PARAM_CMD_REP_PKT_HEADER_SIZE;

  switch (scmdid) {
    case ICS_SCMD_EEPROM:  size += ICS_VMEM_PAYLOAD_SIZE(ics_get_eeprom_size(option)); break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:    size +=  1; break;
    default:               size  =  0; break;
  }

  return size;
}

errno_t ics_ser_get_param_cmd (ics *ics, uint8_t id, uint8_t scmdid, size_t *serialized_size) {
  EVALUE(NULL, ics);
  EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_CMD(id, ICS_CMD_GP_READ);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(scmdid);

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_get_param_cmd (ics *ics, uint8_t id, uint8_t scmdid, uint8_t size, uint8_t rdata[], ics_opt_t option) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  //EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: size / max_size check

  size_t idx = 0;

#if 0
  /* request data */
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_CMD(id, ICS_CMD_GP_READ), EILSEQ);
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);
#endif

  /* reply data */
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REP_CMD(id, ICS_CMD_GP_READ), EILSEQ);
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  switch (scmdid) {
    case ICS_SCMD_EEPROM:
    {
      // TODO: range check
      for (size_t i = 0; i < ics_get_eeprom_size(option); i++) {
        ICS_VMEM_DESER_PAYLOAD(dps->buff, idx, rdata, i);
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

errno_t ics_write_get_param_req (ics *ics, uint8_t id) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_get_param_cmd(ics, id, ICS_SCMD_EEPROM, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_get_param_rep (ics *ics, uint8_t id, uint8_t max_size, uint8_t data[/*max_size*/], ics_opt_t option) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t expected_pkt_size = get_param_reppkt_size(ICS_SCMD_EEPROM, option);

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), true);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_get_param_cmd(ics, id, ICS_SCMD_EEPROM, max_size, data, option));

  return EOK;
}

errno_t _ics_get_param (ics *ics, uint8_t id, uint8_t max_size, uint8_t data[/*max_size*/], ics_opt_t option) {
  EVALUE(NULL, ics);

  ECALL(ics_write_get_param_req(ics, id));
  usleep(30 * 1000); /* at least 8[msec] */
  ECALL2(ics_read_get_param_rep(ics, id, max_size, data, option), false);

  return EOK;
}

errno_t ics_get_param (ics *ics, uint8_t id, uint8_t max_size, uint8_t data[/*max_size*/], ics_opt_t option) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_get_param(ics, id, max_size, data, option);
    if (eno == EOK) {
      break;
    }
    printf(" get_param --- id:%02d size:%d data[0]:%04x \n", id, max_size, data[0]);
  }

  return EOK;
}

static errno_t servo_mem_read (
        dpservo_base *dps, const uint8_t id,
        uint8_t start_addr, size_t rsize, uint8_t data[/*rsize*/], dps_opt_t option)
{
  EVALUE(NULL, dps);
  EVALUE(NULL, data);

  const size_t eeprom_size = ics_get_eeprom_size(option);
  EVALUE(0, eeprom_size);

  ELTGT(ICS_VMEM_ADDR_MIN, eeprom_size, start_addr);
  ELEGT(0, (eeprom_size - start_addr), rsize);
  uint8_t eeprom[eeprom_size];

  ECALL(ics_get_param((ics*)dps, id, eeprom_size, eeprom, option));

  memcpy(data, eeprom + start_addr, rsize);

#if defined(DEBUG)
  for (size_t i = 0; i < eeprom_size; i++) {
    if (i % 16 == 0) {
      printf("\n");
      printf(" 0x%02x :", (uint8_t)i);
    }
    printf(" %02x", eeprom[i]);
  }
  printf("\n");
#endif

  return EOK;
}

/*********************************************************************************************************************/

#define ICS_SET_PARAM_CMD_REP_PKT_HEADER_SIZE 2 /* [byte] */

static inline uint8_t set_param_reppkt_size (uint8_t scmdid, ics_opt_t option) {

  uint8_t size = ICS_SET_PARAM_CMD_REP_PKT_HEADER_SIZE;

  switch (scmdid) {
    case ICS_SCMD_EEPROM:  size +=  0; break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:    size +=  1; break;
    default:               size  =  0; break;
  }

  return size;
}

errno_t ics_ser_set_param_cmd (
        ics *ics, uint8_t id, uint8_t scmdid,
        size_t wsize, uint8_t wdata[/*size*/], size_t *serialized_size,
        ics_opt_t option)
{
  EVALUE(NULL, ics);
  EVALUE(NULL, wdata);
  EVALUE(NULL, serialized_size);

  const size_t payload_size = ics_get_eeprom_size(option);

  EXPECT_VALUE_ERRNO(payload_size, wsize, EINVAL);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_CMD(id, ICS_CMD_GP_WRITE);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(scmdid);

  switch (scmdid) {
    case ICS_SCMD_EEPROM:
    {
      for (size_t i = 0; i < payload_size; i++) {
        ICS_VMEM_SER_PAYLOAD(dps->buff, idx, wdata, i);
      }
    }
      break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:
      //EVALUE(wsize, idx + 1);
      //EVALUE(max_size, 1); TODO: range check
      dps->buff[idx++] = wdata[0];
      break;
    default:
      break;
  }

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_set_param_cmd (
        ics *ics, uint8_t id, uint8_t scmdid,
        uint8_t size, uint8_t rdata[], ics_opt_t option)
{
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  //EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: size / max_size check

  size_t idx = 0;

#if 0
  /* request data */
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_CMD(id, ICS_CMD_GP_READ), EILSEQ);
  EVALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);
#endif

  /* reply data */
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REP_CMD(id, ICS_CMD_GP_WRITE), EILSEQ);
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REQ_SUBCMD(scmdid), EILSEQ);

  switch (scmdid) {
    case ICS_SCMD_EEPROM:
      /* payload is 0 [byte] */
      break;
    case ICS_SCMD_STRETCH:
    case ICS_SCMD_SPEED:
    case ICS_SCMD_CURRENT:
    case ICS_SCMD_TEMP:
      EXPECT_VALUE_ERRNO(dps->buff[idx++], rdata[0], EILSEQ);
      break;
    default:
      break;
  }

  //*serialized_size = idx;

  return EOK;
}

errno_t ics_write_set_param_req (ics *ics, uint8_t id, uint8_t wsize, uint8_t wdata[/*wsize*/], ics_opt_t option) {
  EVALUE(NULL, ics);
  EVALUE(NULL, wdata);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_set_param_cmd(ics, id, ICS_SCMD_EEPROM, wsize, wdata, &size, option));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_set_param_rep (ics *ics, uint8_t id, uint8_t wsize, uint8_t wdata[/*max_size*/], ics_opt_t option) {
  EVALUE(NULL, ics);
  EVALUE(NULL, wdata);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t expected_pkt_size = set_param_reppkt_size(ICS_SCMD_EEPROM, option);

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), true);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_set_param_cmd(ics, id, ICS_SCMD_EEPROM, wsize, wdata, option));

  return EOK;
}

errno_t _ics_set_param (ics *ics, uint8_t id, uint8_t wsize, uint8_t wdata[/*wsize*/], ics_opt_t option) {
  EVALUE(NULL, ics);

  ECALL(ics_write_set_param_req(ics, id, wsize, wdata, option));
  usleep(1000 * 1000);
  ECALL2(ics_read_set_param_rep(ics, id, wsize, wdata, option), false);

  return EOK;
}

errno_t ics_set_param (ics *ics, uint8_t id, uint8_t wsize, uint8_t wdata[/*wsize*/], ics_opt_t option) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_set_param(ics, id, wsize, wdata, option);
    if (eno == EOK) {
      break;
    }
    printf(" set_param --- id:%02d size:%d data[0]:%04x \n", id, wsize, wdata[0]);
  }

  return EOK;
}

static errno_t servo_mem_write (
        dpservo_base *dps, const uint8_t id,
        uint8_t start_addr, size_t wsize, uint8_t data[/*rsize*/], dps_opt_t option)
{
  EVALUE(NULL, dps);
  EVALUE(NULL, data);

  const size_t eeprom_size = ics_get_eeprom_size(option);
  EVALUE(0, eeprom_size);

  ELTGT(ICS_VMEM_ADDR_MIN, eeprom_size, start_addr);
  ELEGT(0, (eeprom_size - start_addr), wsize);
  uint8_t eeprom[eeprom_size];

  ECALL(ics_get_param((ics*)dps, id, eeprom_size, eeprom, option));

  const uint8_t *write_mask = NULL;
  ECALL(ics_get_eeprom_write_mask(option, &write_mask));

  bool is_read_enable = true;

  for (size_t i = 0; i < wsize; i++) {
    size_t addr = start_addr + i;
    const uint8_t new = data[i] & write_mask[addr];

    /* when we change baudrate, then we can shall not receive reply pkt. */
    if (addr == ICS_ROM_BAUDRATE && eeprom[addr] != new) {
      is_read_enable = false;
    }

    if (write_mask[addr]) {
      eeprom[addr] = new;
    }
  }

#if defined(DEBUG)
  for (size_t i = 0; i < eeprom_size; i++) {
    if (i % 16 == 0) {
      printf("\n");
      printf(" 0x%02x :", (uint8_t)i);
    }
    printf(" %02x(%02x)", eeprom[i], write_mask[i]);
  }
  printf("\n");
#endif

  if (is_read_enable) {
    ECALL(ics_set_param((ics*)dps, id, eeprom_size, eeprom, option));
  } else {
    fprintf(stdout, " %s: you change baudrate, you must restart program with new baudrate.\n", __FUNCTION__);
  }

  return EOK;
}

/*********************************************************************************************************************/
/* servo specific. id read/write */

#define ICS_SET_ID_CMD_REP_PKT_SIZE 1 /* [byte] */

errno_t ics_ser_set_id_cmd (
        ics *ics, uint8_t id, size_t *serialized_size)
{
  EVALUE(NULL, ics);
  EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  /* | _CMD | _SC_ | _SC_ | _SC_ | */

  size_t idx = 0;
  dps->buff[idx++] = ICS_SER_REQ_CMD(id, ICS_CMD_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_SET_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_SET_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_SET_ID);

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_set_id_cmd (
        ics *ics, uint8_t id)
{
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: size / max_size check

  size_t idx = 0;

  /* | _CMD | */

  /* reply data */
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REP_CMD(id, ICS_CMD_ID), EILSEQ);

  //*serialized_size = idx;

  return EOK;
}

errno_t ics_write_set_id_req (ics *ics, uint8_t id) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_set_id_cmd(ics, id, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_set_id_rep (ics *ics, uint8_t id) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  const size_t expected_pkt_size = ICS_SET_ID_CMD_REP_PKT_SIZE;

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), true);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_set_id_cmd(ics, id));

  return EOK;
}

errno_t _ics_set_id (ics *ics, uint8_t id) {
  EVALUE(NULL, ics);

  ECALL(ics_write_set_id_req(ics, id));
  usleep(100 * 1000);
  ECALL2(ics_read_set_id_rep(ics, id), false);

  return EOK;
}

errno_t ics_set_id (ics *ics, uint8_t id) {
  EVALUE(NULL, ics);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_set_id(ics, id);
    if (eno == EOK) {
      break;
    }
    printf(" set_id --- id:%02d \n", id);
  }

  return EOK;
}

/*********************************************************************************************************************/

#define ICS_GET_ID_CMD_REP_PKT_SIZE 1 /* [byte] */

errno_t ics_ser_get_id_cmd (
        ics *ics, size_t *serialized_size)
{
  EVALUE(NULL, ics);
  EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: max_size check

  /* | _CMD | _SC_ | _SC_ | _SC_ | */

  size_t idx = 0;
  /* servo id should be ignore with ICS_SCMD_GET_ID */
  dps->buff[idx++] = ICS_SER_REQ_CMD(ICS_SERVO_ID_INV, ICS_CMD_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_GET_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_GET_ID);
  dps->buff[idx++] = ICS_SER_REQ_SUBCMD(ICS_SCMD_GET_ID);

  *serialized_size = idx;

  return EOK;
}

errno_t ics_deser_get_id_cmd (
        ics *ics, uint8_t *id)
{
  EVALUE(NULL, ics);
  EVALUE(NULL, id);

  dpservo_base *dps = get_dpservo_base(ics);

  //TODO: size / max_size check

  size_t idx = 0;

  /* | _CMD | */

  /* reply data */
  *id = dps->buff[idx++] & ICS_CMDID_MASK;
  EXPECT_VALUE_ERRNO(dps->buff[idx++], ICS_SER_REP_CMD(*id, ICS_CMD_ID), EILSEQ);

  //*serialized_size = idx;

  return EOK;
}

errno_t ics_write_get_id_req (ics *ics) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);

  size_t size;

  ECALL(ics_ser_get_id_cmd(ics, &size));
  ECALL(data_dump(dps->buff, size));
  ECALL(hr_serial_write(dps->hrs, dps->buff, size));

  return EOK;
}

errno_t ics_read_get_id_rep (ics *ics, uint8_t *id) {
  EVALUE(NULL, ics);
  EVALUE(NULL, id);

  dpservo_base *dps = get_dpservo_base(ics);

  const size_t expected_pkt_size = ICS_GET_ID_CMD_REP_PKT_SIZE;

  ECALL2(hr_serial_read(dps->hrs, dps->buff, expected_pkt_size), true);
  ECALL(data_dump(dps->buff, expected_pkt_size));
  ECALL(ics_deser_get_id_cmd(ics, id));

  return EOK;
}

errno_t _ics_get_id (ics *ics, uint8_t *id) {
  EVALUE(NULL, ics);

  ECALL(ics_write_get_id_req(ics));
  usleep(100 * 1000);
  ECALL2(ics_read_get_id_rep(ics, id), false);

  return EOK;
}

errno_t ics_get_id (ics *ics, uint8_t *id) {
  EVALUE(NULL, ics);
  EVALUE(NULL, id);

  const size_t max_count = ics->retry_count;
  for (size_t i = 0; i < max_count; i++) {
    errno_t eno = _ics_get_id(ics, id);
    if (eno == EOK) {
      break;
    }
    printf(" get_id --- id:%02x \n", *id);
  }

  return EOK;
}

/*********************************************************************************************************************/

static errno_t set_state (dpservo_base *dps, uint8_t id, dps_servo_state state) {
  EVALUE(NULL, dps);
  switch (state) {
    case kDpsServoOff:
      ECALL(ics_write_read_pos((ics*)dps, id, ICS_CMD_REQ_POS, ICS_POS_FREE, NULL));
      break;
    case kDpsServoBrk:
    case kDpsServoOn:
    {
      uint16_t curpos;
      ECALL(ics_write_read_pos((ics*)dps, id, ICS_CMD_REQ_POS, ICS_POS_FREE, &curpos));
      ECALL(ics_write_read_pos((ics*)dps, id, ICS_CMD_REQ_POS, curpos, &curpos));
    }
      break;
    default:
      return EINVAL;
      break;
  }
  return EOK;
}

static errno_t set_states (dpservo_base *dps, dps_servo_state state) {
  EVALUE(NULL, dps);
  for (size_t i = 0; i < dps->num_of_servo; i++) {
    //ECALL(set_state(dps, dps->servo_ids[i], state)); // TODO:
    (set_state(dps, dps->servo_ids[i], state));
  }
  return EOK;
}

static errno_t set_goal (dpservo_base *dps, uint8_t id, float64_t goal) {
  EVALUE(NULL, dps);
  ELTGT(ICS_POS_MIN, ICS_POS_MAX, goal);

  uint16_t ogoal = ICS_POS2HEX(goal);
  ECALL(ics_write_read_pos((ics*)dps, id, ICS_CMD_REQ_POS, ogoal, NULL));

  //uint16_t curpos;
  //for (size_t i = 0; i < 100; i++) {
  //  ECALL(ics_write_read_pos((ics*)dps, id, ICS_CMD_REQ_POS, 0x0000, &curpos));
  //  printf("===>>> %d %04x %lf\n", ogoal, curpos, ICS_HEX2POS(curpos));
  //  usleep(100 * 1000);
  //}

  return EOK;
}

static errno_t set_goals (dpservo_base *dps, size_t num, float64_t goal[/*num*/]) {
  EVALUE(NULL, dps);

#if defined(HR_SERIAL_LATENCY_CHECK)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

  for (size_t i = 0; i < dps->num_of_servo; i++) {
    uint16_t iogoal = ICS_POS2HEX(goal[i]);
    //ECALL(ics_write_read_pos((ics*)dps, dps->servo_ids[i], ICS_CMD_REQ_POS, iogoal, &iogoal));
    (ics_write_read_pos((ics*)dps, dps->servo_ids[i], ICS_CMD_REQ_POS, iogoal, &iogoal));
    //usleep(1000 * 1000);
    //goal[i] = ICS_HEX2POS(iogoal);
  }

#if defined(HR_SERIAL_LATENCY_CHECK)
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf("     set goals latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif

  return EOK;
}


static errno_t NOT_USED_FUNC mem_write (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/], dps_opt_t option) {
  EVALUE(NULL, dps);
  ECALL(ics_mem_write((ics*)dps, id, start_addr, size, data));
  return EOK;
}

static errno_t NOT_USED_FUNC mem_read (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/], dps_opt_t option) {
  EVALUE(NULL, dps);
  ECALL(ics_mem_read((ics*)dps, id, start_addr, size, data));
  return EOK;
}

errno_t ics_init (ics *ics) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);
  ECALL(dpservo_ops_init(&(dps->ops), set_state, set_states, set_goal, set_goals, servo_mem_write, servo_mem_read));

  ics->retry_count = 3;

  return EOK;
}

/* to be obsoluted */
errno_t ics_open (ics *ics, const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity){
  EVALUE(NULL, ics);
  if (ics->base.io_enabled) ECALL(hr_serial_open(ics->base.hrs, device, port, baudrate, parity));
  return EOK;
}

errno_t ics_close (ics *ics) {
  EVALUE(NULL, ics);
  if (ics->base.io_enabled) ECALL(hr_serial_close(ics->base.hrs));
  return EOK;
}

errno_t ics_set_serial (ics *ics, bool io_enabled) {
  EVALUE(NULL, ics);

  ics->base.io_enabled = io_enabled;

  return EOK;
}



