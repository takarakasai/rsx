
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

/* virtual memory address / size */
#define ICS_VMEM_ADDR_MIN 0x00
#define ICS_VMEM_ADDR_MAX 0x7F
#define ICS_VMEM_MAX_SIZE(start_addr) (start_addr > ICS_VMEM_ADDR_MAX) ? 0 : (ICS_VMEM_ADDR_MAX - start_addr)
#define ICS_VMEM_PAYLOAD_STEP_SIZE (2)
#define ICS_VMEM_PAYLOAD_SIZE(size) (ICS_VMEM_PAYLOAD_STEP_SIZE * size)
#define ICS_VMEM_DESER_PAYLOAD(buff, bidx, data, didx) do{ data[didx]   = ((buff[bidx] << 4) | (buff[bidx + 1] & 0x0F)); bidx += 2; }while(0)
#define ICS_VMEM_SER_PAYLOAD(buff, bidx, data, didx)   do{ buff[bidx++] = ((data[didx] >> 4) & 0x0F); buff[bidx++] = (data[didx] & 0x0F); }while(0)

/* for specific command  */
#define ICS_REP_CMD_MSB_ICS30(data)         (data)  // MSB:as is
#define ICS_REP_CMD_MSB_ICS35(data) (0x7F & (data)) // MSB:0
#define ICS_SER_REQ_CMD(servoid, cmdid) ((servoid & 0x1F) | (cmdid & 0xE0))
#define ICS_SER_REP_CMD(servoid, cmdid) \
  ICS_REP_CMD_MSB_ICS35(ICS_SER_REQ_CMD(servoid, cmdid))

#define ICS_SER_REQ_SUBCMD(scmdid) (scmdid)

/* for specific command like pos command */
#define ICS_REP_POS_CMD_MSB(servoid, baudrate, data)  \
  ((((servoid) == 0) && ((baudrate) == ICS_UART_RATE_115200)) ? (0x80 | (data)) : (0x7F & (data)))
#define ICS_SER_REQ_POS_CMD(servoid, cmdid)           \
  ICS_SER_REQ_CMD(servoid, cmdid)
#define ICS_SER_REP_POS_CMD(servoid, cmdid, baudrate) \
  ICS_REP_POS_CMD_MSB(servoid, baudrate, ICS_SER_REQ_CMD(servoid, cmdid))
#define ICS_SER_POS_L(pos) (0x7F & (pos     ))
#define ICS_SER_POS_H(pos) (0x7F & (pos >> 7))
#define ICS_GET_POS(datah, datal)  ((((uint16_t)datah) << 7) | datal)

/* for general command  */
#define ICS_SER_REQ_GPCMD(servoid, cmdid) ICS_SER_REQ_CMD(servoid, cmdid)
#define ICS_SER_REP_GPCMD(servoid, cmdid) ICS_SER_REQ_CMD(servoid, cmdid)

typedef enum {
  ICS_CMD_REQ_POS         = 0x80, /* 0b 100x xxxx */
  ICS_CMD_REQ_READ_PARAM  = 0xA0, /* 0b 101x xxxx */ // TODO: to be deleted, GP_READ  should be used.
  ICS_CMD_REQ_WRITE_PARAM = 0xC0, /* 0b 110x xxxx */ // TODO: to be deleted, GP_WRITE should be used.
  ICS_CMD_GP_READ         = 0xA0, /* 0b 101x xxxx */
  ICS_CMD_GP_WRITE        = 0xC0, /* 0b 110x xxxx */
  ICS_CMD_ID_SET          = 0xE0, /* 0b 111x xxxx */
} ICS_CMDID;

/* 
 * following sub command id shall be used with ICS_CMD_GP_READ/ICS_CMD_GP_WRITE
 */
typedef enum {
  ICS_SCMD_EEPROM  = 0x00,
  ICS_SCMD_STRETCH = 0x01,
  ICS_SCMD_SPEED   = 0x02,
  ICS_SCMD_CURRENT = 0x03,
  ICS_SCMD_TEMP    = 0x04,
  ICS_SCMD_VIRTRAM = 0x7F
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
//errno_t ics_ser_set_pos_cmd (ics* ics, uint8_t id, uint8_t cmdid, uint16_t pos, size_t *serialized_size);
//errno_t ics_deser_set_pos_cmd (ics* ics, uint8_t id, uint8_t cmdid, uint16_t refpos, uint16_t *curpos, ICS_UART_RATE baudrate);

//errno_t ics_ser_get_param_cmd (ics *ics, uint8_t id, uint8_t scmdid, size_t *serialized_size);
//errno_t ics_deser_get_param_cmd (ics *ics, uint8_t id, uint8_t scmdid, uint8_t size, uint8_t rdata[]);

#endif
