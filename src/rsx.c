
#include "rsx/rsx.h"

#include "stdio.h"

#include "serial/hr_serial.h"

errno_t rsx_open (rsx *rsx, const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity){
  EVALUE(NULL, rsx);
  if (rsx->base.io_enabled) ECALL(hr_serial_open(rsx->base.hrs, device, port, baudrate, parity));
  return EOK;
}

errno_t rsx_close (rsx *rsx) {
  EVALUE(NULL, rsx);
  if (rsx->base.io_enabled) ECALL(hr_serial_close(rsx->base.hrs));
  return EOK;
}

errno_t rsx_set_serial (rsx *rsx, bool io_enabled) {
  EVALUE(NULL, rsx);

  rsx->base.io_enabled = io_enabled;

  return EOK;
}

errno_t rsx_spkt_write (rsx *rsx) {
  EVALUE(NULL, rsx);

  errno_t eno = EOK;

  size_t pkt_size;
  ECALL(rsx_pkt_ser(rsx->spkt, rsx->base.buff, rsx->base.max_size, &pkt_size));

  ECALL(hr_serial_write(rsx->base.hrs, rsx->base.buff, pkt_size));

  return eno;
}

errno_t rsx_spkt_write_read (rsx *rsx) {
  EVALUE(NULL, rsx);

  errno_t eno = EOK;

  size_t pkt_size;
  ECALL(rsx_pkt_ser(rsx->spkt, rsx->base.buff, rsx->base.max_size, &pkt_size));

  if (rsx->base.io_enabled) {
    for (size_t cnt = 0; cnt < rsx->retry_count; cnt++) {
      ECALL(hr_serial_write(rsx->base.hrs, rsx->base.buff, pkt_size));
      ECALL(data_dump(rsx->base.buff, pkt_size));

      //usleep(10 * 1000);
      const size_t recv_payload_size = RSX_SPKT_GETLENGTH(*(rsx->spkt));
      errno_t eno = hr_serial_read(rsx->base.hrs, rsx->base.buff, pkt_size + recv_payload_size);
      //printf("------> %d %d+%d\n", size, pkt_size, recv_payload_size);
      ECALL(data_dump(rsx->base.buff, pkt_size + recv_payload_size));

      //uint8_t size;
      //ECALL(rsx_pkt_get_rtn_size(rsx->spkt, &size));
      //errno_t eno = hr_serial_read(rsx->base.hrs, rsx->base.buff, size);
      if (eno == EOK) {
        ECALL(rsx_pkt_deser(rsx->spkt, rsx->base.buff, rsx->base.max_size, &pkt_size));
        break;
      } else {
        if (cnt == rsx->retry_count - 1) {
          eno = -1;
        }
        continue;
      }
    }
  }

  return eno;
}

errno_t rsx_lpkt_write (rsx *rsx) {
  EVALUE(NULL, rsx);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(rsx->lpkt, rsx->base.buff, rsx->base.max_size, &pkt_size));

  ECALL(data_dump(rsx->base.buff, pkt_size));

  if(rsx->base.io_enabled) ECALL(hr_serial_write(rsx->base.hrs, rsx->base.buff, pkt_size));

  return EOK;
}

errno_t rsx_spkt_mem_write (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  ECALL(rsx_pkt_reset(rsx->spkt)); /* TODO: */

  RSX_SPKT_SETID(*(rsx->spkt), id);
  RSX_SPKT_SETFLAG(*(rsx->spkt), RSX_SPKT_SND_FLG_DEFAULT & RSX_SPKT_RTN_FLG_RTNLESS);
  RSX_SPKT_SETADDR(*(rsx->spkt), start_addr);
  RSX_SPKT_SETLENGTH(*(rsx->spkt), size);

  for (size_t i = 0; i < size; i++) {
    RSX_SPKT_GET_U8(*(rsx->spkt), i) = data[i];
  }

  ECALL(rsx_spkt_write(rsx));

  return EOK;
}

