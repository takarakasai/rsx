#include "rsx/rsx.h"

#include <stdlib.h>

/* for wait_usec */
#include "rsx/util/time.h"

static const int kAvailableBaudrates[RSX_NUM_OF_BAUDRATES] = {
  9600,
  14400,
  19200,
  28800,
  38400,
  57600,
  115200,
  153600,
  230400,
  // 460800
};
 
static errno_t int_to_baudrate(int from, uint8_t* to) {
  EVALUE(NULL, to);

  switch (from) {
    case   9600: *to = 0x00; break;
#if defined(B28800)
    case  14400: *to = 0x01; break;
#endif
    case  19200: *to = 0x02; break;
#if defined(B28800)
    case  28800: *to = 0x03; break;
#endif
    case  38400: *to = 0x04; break;
    case  57600: *to = 0x05; break;
#if defined(B76800)
    case  76800: *to = 0x06; break;
#endif
    case 115200: *to = 0x07; break;
#if defined(B28800)
    case 153600: *to = 0x08; break;
#endif
    case 230400: *to = 0x09; break;
    default:
      return EINVAL;
  }

  return EOK;
}

static errno_t baudrate_to_int(uint8_t from, int* to) {
  EVALUE(NULL, to);

  switch (from) {
    case 0x00: *to = 9600; break;
#if defined(B28800)
    case 0x01: *to = 14400; break;
#endif
    case 0x02: *to = 19200; break;
#if defined(B28800)
    case 0x03: *to = 28800; break;
#endif
    case 0x04: *to = 38400; break;
    case 0x05: *to = 57600; break;
#if defined(B28800)
    case 0x06: *to = 76800; break;
#endif
    case 0x07: *to = 115200; break;
    case 0x08: *to = 153600; break;
    case 0x09: *to = 230400; break;
    default:
      *to = -1;
      return EINVAL;
  }

  return EOK;
}

errno_t rsx_config_init(  //
    rsx_config* config) {
  EVALUE(NULL, config);

  config->num_of_servos    = 8;
  config->max_payload_size = 32;

  return EOK;
}

errno_t rsx_config_copy(
    rsx_config* from,
    rsx_config* to) {
  EVALUE(NULL, from);
  EVALUE(NULL, to);

  to->num_of_servos = from->num_of_servos;
  to->max_payload_size = from->max_payload_size;

  return EOK;
}

errno_t rsx_init(  //
    rsx* rsx,      //
    rsx_config* config) {
  EVALUE(NULL, rsx);

  rsx->write_size = 0;
  rsx->read_size = 0;
  rsx->retry_count = 0;

  rsx->wbuff = NULL;
  rsx->rbuff = NULL;
  rsx->payload = NULL;
  rsx->pkt.data = NULL;

  if (config) {
    ECALL(rsx_config_copy(config, &rsx->config));
  } else {
    ECALL(rsx_config_init(&rsx->config));
  }

  // <------------ max_frame_size ----------------------->
  // | HEADER | ID | FLG | ADR | LEN | CNT | data | SUM |
  //                                           |
  //   +---------------------------------------+
  //   |
  //   v     <- lpkt_max_1payload_size->
  // data := | ID1 | DATA1(LEN[Bytes]) | ID2 | DATA2(LEN) | ... | IDx | DATAx(LEN)|
  //         <----------------------- x = [0..CNT] ------------------------------->
  ssize_t lpkt_max_1payload_size = RSX_PKT_SIZE_ID + rsx->config.max_payload_size;
  rsx->max_payload_size = lpkt_max_1payload_size * rsx->config.num_of_servos;
  rsx->max_frame_size   = rsx->max_payload_size + RSX_PKT_SIZE_MIN;

  rsx->wbuff = malloc(rsx->max_frame_size);
  if (rsx->wbuff == NULL) {
    errno_t eno = errno;
    // ENOMEM
    ECALL(rsx_destroy(rsx));
    return eno;
  }

  rsx->rbuff = malloc(rsx->max_frame_size);
  if (rsx->rbuff == NULL) {
    errno_t eno = errno;
    // ENOMEM
    ECALL(rsx_destroy(rsx));
    return eno;
  }

  rsx->payload = malloc(rsx->max_payload_size);
  if (rsx->payload == NULL) {
    errno_t eno = errno;
    // ENOMEM
    ECALL(rsx_destroy(rsx));
    return eno;
  }

  // FIXME(takara.kasai@gmail.com) : remove 1st/2nd args
  ECALL(rsx_pkt_init(&(rsx->pkt), 0, 0, rsx->payload));

  return EOK;
}

