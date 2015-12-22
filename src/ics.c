
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

#define ICS_POS_MAX       (135.0) /* [deg] */
#define ICS_POS_MIN      (-135.0) /* [deg] */
                                /* ICS spec   |  DP spec   | decimal       */
#define ICS_POS_MAX_HEX  0x2CEC /* 270[deg]  --> +135[deg] | 11500(0x2cec) */ /* 0x3FFF? */
#define ICS_POS_MID_HEX  0x1D4C /* 135[deg]  -->    0[deg] |  7500(0x1d4c) */ /* 0x2000? */
#define ICS_POS_MIN_HEX  0x0DAC /*   0[deg]  --> -135[deg] |  3500(0x0dac) */ /* 0x0001? */
#define ICS_POS_FREE     0x0000 /* servo free              */

#define ICS_POS2HEX(pos) ((pos) / ICS_POS_MAX * (ICS_POS_MAX_HEX - ICS_POS_MID_HEX) + ICS_POS_MID_HEX)
#define ICS_HEX2POS(hex) (((hex) - ICS_POS_MID_HEX) * ICS_POS_MAX / (ICS_POS_MAX_HEX - ICS_POS_MID_HEX))

errno_t ics_ser_set_pos_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint16_t pos, size_t *serialized_size) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  /* base class member */
  dpservo_base *dps = get_dpservo_base(ics);

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
  if (dps->buff[idx++] != ICS_SER_REP_CMD(id, cmdid, baudrate)) {
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
    printf("--- id:%02d pos:%04x fpos:%lf %zd/%zd --failed\n", id, refpos, ICS_HEX2POS(refpos), i, max_count);
  }

  return EOK;
}

errno_t ics_ser_get_param_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint8_t scmdid, size_t *serialized_size) {
  EVALUE(NULL, ics);
  //EVALUE(NULL, data);
  EVALUE(NULL, serialized_size);

  dpservo_base *dps = get_dpservo_base(ics);

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

  dpservo_base *dps = get_dpservo_base(ics);

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

#if defined(HR_SERIAL_LATENCY_CHECK)
#include "time/hr_unixtime.h"
#endif
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

static errno_t write_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  fprintf(stderr, "%s:%s:%d not implemented yet\n", __FILE__, __FUNCTION__, __LINE__);
  //ECALL(rsx_spkt_mem_write((rsx*)dps, id, start_addr, size, data));
  return EOK;
}

static errno_t read_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/]) {
  EVALUE(NULL, dps);
  fprintf(stderr, "%s:%s:%d not implemented yet\n", __FILE__, __FUNCTION__, __LINE__);
  //ECALL(rsx_spkt_mem_read((rsx*)dps, id, start_addr, size, data));
  return EOK;
}

errno_t ics_init (ics *ics) {
  EVALUE(NULL, ics);

  dpservo_base *dps = get_dpservo_base(ics);
  ECALL(dpservo_ops_init(&(dps->ops), set_state, set_states, set_goal, set_goals, write_mem, read_mem));

  ics->retry_count = 3;

  return EOK;
}

