
#ifndef RSX_PKT_H
#define RSX_PKT_H

#if defined(DATA_DUMP)
#include <stdio.h>
#endif

#include "rsx_type.h"

#define EVALUE(value, variable) \
  if ((value) == (variable)) {      \
    return EINVAL;              \
  }

#define ELTGT(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue < variable)) {  \
    printf("%d %d %d\n", (int)lvalue, (int)hvalue, (int)variable);  \
    return EINVAL;                                   \
  }

#define ELTGE(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue <= variable)) { \
    return EINVAL;                                   \
  }

#define ELEGT(lvalue, hvalue, variable)              \
  if ((variable <= lvalue) || (hvalue < variable)) { \
    return EINVAL;                                   \
  }

#define EOK 0

#define ECALL(function)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {       \
      fprintf(stderr, "--> %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); \
      return eno;           \
    }                       \
  } while(0)

/*
 * @length  max length of data payload
 * @count   max num of servo
 */
#define RSX_PKT_DECL(name, num_of_servo, length)  \
  uint8_t name ## _data[num_of_servo][length];    \
  rsx_pkt name;
#define RSX_LPKT_DECL(name, num_of_servo, length) RSX_PKT_DECL(name, num_of_servo, length)
#define RSX_SPKT_DECL(name, length)               RSX_PKT_DECL(name, 1, length)

#define RSX_PKT_INIT(name)                                   \
  do {                                                       \
    rsx_pkt_init(&name,                                      \
      sizeof(name ## _data) / sizeof(name ## _data[0]),      \
      sizeof(name ## _data[0]) / sizeof(name ## _data[0][0]),\
      (uint8_t*)(name ## _data));                            \
  } while(0)
#define RSX_LPKT_INIT(name) RSX_PKT_INIT(name)
#define RSX_SPKT_INIT(name) RSX_PKT_INIT(name)

#define RSX_PKT_GETLENGTH(pkt)          ((pkt).length)
#define RSX_PKT_GETCOUNT(pkt)           ((pkt).count)
#define RSX_PKT_SET_U8(pkt, count, idx, ofst, val) (((uint8_t*)(pkt).data)[count * (pkt).length + (idx + ofst)] = val)
#define RSX_PKT_GET_U8(pkt, count, idx, ofst)      (((uint8_t*)(pkt).data)[count * (pkt).length + (idx + ofst)])

//#define RSX_LPKT_SETID(pkt, count, id)  do{pkt.data[count * pkt.length] = id;}while(0)
#define RSX_LPKT_SETVID(pkt, count, id) do{pkt.data[count * pkt.length] = id;}while(0)
#define RSX_LPKT_SETFLAG(pkt, val)      do{pkt.flag    = val    ;}while(0)
#define RSX_LPKT_GETFLAG(pkt)           ((pkt).flag)
#define RSX_LPKT_SETADDR(pkt, val)      do{pkt.address = val    ;}while(0)
#define RSX_LPKT_SETLENGTH(pkt, val)    do{pkt.length  = val + sizeof(uint8_t);}while(0) // sizeof(uint8_t) is for id size:
#define RSX_LPKT_SETCOUNT(pkt, val)     do{pkt.count   = val  ;}while(0)
#define RSX_LPKT_GETLENGTH(pkt)         RSX_PKT_GETLENGTH(pkt)
#define RSX_LPKT_GETCOUNT(pkt)          RSX_PKT_GETCOUNT(pkt)
#define RSX_LPKT_SET_U8(pkt, count, idx, data) RSX_PKT_SET_U8(pkt, count, idx, 1, data)
#define RSX_LPKT_GET_U8(pkt, count, idx)       RSX_PKT_GET_U8(pkt, count, idx, 1)
#define RSX_LPKT_SET_INT16(pkt, count, idx, val)                \
    do {                                                        \
        RSX_LPKT_SET_U8(pkt, count, idx    , (val     ) & 0xFF);\
        RSX_LPKT_SET_U8(pkt, count, idx + 1, (val >> 8) & 0xFF);\
    } while(0)

#define RSX_SPKT_SETID(pkt, val)        do{(pkt).id      = val;}while(0)
#define RSX_SPKT_GETID(pkt)             ((pkt).id)
#define RSX_SPKT_SETFLAG(pkt, val)      do{(pkt).flag    = val;}while(0)
#define RSX_SPKT_GETFLAG(pkt)           ((pkt).flag)
#define RSX_SPKT_SETADDR(pkt, val)      do{(pkt).address = val;}while(0)
#define RSX_SPKT_SETLENGTH(pkt, val)    do{(pkt).length  = val;}while(0)
#define RSX_SPKT_GETLENGTH(pkt)         RSX_PKT_GETLENGTH(pkt)
#define RSX_SPKT_GETCOUNT(pkt)          RSX_PKT_GETCOUNT(pkt)
#define RSX_SPKT_SET_U8(pkt, idx, data) RSX_PKT_SET_U8(pkt, 0, idx, 0, data)
#define RSX_SPKT_GET_U8(pkt, idx)       RSX_PKT_GET_U8(pkt, 0, idx, 0)
#define RSX_SPKT_SET_INT16(pkt, idx, data)                \
    do {                                                  \
        RSX_SPKT_SET_U8(pkt, idx    , (data     ) & 0xFF);\
        RSX_SPKT_SET_U8(pkt, idx + 1, (data >> 8) & 0xFF);\
    } while(0)

#define RSX_PKT_ID_LONG 0x00
#define RSX_PKT_ID_ALL  0xFF

/* upper 4bit */
#define RSX_SPKT_SND_FLG_DEFAULT   0x00 /* 0b 0000 0000 */
#define RSX_SPKT_SND_FLG_WRFLASH   0x40 /* 0b 0100 0000 */
#define RSX_SPKT_SND_FLG_RESTART   0x20 /* 0b 0010 0000 */
#define RSX_SPKT_SND_FLG_RST_MEM   0x10 /* 0b 0001 0000 */
/* lower 4bit */
#define RSX_SPKT_RTN_FLG_RTNLESS   0x00 /* 0b 0000 0000 */
#define RSX_SPKT_RTN_FLG_ACKNACK   0x01 /* 0b 0000 0001 */
#define RSX_SPKT_RTN_FLG_00TO029   0x03 /* 0b 0000 0011 */
#define RSX_SPKT_RTN_FLG_30TO059   0x05 /* 0b 0000 0101 */
#define RSX_SPKT_RTN_FLG_20TO029   0x06 /* 0b 0000 0111 */
#define RSX_SPKT_RTN_FLG_42TO059   0x09 /* 0b 0000 1001 */
#define RSX_SPKT_RTN_FLG_30TO041   0x0B /* 0b 0000 1011 */
#define RSX_SPKT_RTN_FLG_60TO127   0x0C /* 0b 0000 1101 */
#define RSX_SPKT_RTN_FLG_ADDR      0x0F /* 0b 0000 1111 */

#define RSX_DATA_SERVO_OFF 0x00
#define RSX_DATA_SERVO_ON  0x01
#define RSX_DATA_SERVO_BRK 0x02

typedef struct {
  uint16_t    header; /* magic number for short packet. */
  uint8_t         id; /* 1 to 127 (01H to 7FH)    */
  uint8_t       flag;
  uint8_t    address;
  uint8_t     length;
  uint8_t      count;
  uint8_t      *data;
  uint8_t  check_sum;
} rsx_pkt; // RS301/302 short/long packet

errno_t rsx_pkt_get_size (rsx_pkt *pkt, size_t *size);
errno_t rsx_pkt_init  (rsx_pkt *pkt, uint8_t length, uint8_t count, void *data);
errno_t rsx_pkt_reset (rsx_pkt *pkt);
errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size);
errno_t rsx_pkt_ser   (rsx_pkt *pkt, uint8_t dst[/*max_num*/], size_t max_num, size_t *size);

static inline errno_t data_dump (uint8_t *data, size_t size) {
  EVALUE(NULL, data);

#if defined(DATA_DUMP)
  printf("size:%04zd ::", size);
  for (size_t i = 0; i < size; i++) {
    printf(" %02x", data[i]);
  }
  printf("\n");
#endif

  return EOK;
}

#endif