errno_t rsx_destroy(  //
    rsx* rsx) {
  EVALUE(NULL, rsx);

  if (rsx->wbuff != NULL) {
    free(rsx->wbuff);
  }

  if (rsx->rbuff != NULL) {
    free(rsx->rbuff);
  }

  if (rsx->payload != NULL) {
    free(rsx->payload);
  }

  rsx->wbuff = NULL;
  rsx->rbuff = NULL;
  rsx->payload = NULL;
  rsx->pkt.data = NULL;

  return EOK;
}

errno_t rsx_bulk_write_impl (rsx* rsx, hr_serial* hrs) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));
  ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));

  // rsx_wait_usec(1000);

  return EOK;
}

errno_t rsx_oneshot_read_impl (
    rsx* rsx, hr_serial* hrs) {
  EVALUE(NULL, rsx);

  const size_t kTimeout = 0;
  errno_t eno  = -1;
  size_t count = 0;
  do {
    rsx_wait_usec(100);
    eno = hr_serial_read(hrs, rsx->rbuff, rsx->read_size);
    if (eno == EOK) {
      // ECALL(data_dump(rsx->rbuff, rsx->read_size));
      size_t pkt_size;
      eno = rsx_pkt_deser(&(rsx->pkt), rsx->rbuff, rsx->max_frame_size, &pkt_size);
      assert(rsx->read_size == pkt_size);
      if (eno == EOK) {
        break;
      }
    }
  } while(count++ < 200);
  rsx->retry_count = count;

  if (eno != EOK && count >= kTimeout) {
    return ETIMEDOUT;
  }

  return EOK;
}

errno_t rsx_oneshot_sync_read_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size) {
  EVALUE(NULL, rsx);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));

  const size_t kTimeout = 0;
  size_t count = 0;
  rsx->read_size = pkt_size + payload_size;
  errno_t eno = -1;
  do {
    // ECALL(data_dump(rsx->wbuff, pkt_size));
    ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));

    eno = rsx_oneshot_read_impl(rsx, hrs);
    if (eno == EOK) {
      break;
    }
  } while (count++ < kTimeout);

  if (eno != EOK && count >= kTimeout) {
    return ETIMEDOUT;
  }

  return EOK;
}

/* w/o data check */
errno_t rsx_oneshot_write_impl (
    rsx* rsx, hr_serial* hrs) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));
  rsx->write_size = pkt_size;
  // ECALL(data_dump(rsx->wbuff, pkt_size));
  ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));

  return EOK;
}

/* w/ data check by the oneshot read command */
errno_t rsx_oneshot_sync_write_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));
  // ECALL(data_dump(rsx->wbuff, pkt_size));
  ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));
  rsx_wait_usec(100 * 1000);

  ECALL(rsx_spkt_conv_read_cmd(&(rsx->pkt)));
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));
  // ECALL(data_dump(rsx->wbuff, pkt_size));

  size_t count = 0;
  rsx->read_size = pkt_size + payload_size;
  do {
    // ECALL(data_dump(rsx->wbuff, pkt_size));
    errno_t eno = 0;
    // ECALL(data_dump(rsx->wbuff, pkt_size));
    ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));
    rsx_wait_usec(100 * 1000);

    eno = hr_serial_read(hrs, rsx->rbuff, rsx->read_size);
    if (eno == EOK) {
      // ECALL(data_dump(rsx->rbuff, rsx->read_size));
      eno = rsx_pkt_deser_and_check(&(rsx->pkt), rsx->rbuff, rsx->max_frame_size);
      if (eno == EOK) {
        break;
      }
    }
  } while (count++ < 10);
  rsx->retry_count = count;

  if (count >= 10) {
    return ETIMEDOUT;
  }

  return EOK;
}

