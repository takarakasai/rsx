#include "rsx.h"

#include <stdlib.h>

errno_t rsx_config_init(  //
    rsx_config* config) {
  EVALUE(NULL, config);

  config->num_of_servos    = 8;
  config->max_payload_size = 6;

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
  EVALUE(NULL, config);

  rsx->wbuff = NULL;
  rsx->rbuff = NULL;
  rsx->payload = NULL;
  rsx->pkt.data = NULL;

  ECALL(rsx_config_copy(config, &rsx->config));

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

  return EOK;
}

errno_t rsx_oneshot_read_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size) {
  EVALUE(NULL, rsx);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));

  size_t count = 0;
  rsx->read_size = pkt_size + payload_size;
  do {
    errno_t eno = 0;
    // ECALL(data_dump(rsx->wbuff, pkt_size));
    ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));
    usleep(100 * 1000);

    // RSX_SPKT_SETFLAG(rsx->pkt, addr);

    eno = hr_serial_read(hrs, rsx->rbuff, rsx->read_size);
    if (eno == EOK) {
      // ECALL(data_dump(rsx->rbuff, rsx->read_size));
      eno = rsx_pkt_deser(&(rsx->pkt), rsx->rbuff, rsx->max_frame_size, &pkt_size);
      if (eno == EOK) {
        break;
      }
    }
  } while (count++ < 10);

  assert(count < 10);

  return EOK;
}

errno_t rsx_oneshot_sync_write_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  size_t pkt_size;
  ECALL(rsx_pkt_ser(&(rsx->pkt), rsx->wbuff, rsx->max_frame_size, &pkt_size));
  // ECALL(data_dump(rsx->wbuff, pkt_size));
  ECALL(hr_serial_write(hrs, rsx->wbuff, pkt_size));
  usleep(50 * 1000);

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
    usleep(100 * 1000);

    eno = hr_serial_read(hrs, rsx->rbuff, rsx->read_size);
    if (eno == EOK) {
      // ECALL(data_dump(rsx->rbuff, rsx->read_size));
      eno = rsx_pkt_deser_and_check(&(rsx->pkt), rsx->rbuff, rsx->max_frame_size);
      if (eno == EOK) {
        break;
      }
    }
  } while (count++ < 10);

  assert(count < 10);

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

  ECALL(rsx_oneshot_read_impl(rsx, hrs, size /* Byte */));

  return EOK;
}

errno_t rsx_oneshot_read_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t words) {
  EVALUE(NULL, rsx);
  // FIXME(takara.kasai@gmail.com): range check for id, addr.

  // FIXME(takara.kasai@gmail.com): optimize FLAG bits.
  ECALL(rsx_spkt_init(&(rsx->pkt), id, 0xF));
  RSX_SPKT_SETADDR(rsx->pkt, addr);
  RSX_SPKT_SETLENGTH(rsx->pkt, 2 * words /* Byte */);

  ECALL(rsx_oneshot_read_impl(rsx, hrs, 2 * words /* Byte */));

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
  ECALL(rsx_oneshot_read_bytes_impl(rsx, hrs, id, addr, 1));
  *data = RSX_SPKT_GET_U8(rsx->pkt, 0);
  return EOK;
}

errno_t rsx_oneshot_read_bytes (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint8_t data[num]) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_read_bytes_impl(rsx, hrs, id, addr, num));
  // FIXME(takara.kasai@gmail.com): memcpy to be used.
  for (size_t i = 0; i < num; i++) {
    data[i] = RSX_SPKT_GET_U8(rsx->pkt, i);
  }
  return EOK;
}

errno_t rsx_oneshot_read_word (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t* data) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_read_words_impl(rsx, hrs, id, addr, 1));
  *data = rsx_get_u16(RSX_SPKT_GET_U8(rsx->pkt, 1), RSX_SPKT_GET_U8(rsx->pkt, 0));
  return EOK;
}

errno_t rsx_oneshot_read_words (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint16_t data[num]) {
  EVALUE(NULL, data);
  ECALL(rsx_oneshot_read_words_impl(rsx, hrs, id, addr, num));
  for (size_t i = 0; i < num; i++) {
    data[i] = rsx_get_u16(RSX_SPKT_GET_U8(rsx->pkt, 2 * i + 1),  //
                          RSX_SPKT_GET_U8(rsx->pkt, 2 * i + 0));
  }
  return EOK;
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
