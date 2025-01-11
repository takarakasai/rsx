#ifndef RSX_H
#define RSX_H

#include <stdint.h>
#include <stddef.h>

// for size_t, ssize_t
#include <unistd.h>

#include <assert.h>

// for hr_serial
#include <rsx_io.h>
#include <rsx_raw.h>

typedef enum {
  RSX_POS_CONTROL = 0x00,
  RSX_MIX_CONTROL = 0x01
} rsx_control_mode_t;

inline uint16_t rsx_get_u16(uint8_t hbyte, uint8_t lbyte) {
  return (((uint16_t)hbyte) << 8) | lbyte;
}

/*
 * type definition
 */

typedef struct {
  ssize_t num_of_servos;
  ssize_t max_payload_size;
} rsx_config;

typedef struct {
  rsx_config config;

  ssize_t max_frame_size;
  ssize_t write_size;  /* for debug */
  ssize_t read_size;  /* for debug */
  uint8_t* wbuff;
  uint8_t* rbuff;

  ssize_t max_payload_size;
  uint8_t* payload;

  rsx_pkt pkt;
} rsx;

errno_t rsx_config_init(rsx_config* config);
errno_t rsx_config_copy(rsx_config* from, rsx_config* to);

errno_t rsx_init(rsx* rsx, rsx_config* config);
errno_t rsx_destroy(rsx* rsx);

inline uint8_t rsx_spkt_get_u8(rsx* rsx) {
  assert(rsx);
  return RSX_SPKT_GET_U8(rsx->pkt, 0);
}

inline int8_t rsx_spkt_get_i8(rsx* rsx) {
  assert(rsx);
  int8_t byte = (int8_t)(rsx_spkt_get_u8(rsx));
  return byte;
  return RSX_SPKT_GET_U8(rsx->pkt, 0);
}

inline uint16_t rsx_spkt_get_u16(rsx* rsx) {
  assert(rsx);
  uint8_t lbyte = RSX_SPKT_GET_U8(rsx->pkt, 0);
  uint8_t hbyte = RSX_SPKT_GET_U8(rsx->pkt, 1);
  uint16_t word = (((uint16_t)hbyte) << 8) | lbyte;
  return word;
}

inline int16_t rsx_spkt_get_i16(rsx* rsx) {
  assert(rsx);
  int16_t word = (int16_t)(rsx_spkt_get_u16(rsx));
  return word;
}

errno_t rsx_bulk_write_impl (rsx* rsx, hr_serial* hrs);
errno_t rsx_oneshot_read_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size);
errno_t rsx_oneshot_sync_write_impl (
    rsx* rsx, hr_serial* hrs, ssize_t payload_size);

//////////////////////////////////////////////////////////////

errno_t rsx_bulk_write_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t addr, 
    uint8_t ids[/*num*/], uint8_t data[/*num*/], size_t num);
errno_t rsx_bulk_write_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t addr,
    uint8_t ids[/*num*/], uint16_t data[/*num*/], size_t num);

errno_t rsx_oneshot_read_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t size);
errno_t rsx_oneshot_read_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t words);

errno_t rsx_oneshot_sync_write_bytes_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t data[/*size*/], size_t size);
errno_t rsx_oneshot_sync_write_words_impl (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t data[/*words*/], size_t words);

/////////////////////////////////////////////////////////////

errno_t rsx_bulk_write_byte (  //
    rsx* rsx, hr_serial* hrs,  //
    uint8_t addr, uint8_t ids[/*num*/], uint8_t data[/*num*/], size_t num);
errno_t rsx_bulk_write_word (  //
    rsx* rsx, hr_serial* hrs,  //
    uint8_t addr, uint8_t ids[/*num*/], uint16_t data[/*num*/], size_t num);

errno_t rsx_oneshot_read_byte (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t* data);
errno_t rsx_oneshot_read_bytes (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint8_t data[num]);

errno_t rsx_oneshot_read_word (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t* data);
errno_t rsx_oneshot_read_words (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint16_t data[num]);

errno_t rsx_oneshot_sync_write_byte (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t data);
errno_t rsx_oneshot_sync_write_bytes (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint8_t data[num]);
errno_t rsx_oneshot_sync_write_word (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint16_t data);
errno_t rsx_oneshot_sync_write_words (
    rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, size_t num, uint16_t data[num]);

errno_t rsx_servo_set_control_mode(rsx* rsx, hr_serial* hrs, uint8_t id, rsx_control_mode_t mode);
errno_t rsx_servo_on(rsx* rsx, hr_serial* hrs, uint8_t id);
errno_t rsx_servo_off(rsx* rsx, hr_serial* hrs, uint8_t id);

/* torque:[-1000, 1000] [0.1%], veloc:[-100,100] [rpm], pos:[-32700,32700] [0.1deg] */
errno_t rsx_set_goal_torque_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal);
errno_t rsx_set_goal_velocity_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal);
errno_t rsx_set_goal_position_raw(rsx* rsx, hr_serial* hrs, uint8_t id, int16_t goal);

/* goal : [-100.0, 100.0] [%] */
errno_t rsx_set_goal_torque(rsx* rsx, hr_serial* hrs, uint8_t id, float goal);
/* goal : [-3270.0, 3270.0] [deg] */
errno_t rsx_set_goal_position(rsx* rsx, hr_serial* hrs, uint8_t id, float goal);

errno_t rsx_set_goal_toruqes_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num);
errno_t rsx_set_goal_velocities_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num);
errno_t rsx_set_goal_positions_raw(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], int16_t goals[/*num*/], size_t num);

errno_t rsx_set_goal_torques(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], float goals[/*num*/], size_t num);
errno_t rsx_set_goal_positions(  //
    rsx* rsx, hr_serial* hrs, uint8_t ids[/*num*/], float goals[/*num*/], size_t num);

/* command to check connection */
errno_t rsx_check_connection(rsx* rsx, hr_serial* hrs, uint8_t id);
 
/* command w/ payload less */
/*  -- save ROM area */
errno_t rsx_save_rom(rsx* rsx, hr_serial* hrs, uint8_t id);
errno_t rsx_reboot(rsx* rsx, hr_serial* hrs, uint8_t id);
errno_t rsx_factory_reset(rsx* rsx, hr_serial* hrs, uint8_t id);

/* command to change ROM area */
errno_t rsx_set_id(rsx* rsx, hr_serial* hrs, uint8_t id_from, uint8_t id_to);
errno_t rsx_set_baudrate(rsx* rsx, hr_serial* hrs, uint8_t id, int baudrate);
errno_t rsx_get_baudrate(rsx* rsx, hr_serial* hrs, uint8_t id, int* baudrate);

/* utility commands */
errno_t rsx_search_servo(rsx* rsx, hr_serial* hrs, int* bandrate, uint8_t* id);

#endif