errno_t rsx_bulk_write_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t addr, uint8_t ids[/*num*/], uint8_t data[/*num*/], size_t num) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);
  EVALUE(NULL, ids);
  EVALUE(NULL, data);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.

  // ECALL(rsx_spkt_init(&(rsx->pkt), id, 0xF));
  // | HEADER | ID | FLG | ADR | LEN | CNT | data | SUM |
  // |   2    | 1  |  1  |  1  |  1  |  1  |  1   |  1  |
  //   FA AF    0     0    adr   (1)   cnt  id,data sum
  // (1) num of bytes per servo. (id + data)
  ECALL(rsx_lpkt_init(&(rsx->pkt)));
  RSX_LPKT_SETADDR(rsx->pkt, addr);
  RSX_LPKT_SETLENGTH(rsx->pkt, sizeof(uint8_t) + sizeof(uint8_t));
  RSX_LPKT_SETCOUNT(rsx->pkt, num);

  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SET_U8(rsx->pkt, i, 0, ids[i]);
    RSX_LPKT_SET_U8(rsx->pkt, i, 1, data[i]);
  }

  ECALL(rsx_bulk_write_impl(rsx, hrs));

  return EOK;
}

errno_t rsx_bulk_write_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t addr, uint8_t ids[/*num*/], uint16_t data[/*num*/], size_t num) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);
  EVALUE(NULL, ids);
  EVALUE(NULL, data);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.

  // ECALL(rsx_spkt_init(&(rsx->pkt), id, 0xF));
  // | HEADER | ID | FLG | ADR | LEN | CNT | data | SUM |
  // |   2    | 1  |  1  |  1  |  1  |  1  |  1   |  1  |
  //   FA AF    0     0    adr   (1)   cnt  id,data sum
  // (1) num of bytes per servo. (id + data)
  ECALL(rsx_lpkt_init(&(rsx->pkt)));
  RSX_LPKT_SETADDR(rsx->pkt, addr);
  RSX_LPKT_SETLENGTH(rsx->pkt, sizeof(uint16_t));
  RSX_LPKT_SETCOUNT(rsx->pkt, num);

  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SETID(rsx->pkt, i, ids[i]);
  }
  for (size_t i = 0; i < num; i++) {
    RSX_LPKT_SET_INT16(rsx->pkt, i, 0, data[i]);
  }

  ECALL(rsx_bulk_write_impl(rsx, hrs));

  return EOK;
}

errno_t rsx_oneshot_read_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t size) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0xF));
  RSX_SPKT_SETADDR(rsx->pkt, addr);
  RSX_SPKT_SETLENGTH(rsx->pkt, size /* Byte */);

  errno_t eno = rsx_oneshot_sync_read_impl(rsx, hrs, size /* Byte */);

  return eno;
}

errno_t rsx_oneshot_read_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t words) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0xF));
  RSX_SPKT_SETADDR(rsx->pkt, addr);
  RSX_SPKT_SETLENGTH(rsx->pkt, 2 * words /* Byte */);

  errno_t eno = rsx_oneshot_sync_read_impl(rsx, hrs, 2 * words /* Byte */);

  return eno;
}

/* this command would be disabled after saving 60k [times]. */
errno_t rsx_oneshot_save_rom (
    rsx* rsx, hr_serial* hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0x40));
  RSX_SPKT_SETADDR(rsx->pkt, 0xFF);
  RSX_SPKT_SETLENGTH(rsx->pkt, 0);
  RSX_SPKT_SETCOUNT(rsx->pkt, 0);

  ECALL(rsx_oneshot_write_impl(rsx, hrs));

  return EOK;
}

errno_t rsx_oneshot_reboot (
    rsx* rsx, hr_serial* hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0x20));
  RSX_SPKT_SETADDR(rsx->pkt, 0xFF);
  RSX_SPKT_SETLENGTH(rsx->pkt, 0);
  RSX_SPKT_SETCOUNT(rsx->pkt, 0);

  ECALL(rsx_oneshot_write_impl(rsx, hrs));

  return EOK;
}