errno_t rsx_spkt_mem_write_int16 (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  ECALL(rsx_pkt_reset(rsx->spkt)); /* TODO: */

  RSX_SPKT_SETID(*(rsx->spkt), id);
  RSX_SPKT_SETFLAG(*(rsx->spkt), RSX_SPKT_SND_FLG_DEFAULT & RSX_SPKT_RTN_FLG_RTNLESS);
  RSX_SPKT_SETADDR(*(rsx->spkt), start_addr);
  RSX_SPKT_SETLENGTH(*(rsx->spkt), size * sizeof(int16_t));

  for (size_t i = 0; i < size; i += sizeof(int16_t)) {
    RSX_SPKT_SET_INT16(*(rsx->lpkt), i, data[i]);
  }

  ECALL(rsx_spkt_write(rsx));

  return EOK;
}

errno_t rsx_spkt_mem_read (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  ECALL(rsx_pkt_reset(rsx->spkt));

  RSX_SPKT_SETID(*(rsx->spkt), id);
  RSX_SPKT_SETFLAG(*(rsx->spkt), RSX_SPKT_SND_FLG_DEFAULT | RSX_SPKT_RTN_FLG_ADDR);
  RSX_SPKT_SETADDR(*(rsx->spkt), start_addr);
  RSX_SPKT_SETLENGTH(*(rsx->spkt), size);

  ECALL(rsx_spkt_write_read(rsx));

  for (size_t i = 0; i < size; i++) {
    data[i] = RSX_SPKT_GET_U8(*(rsx->spkt), i);
  }

  return EOK;
}

errno_t rsx_spkt_mem_read_int16 (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  ECALL(rsx_pkt_reset(rsx->spkt));

  RSX_SPKT_SETID(*(rsx->spkt), id);
  RSX_SPKT_SETFLAG(*(rsx->spkt), RSX_SPKT_SND_FLG_DEFAULT | RSX_SPKT_RTN_FLG_ADDR);
  RSX_SPKT_SETADDR(*(rsx->spkt), start_addr);
  RSX_SPKT_SETLENGTH(*(rsx->spkt), size * sizeof(int16_t));

  ECALL(rsx_spkt_write_read(rsx));

  for (size_t i = 0; i < size; i += sizeof(int16_t)) {
    data[i] = RSX_SPKT_GET_INT16(*(rsx->spkt), i);
  }

  return EOK;
}

static errno_t _rsx_lpkt_mem_write (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, uint8_t data[/*num*/][size]) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, id);
  EVALUE(NULL, data);

  RSX_LPKT_SETADDR(*(rsx->lpkt), start_addr);
  RSX_LPKT_SETCOUNT(*(rsx->lpkt), num);
  RSX_LPKT_SETLENGTH(*(rsx->lpkt), size);
  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SETVID(*(rsx->lpkt), i, id[i]);
    for (size_t j = 0; j < size; j++) {
      RSX_LPKT_SET_U8(*(rsx->lpkt), i, j, data[i][j]);
    }
  }
  ECALL(rsx_lpkt_write(rsx));

  return 0;
}

errno_t rsx_lpkt_mem_write (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, const uint8_t size, uint8_t **wdata) {
  uint8_t (*data) [size] = (uint8_t (*)[size])wdata;
  ECALL(_rsx_lpkt_mem_write(rsx, id, num, start_addr, size, data));
  return 0;
}

static errno_t _rsx_lpkt_mem_write_int16 (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t data[/*num*/][size]) {
  EVALUE(NULL, rsx);

  RSX_LPKT_SETADDR(*(rsx->lpkt), start_addr);
  RSX_LPKT_SETCOUNT(*(rsx->lpkt), num);
  RSX_LPKT_SETLENGTH(*(rsx->lpkt), size * sizeof(int16_t));
  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SETVID(*(rsx->lpkt), i, id[i]);
    for (size_t j = 0; j < size; j += sizeof(int16_t)) {
      RSX_LPKT_SET_INT16(*(rsx->lpkt), i, j, data[i][j]);
    }
  }
  ECALL(rsx_lpkt_write(rsx));

  return 0;
}

errno_t rsx_lpkt_mem_write_int16 (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t **wdata) {
  int16_t (*data) [size] = (int16_t (*)[size])wdata;
  ECALL(_rsx_lpkt_mem_write_int16(rsx, id, num, start_addr, size, data));
  return 0;
}

errno_t rsx_lpkt_mem_write_all (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  RSX_LPKT_SETADDR(*(rsx->lpkt), start_addr);
  RSX_LPKT_SETCOUNT(*(rsx->lpkt), num);
  RSX_LPKT_SETLENGTH(*(rsx->lpkt), size);
  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SETVID(*(rsx->lpkt), i, id[i]);
    for (size_t j = 0; j < size; j++) {
      RSX_LPKT_SET_U8(*(rsx->lpkt), i, j, data[j]);
    }
  }
  ECALL(rsx_lpkt_write(rsx));

  return 0;
}

