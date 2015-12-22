
#ifndef ICS_H
#define ICS_H

/* interface */
#include "dpservo.h"

#include "dp_type.h"
#include "dp_err.h"

// for hr_serial
#include "serial/hr_serial.h"

#define ICS_DECL(name, num_of_servo)  \
    ics name

#define ICS_INIT(name)                \
    ics_init(&name)

#define ICS_SERVO_ID_MIN 0x00 //  0
#define ICS_SERVO_ID_MAX 0x1F // 31

#define ICS_REP_CMD_MSB(servoid, baudrate, data) \
  ((((servoid) == 0) && ((baudrate) == ICS_UART_RATE_115200)) ? (0x80 | (data)) : (0x7F & (data)))
#define ICS_SER_REQ_CMD(servoid, cmdid) ((servoid & 0x1F) | (cmdid & 0xE0))
#define ICS_SER_REP_CMD(servoid, cmdid, baudrate) \
  ICS_REP_CMD_MSB(servoid, baudrate, ICS_SER_REQ_CMD(servoid, cmdid))
#define ICS_SER_REQ_SUBCMD(scmdid) (scmdid)
#define ICS_SER_POS_L(pos) (0x7F & (pos     ))
#define ICS_SER_POS_H(pos) (0x7F & (pos >> 7))
#define ICS_GET_POS(datah, datal)  ((((uint16_t)datah) << 7) | datal)

typedef enum {
  ICS_CMD_REQ_POS        = 0x80,
  ICS_CMD_REQ_READ_PARAM = 0x90,
} ICS_CMDID;

typedef enum {
  ICS_SCMD_EEPROM  = 0x00,
  ICS_SCMD_STRETCH = 0x01,
  ICS_SCMD_SPEED   = 0x02,
  ICS_SCMD_CURRENT = 0x03,
  ICS_SCMD_TEMP    = 0x04
} ICS_SUB_CMDID;

typedef enum {
  ICS_UART_RATE_115200,
  ICS_UART_RATE_625000,
  ICS_UART_RATE_1250000,
} ICS_UART_RATE;

typedef struct {
  dpservo_base base; /* inherit dpservo */

  size_t retry_count;

  /* debug */
} ics;

errno_t ics_init (ics *ics);
//errno_t ics_ser_set_pos_cmd (rsx *rsx, uint8_t id, uint8_t cmdid, uint16_t pos, uint8_t max_size, uint8_t data[/*max_size*/], size_t *serialized_size);
errno_t ics_ser_set_pos_cmd (ics* ics, uint8_t id, uint8_t cmdid, uint16_t pos, size_t *serialized_size);
errno_t ics_deser_set_pos_cmd (ics* ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos, ICS_UART_RATE baudrate);
errno_t ics_ser_get_param_cmd (ics* ics, uint8_t id, uint8_t cmdid, uint8_t scmdid, size_t *serialized_size);
//errno_t ics_deser_get_param_cmd (uint8_t id, uint8_t cmdid, uint8_t scmdid, uint8_t size, uint8_t data[/*max_size*/], uint8_t max_size, uint8_t rdata[], ICS_UART_RATE baudrate);
errno_t ics_deser_get_param_cmd (ics *ics, uint8_t id, uint8_t cmdid, uint8_t scmdid, uint8_t max_size, uint8_t rdata[], ICS_UART_RATE baudrate);

#endif