/* this command change ID to 0x01 */
errno_t rsx_oneshot_factory_reset (
    rsx* rsx, hr_serial* hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0x10));
  RSX_SPKT_SETADDR(rsx->pkt, 0xFF);
  RSX_SPKT_SETLENGTH(rsx->pkt, 0xFF);
  RSX_SPKT_SETCOUNT(rsx->pkt, 0);

  ECALL(rsx_oneshot_write_impl(rsx, hrs));

  return EOK;
}

errno_t rsx_oneshot_sync_write_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t data[/*size*/], size_t size) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.
  EVALUE(NULL, data);

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0x00));
  RSX_SPKT_SETADDR(rsx->pkt, addr);
  RSX_SPKT_SETLENGTH(rsx->pkt, size /* Byte */);
  RSX_SPKT_SETCOUNT(rsx->pkt, 0x01);
  for (size_t i = 0; i < size; i++) {
    RSX_SPKT_SET_U8(rsx->pkt, i, data[i]);
  }

  ECALL(rsx_oneshot_sync_write_impl(rsx, hrs, size /* Byte */));

  return EOK;
}

errno_t rsx_oneshot_sync_write_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t data[/*words*/], size_t words) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0x00));
  RSX_SPKT_SETADDR(rsx->pkt, addr);
  RSX_SPKT_SETLENGTH(rsx->pkt, 2 * words /* Byte */);
  RSX_SPKT_SETCOUNT(rsx->pkt, 0x01);
  for (size_t i = 0; i < words; i++) {
    RSX_SPKT_SET_INT16(rsx->pkt, i, data[i]);
  }

  ECALL(rsx_oneshot_sync_write_impl(rsx, hrs, 2 * words /* Byte */));

  return EOK;
}

errno_t rsx_bulk_write_byte (  //
    rsx* rsx, hr_serial* hrs,  //
    uint8_t addr, uint8_t ids[/*num*/], uint8_t data[/*num*/], size_t num) {
  ECALL(rsx_bulk_write_bytes_impl(rsx, hrs, addr, ids, data, num));
  return EOK;
}

errno_t rsx_bulk_write_word (  //
    rsx* rsx, hr_serial* hrs,  //
    uint8_t addr, uint8_t ids[/*num*/], uint16_t data[/*num*/], size_t num) {
  ECALL(rsx_bulk_write_words_impl(rsx, hrs, addr, ids, data, num));
  return EOK;
}

#if 0
errno_t rsx_bulk_write_words (  //
    rsx* rsx, hr_serial* hrs,           //
    uint8_t addr, uint8_t ids[/*num*/], uint16_t data[/*num*/][/*size*/], size_t num, size_t size) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_sync_write_words_impl(rsx, hrs, id, addr, data, num));
  return EOK;
}
#endif

errno_t rsx_oneshot_read_byte (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t* data) {
  EVALUE(NULL, data);
  errno_t eno = rsx_oneshot_read_bytes_impl(rsx, hrs, id, addr, 1);
  if (eno == EOK) {
    *data = RSX_SPKT_GET_U8(rsx->pkt, 0);
  }
  return eno;
}

errno_t rsx_oneshot_read_bytes (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint8_t data[num]) {
  EVALUE(NULL, data);
  errno_t eno = rsx_oneshot_read_bytes_impl(rsx, hrs, id, addr, num);
  if (eno == EOK) {
    // FIXME(takara.kasai@gmail.com): memcpy to be used.
    for (size_t i = 0; i < num; i++) {
      data[i] = RSX_SPKT_GET_U8(rsx->pkt, i);
    }
  }
  return eno;
}

errno_t rsx_oneshot_read_word (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t* data) {
  EVALUE(NULL, data);
  errno_t eno = rsx_oneshot_read_words_impl(rsx, hrs, id, addr, 1);
  if (eno == EOK) {
    *data = rsx_get_u16(RSX_SPKT_GET_U8(rsx->pkt, 1), RSX_SPKT_GET_U8(rsx->pkt, 0));
  }
  return eno;
}

