
#ifndef RSX_H
#define RSX_H

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
    if (eno != EOK) {        \
      return eno;           \
    }                       \
  } while(0)


/*
 * @length  max length of data payload
 * @count   max num of servo
 */
#define RSX_LPKT_DECL(name, count, length) \
  uint8_t name ## _data[count][length];      \
  rsx_vload name ## _vload[count];/*TODO:*/\
  rsx_pkt name;

#define RSX_LPKT_INIT(name) \
  do {                        \
    rsx_pkt_init(&name,     \
      sizeof(name ## _data[0]) / sizeof(name ## _data[0][0]), \
      sizeof(name ## _vload) / sizeof(name ## _vload[0]),/*TODO:*/\
      (uint8_t*)(name ## _data));                             \
    rsx_lpkt_init(&name);                                   \
  } while(0)

#define RSX_SPKT_DECL(name, length) \
  uint8_t name ## _data[length];    \
  rsx_pkt name;

#define RSX_SPKT_INIT(name) \
  do {                        \
    rsx_pkt_init(&name,     \
      sizeof(name ## _data) / sizeof(name ## _data[0]), \
      1,                                                \
      (name ## _data));                                 \
    rsx_lpkt_init(&name);                  /*TODO:*/  \
  } while(0)

#define RSX_PKT_GETLENGTH(pkt)          ((pkt).length)
#define RSX_PKT_GETCOUNT(pkt)           ((pkt).count)
#define RSX_PKT_SET_U8(pkt, count, idx, val) (((uint8_t*)(pkt).data)[count * (pkt).length + idx] = val)
#define RSX_PKT_GET_U8(pkt, count, idx) (((uint8_t*)(pkt).data)[count * (pkt).length + idx])

//#define RSX_LPKT_SETID(pkt, id)         do{pkt.id      = id     ;}while(0)
#define RSX_LPKT_SETID(pkt, count, id)  do{pkt.data[count * pkt.length] = id;}while(0)
#define RSX_LPKT_SETFLAG(pkt, val)      do{pkt.flag    = val    ;}while(0)
#define RSX_LPKT_GETFLAG(pkt)           ((pkt).flag)
#define RSX_LPKT_SETADDR(pkt, val)      do{pkt.address = val    ;}while(0)
#define RSX_LPKT_SETLENGTH(pkt, val)    do{pkt.length  = val + sizeof(uint8_t);}while(0) // TODO:
#define RSX_LPKT_SETCOUNT(pkt, val)     do{pkt.count   = val  ;}while(0)
#define RSX_LPKT_GETLENGTH(pkt)         RSX_PKT_GETLENGTH(pkt)
#define RSX_LPKT_GETCOUNT(pkt)          RSX_PKT_GETCOUNT(pkt)
//#define RSX_LPKT_SET_U8(pkt, count, idx, data) RSX_PKT_SET_U8(pkt, count, idx, data)
#define RSX_LPKT_SET_U8(pkt, count, idx, val)   do{pkt.data[count * pkt.length + (idx + 1)]  = val;}while(0)
#define RSX_LPKT_SET_INT16(pkt, count, idx, val)                \
    do {                                                          \
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
#define RSX_SPKT_SET_U8(pkt, idx, data) RSX_PKT_SET_U8(pkt, 0, idx, data)
#define RSX_SPKT_SET_INT16(pkt, idx, data)                \
    do {                                                    \
        RSX_SPKT_SET_U8(pkt, idx    , (data     ) & 0xFF);\
        RSX_SPKT_SET_U8(pkt, idx + 1, (data >> 8) & 0xFF);\
    } while(0)
#define RSX_SPKT_GET_U8(pkt, idx)       RSX_PKT_GET_U8(pkt, 0, idx)

#define SPKT_FLAG_WRITE_FLASH 0x40 /* 0b 0100 0000 */
#define SPKT_FLAG_RESET_SERVO 0x20 /* 0b 0010 0000 */
#define SPKT_FLAG_MEMMAP_CLR  0x10 /* 0b 0001 0000 */
#define SPKT_FLAG_RTN_PKT_ADDR(addr) (addr & 0x0F /* 0b 0000 1111 */)

typedef struct {
  uint8_t vid;   /* virtual servo id */
  uint8_t *data; /* expect uint8_t data[length in rsx_pkt] */
} rsx_vload;

typedef struct {
  uint16_t    header; /* 0xFAAF for short packet. */
  uint8_t         id; /* 1 to 127 (01H to 7FH)    */
  uint8_t       flag;
  uint8_t    address;
  uint8_t     length;
  uint8_t      count;
  uint8_t      *data; // TODO: void*? /* expect uint8_t data[count][length] */
  uint8_t  check_sum; // TODO:
} rsx_pkt; // RS301/302 Short packet

errno_t rsx_pkt_get_size (rsx_pkt *pkt, size_t *size);
errno_t rsx_pkt_init (rsx_pkt *pkt, uint8_t length, uint8_t count, uint8_t *data);
errno_t rsx_lpkt_init (rsx_pkt *pkt);
errno_t rsx_spkt_init (rsx_pkt *pkt, uint8_t id, uint8_t flag);
errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size);
errno_t rsx_pkt_ser (rsx_pkt *pkt, uint8_t dst[/*max_num*/], size_t max_num, size_t *size);

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

