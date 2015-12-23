
#ifndef ICS3X_H
#define ICS3X_H

#include "ics_type.h"

#define ICS30_EEPROM_SIZE 0x1D /* 58 [byte] */
#define ICS35_EEPROM_SIZE 0x20 /* 64 [byte] */

static const ics_opt_t kICS_VER_MASK = 0x3;

#define ICS_VER_20 0x01 /* not implemented */
#define ICS_VER_30 0x02
#define ICS_VER_35 0x03

typedef enum {
  ICS30_ROM_PUNCH          = 0x03,
  ICS30_ROM_DEAD_BAND      = 0x04,
  ICS30_ROM_DAMPING        = 0x05,
  ICS30_ROM_SAFE_TIMER     = 0x06,
  ICS30_ROM_FLAG           = 0x07,
  ICS30_ROM_PULSE_LLIMIT_H = 0x08,
  ICS30_ROM_PULSE_LLIMIT_L = 0x09,
  ICS30_ROM_PULSE_HLIMIT_H = 0x0A,
  ICS30_ROM_PULSE_HLIMIT_L = 0x0B,

  ICS30_ROM_BAUDRATE       = 0x0D,
  ICS30_ROM_TEMP_LIMIT     = 0x0E,
  ICS30_ROM_CUR_LIMIT      = 0x0F,

  ICS30_ROM_STRETCH_GAIN   = 0x11,
  ICS30_ROM_SPEED          = 0x12,

  ICS30_ROM_OFFSET         = 0x1B,
  ICS30_ROM_ID             = 0x1C
} ICS30_MEM_ADDR;

typedef enum {
  ICS35_ROM_STRETCH_GAIN   = 0x01,
  ICS35_ROM_SPEED          = 0x02,
  ICS35_ROM_PUNCH          = 0x03,
  ICS35_ROM_DEAD_BAND      = 0x04,
  ICS35_ROM_DAMPING        = 0x05,
  ICS35_ROM_SAFE_TIMER     = 0x06,
  ICS35_ROM_FLAG           = 0x07,
  ICS35_ROM_PULSE_LLIMIT_H = 0x08,
  ICS35_ROM_PULSE_LLIMIT_L = 0x09,
  ICS35_ROM_PULSE_HLIMIT_H = 0x0A,
  ICS35_ROM_PULSE_HLIMIT_L = 0x0B,

  ICS35_ROM_BAUDRATE       = 0x0D,
  ICS35_ROM_TEMP_LIMIT     = 0x0E,
  ICS35_ROM_CUR_LIMIT      = 0x0F,

  ICS35_ROM_RESPONSE       = 0x19,
  ICS35_ROM_USER_OFFSET    = 0x1A,

  ICS35_ROM_ID             = 0x1C,
  ICS35_ROM_C_CHG_STRETCH1 = 0x1D,
  ICS35_ROM_C_CHG_STRETCH2 = 0x1E,
  ICS35_ROM_C_CHG_STRETCH3 = 0x1F
} ICS35_MEM_ADDR;

static inline size_t ics_get_eeprom_size (dps_opt_t option) {
  switch (kICS_VER_MASK & option) {
    case ICS_VER_30: return ICS30_EEPROM_SIZE;
    case ICS_VER_35: return ICS35_EEPROM_SIZE;
    default:         return ICS30_EEPROM_SIZE;
  }
  return 0;
}

#endif