errno_t rsx_oneshot_read_words (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint16_t data[num]) {
  EVALUE(NULL, data);
  errno_t eno = rsx_oneshot_read_words_impl(rsx, hrs, id, addr, num);
  if (eno == EOK) {
    for (size_t i = 0; i < num; i++) {
      data[i] = rsx_get_u16(RSX_SPKT_GET_U8(rsx->pkt, 2 * i + 1),  //
                            RSX_SPKT_GET_U8(rsx->pkt, 2 * i + 0));
    }
  }
  return eno;
}

errno_t rsx_oneshot_sync_write_byte (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t data) {
  ECALL(rsx_oneshot_sync_write_bytes_impl(rsx, hrs, id, addr, &data, 1));
  return EOK;
}

errno_t rsx_oneshot_sync_write_bytes (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint8_t data[num]) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_sync_write_bytes_impl(rsx, hrs, id, addr, data, num));
  return EOK;
}

errno_t rsx_oneshot_sync_write_word (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t data) {
  ECALL(rsx_oneshot_sync_write_words_impl(rsx, hrs, id, addr, &data, 1));
  return EOK;
}

errno_t rsx_oneshot_sync_write_words (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint16_t data[num]) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_sync_write_words_impl(rsx, hrs, id, addr, data, num));
  return EOK;
}

errno_t rsx_servo_set_control_mode(rsx* rsx, hr_serial* hrs, uint8_t id, rsx_control_mode_t mode) {
  ECALL(rsx_oneshot_sync_write_byte(rsx, hrs, id, 0x15, mode));
  return EOK;
}

errno_t rsx_servo_on(rsx* rsx, hr_serial* hrs, uint8_t id) {
  ECALL(rsx_oneshot_sync_write_byte(rsx, hrs, id, 0x24, 1));
  return EOK;
}

errno_t rsx_servo_off(rsx* rsx, hr_serial* hrs, uint8_t id) {
  ECALL(rsx_oneshot_sync_write_byte(rsx, hrs, id, 0x24, 0));
  return EOK;
}

errno_t rsx_set_goal_torque_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal) {
  ECALL(rsx_oneshot_sync_write_word(rsx, hrs, id, 0x27, goal));
  return EOK;
}

errno_t rsx_set_goal_velocity_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal) {
  ECALL(rsx_oneshot_sync_write_word(rsx, hrs, id, 0x25, goal));
  return EOK;
}

errno_t rsx_set_goal_position_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal) {
  ECALL(rsx_oneshot_sync_write_word(rsx, hrs, id, 0x1e, goal));
  return EOK;
}

/* goal : [-100.0, 100.0] [%] */
errno_t rsx_set_goal_torque(rsx* rsx, hr_serial* hrs, uint8_t id, float goal) {
  ECALL(rsx_oneshot_sync_write_word(rsx, hrs, id, 0x27, (int16_t)(goal * 10)));
  return EOK;
}

/* goal : [-3270.0, 3270.0] [deg] */
errno_t rsx_set_goal_position(rsx* rsx, hr_serial* hrs, uint8_t id, float goal) {
  ECALL(rsx_oneshot_sync_write_word(rsx, hrs, id, 0x1e, (int16_t)(goal * 10)));
  return EOK;
}

errno_t rsx_set_goal_torques_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num) {
  ECALL(rsx_bulk_write_word(rsx, hrs, 0x27, ids, (uint16_t*)goals, num));
  return EOK;
}

errno_t rsx_set_goal_velocities_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num) {
  ECALL(rsx_bulk_write_word(rsx, hrs, 0x25, ids, (uint16_t*)goals, num));
  return EOK;
}

errno_t rsx_set_goal_positions_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num) {
  ECALL(rsx_bulk_write_word(rsx, hrs, 0x1e, ids, (uint16_t*)goals, num));
  return EOK;
}

errno_t rsx_set_goal_torques(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], float goals[/*num*/], size_t num) {
  int16_t i_goals[num];
  for (size_t i = 0; i < num; i++) {
    i_goals[i] = (int16_t)(goals[i] * 10);
  }
  ECALL(rsx_bulk_write_word(rsx, hrs, 0x27, ids, (uint16_t*)i_goals, num));
  return EOK;
}

