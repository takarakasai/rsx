
#include "rsx.h"

#include "stdio.h"

#include "serial/hr_serial.h"

errno_t rsx_init (rsx *rsx, rsx_pkt *lpkt, rsx_pkt *spkt, hr_serial *hrs, uint8_t buff[/*max_size*/], size_t max_size) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, lpkt);
  EVALUE(NULL, spkt);
  EVALUE(NULL, hrs);
  EVALUE(NULL, buff);

  ECALL(hr_serial_init(hrs));

  rsx->lpkt = lpkt;
  rsx->spkt = spkt;
  rsx->hrs = hrs;
  rsx->buff = buff;
  rsx->max_size = max_size;

  rsx->retry_count = 5;

  rsx->use_serial = true;

  return EOK;
}

errno_t rsx_open (rsx *x, const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity){
  EVALUE(NULL, x);
  if (x->use_serial) ECALL(hr_serial_open(x->hrs, device, port, baudrate, parity));
  return EOK;
}

errno_t rsx_close (rsx *x) {
  EVALUE(NULL, x);
  if (x->use_serial) ECALL(hr_serial_close(x->hrs));
  return EOK;
}

errno_t rsx_spkt_write (rsx *x) {
  EVALUE(NULL, x);

  errno_t eno = EOK;

  size_t pkt_size;
  ECALL(rsx_pkt_ser(x->spkt, x->buff, x->max_size, &pkt_size));

  ECALL(hr_serial_write(x->hrs, x->buff, pkt_size));

  return eno;
}

errno_t rsx_spkt_write_read (rsx *x) {
  EVALUE(NULL, x);

  errno_t eno = EOK;

  size_t pkt_size;
  ECALL(rsx_pkt_ser(x->spkt, x->buff, x->max_size, &pkt_size));

  if (x->use_serial) {
    for (size_t cnt = 0; cnt < x->retry_count; cnt++) {
      ECALL(hr_serial_write(x->hrs, x->buff, pkt_size));
      ECALL(data_dump(x->buff, pkt_size));

      //usleep(10 * 1000);
      const size_t recv_payload_size = RSX_SPKT_GETLENGTH(*(x->spkt));
      errno_t eno = hr_serial_read(x->hrs, x->buff, pkt_size + recv_payload_size);
      //printf("------> %d %d+%d\n", size, pkt_size, recv_payload_size);
      ECALL(data_dump(x->buff, pkt_size + recv_payload_size));

      //uint8_t size;
      //ECALL(rsx_pkt_get_rtn_size(x->spkt, &size));
      //errno_t eno = hr_serial_read(x->hrs, x->buff, size);
      if (eno == EOK) {
        ECALL(rsx_pkt_deser(x->spkt, x->buff, x->max_size, &pkt_size));
        break;
      } else {
        if (cnt == x->retry_count - 1) {
          eno = -1;
        }
        continue;
      }
    }
  }

  return eno;
}

errno_t rsx_lpkt_write (rsx *x) {
  EVALUE(NULL, x);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(x->lpkt, x->buff, x->max_size, &pkt_size));

  ECALL(data_dump(x->buff, pkt_size));

  if(x->use_serial) ECALL(hr_serial_write(x->hrs, x->buff, pkt_size));

  return EOK;
}

errno_t rsx_set_serial (rsx *x, bool use_serial) {
  EVALUE(NULL, x);

  x->use_serial = use_serial;

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