errno_t rsx_lpkt_mem_write_int16_all (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]) {
  EVALUE(NULL, rsx);

  RSX_LPKT_SETADDR(*(rsx->lpkt), start_addr);
  RSX_LPKT_SETCOUNT(*(rsx->lpkt), num);
  RSX_LPKT_SETLENGTH(*(rsx->lpkt), size * sizeof(int16_t));
  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SETVID(*(rsx->lpkt), i, id[i]);
    for (size_t j = 0; j < size; j += sizeof(int16_t)) {
      RSX_LPKT_SET_INT16(*(rsx->lpkt), i, j, data[j]);
    }
  }
  ECALL(rsx_lpkt_write(rsx));

  return 0;
}

static inline errno_t servo_state_dps2rsx (dps_servo_state istate, uint8_t *ostate) {
  EVALUE(NULL, ostate);
  switch (istate) {
    case kDpsServoOff : *ostate = RSX_DATA_SERVO_OFF; break;
    case kDpsServoBrk : *ostate = RSX_DATA_SERVO_BRK; break;
    case kDpsServoOn  : *ostate = RSX_DATA_SERVO_ON;  break;
    default:
      return EINVAL;
  }
  return EOK;
}

// TODO:
#include "rsx/mmap/rs30x.h"

errno_t set_state (dpservo_base *dps, uint8_t id, dps_servo_state state) {
  EVALUE(NULL, dps);
  uint8_t servo_state;
  ECALL(servo_state_dps2rsx(state, &servo_state));
  ECALL(rsx_spkt_mem_write((rsx*)dps, id, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));
  return EOK;
}

errno_t set_states (dpservo_base *dps, dps_servo_state state) {
  EVALUE(NULL, dps);
  uint8_t servo_state;
  ECALL(servo_state_dps2rsx(state, &servo_state));
  ECALL(rsx_lpkt_mem_write_all((rsx*)dps, dps->servo_ids, dps->num_of_servo, RSX_RAM_TRQ_ENABLE, 1, ((uint8_t[]){servo_state})));
  return EOK;
}

errno_t set_goal (dpservo_base *dps, uint8_t id, float64_t goal) {
  EVALUE(NULL, dps);
  int16_t ogoal = (int16_t)(goal * 10);
  ECALL(rsx_spkt_mem_write_int16((rsx*)dps, id, RSX_RAM_GOAL_POS_L, 1, ((int16_t[]){ogoal})));
  return EOK;
}

errno_t set_goals (dpservo_base *dps, size_t num, float64_t goal[/*num*/]) {
  EVALUE(NULL, dps);
  if (num != dps->num_of_servo) {
    return EINVAL;
  }
  int16_t ogoal[dps->num_of_servo][1];
  for (size_t i = 0; i < num; i++) {
    ogoal[i][0] = (int16_t)(goal[i] * 10);
  }
  ECALL(_rsx_lpkt_mem_write_int16((rsx*)dps, dps->servo_ids, dps->num_of_servo, RSX_RAM_GOAL_POS_L, 1, ogoal));
  return EOK;
}

static errno_t write_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/], dps_opt_t option) {
  EVALUE(NULL, dps);
  ECALL(rsx_spkt_mem_write((rsx*)dps, id, start_addr, size, data));
  return EOK;
}

static errno_t read_mem (dpservo_base *dps, const uint8_t id, uint8_t start_addr, size_t size/*[byte]*/, uint8_t data[/*size*/], dps_opt_t option) {
  EVALUE(NULL, dps);
  ECALL(rsx_spkt_mem_read((rsx*)dps, id, start_addr, size, data));
  return EOK;
}

errno_t rsx_init (rsx *rsx, rsx_pkt *lpkt, rsx_pkt *spkt) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, lpkt);
  EVALUE(NULL, spkt);

  dpservo_base *dps = get_dpservo_base(rsx);
  ECALL(dpservo_ops_init(&(dps->ops), set_state, set_states, set_goal, set_goals, write_mem, read_mem));

  rsx->retry_count = 5;

  rsx->lpkt = lpkt;
  rsx->spkt = spkt;

  return EOK;
}