errno_t rsx_set_goal_positions(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], float goals[/*num*/], size_t num) {
  int16_t i_goals[num];
  for (size_t i = 0; i < num; i++) {
    i_goals[i] = (int16_t)(goals[i] * 10);
  }
  ECALL(rsx_bulk_write_word(rsx, hrs, 0x1e, ids, (uint16_t*)i_goals, num));
  return EOK;
}

/* command to check connection */
errno_t rsx_check_connection(rsx* rsx, hr_serial* hrs, uint8_t id) {
  /* check id */
  uint8_t rid;
  errno_t eno = rsx_oneshot_read_byte(rsx, hrs, id, 0x04, &rid);
  if (eno == EOK) {
    if (id == rid) {
      return EOK;
    }
  }
  return -1;
}

/* command w/ payload less */
/*  -- save ROM area */
errno_t rsx_save_rom(rsx* rsx, hr_serial* hrs, uint8_t id) {
  ECALL(rsx_oneshot_save_rom(rsx, hrs, id));
  return EOK;
}

errno_t rsx_reboot(rsx* rsx, hr_serial* hrs, uint8_t id) {
  ECALL(rsx_oneshot_reboot(rsx, hrs, id));
  return EOK;
}

errno_t rsx_factory_reset(rsx* rsx, hr_serial* hrs, uint8_t id) {
  ECALL(rsx_oneshot_factory_reset(rsx, hrs, id));
  return EOK;
}

/* command to change ROM area */
errno_t rsx_set_id(rsx* rsx, hr_serial* hrs, uint8_t id_from, uint8_t id_to) {
  ECALL(rsx_oneshot_sync_write_byte(rsx, hrs, id_from, 0x04, id_to));
  return EOK;
}

errno_t rsx_set_baudrate(rsx* rsx, hr_serial* hrs, uint8_t id, int baudrate) {
  uint8_t req;
  ECALL(int_to_baudrate(baudrate, &req));
  ECALL(rsx_oneshot_sync_write_byte(rsx, hrs, id, 0x06, req));
  return EOK;
}

errno_t rsx_get_baudrate(rsx* rsx, hr_serial* hrs, uint8_t id, int* baudrate) {
  EVALUE(NULL, baudrate);
  uint8_t rep;
  ECALL(rsx_oneshot_read_byte(rsx, hrs, id, 0x06, &rep));
  return baudrate_to_int(rep, baudrate);
}

/* utility commands */
errno_t rsx_search_servo(rsx* rsx, hr_serial* hrs, rsx_device_info* info) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);
  EVALUE(NULL, info);

  printf("===== searching servo =====\n");
  size_t total_count = 0;
  for (int i = 0; i < sizeof(kAvailableBaudrates)/sizeof(kAvailableBaudrates[0]); i++) {
    printf("== %2d baudrate:%d", i, kAvailableBaudrates[i]);
    errno_t eno = hr_serial_set_baudrate(hrs, kAvailableBaudrates[i]);
    if (eno != EOK) {
      printf(" -- skip\n");
      continue;
    }
    printf("\n");

    printf(" --");
    info->group[i].num_of_devices = 0;
    for (uint8_t r_id = 0; r_id < 127; r_id++) {
      printf("\r %5.1f%% :", 100.0 * r_id / 126);
      for (ssize_t j = 0; j < info->group[i].num_of_devices; j++) {
        printf(" %02x", info->group[i].ids[j]);
      }
      fflush(stdout);

      eno = rsx_check_connection(rsx, hrs, r_id);
      if (eno == EOK) {
        printf(" %0x", r_id);
        fflush(stdout);
        info->group[i].baudrate = kAvailableBaudrates[i];
        info->group[i].ids[info->group[i].num_of_devices] = r_id;
        info->group[i].num_of_devices++;
        total_count++;
      }
    }
    printf("\n");
  }
  if (total_count == 0) {
    return -1;
  }
  return EOK;
}
