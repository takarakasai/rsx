
#include "rsx_pkt.h"

/* for error print */
#include <stdio.h>

#include "serial/hr_serial.h"

#define RSX_MAGIC_NUMBER 0xFAAF

static errno_t get_header_footer_size (rsx_pkt *pkt, uint8_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  *size = 0;
  *size += sizeof(pkt->header);
  *size += sizeof(pkt->id);
  *size += sizeof(pkt->flag);
  *size += sizeof(pkt->address);
  *size += sizeof(pkt->length);
  *size += sizeof(pkt->count);

  // pkt->flag with non-zero indicates that there are no payload.
  //if (pkt->flag == 0x00) {
  //  *size += pkt->count * pkt->length;
  //}

  *size += sizeof(pkt->check_sum);

  return EOK;
}

errno_t rsx_pkt_get_snd_size (rsx_pkt *pkt, uint8_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  ECALL(get_header_footer_size(pkt, size));
  if (pkt->flag == (RSX_SPKT_SND_FLG_DEFAULT | RSX_SPKT_RTN_FLG_RTNLESS)) {
    *size += pkt->count * pkt->length;
  }

  return EOK;
}

errno_t rsx_pkt_get_rtn_payload_size (rsx_pkt *pkt, uint8_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  switch (pkt->flag) {
    case RSX_SPKT_RTN_FLG_ACKNACK:
        /* programming error */
        return -1;
        break;
    case RSX_SPKT_RTN_FLG_00TO029: /* size:30 */
    case RSX_SPKT_RTN_FLG_30TO059: /* size:30 */
        *size += 30;
        break;
    case RSX_SPKT_RTN_FLG_20TO029: /* size:10 */
        *size += 10;
        break;
    case RSX_SPKT_RTN_FLG_42TO059: /* size:18 */
        *size += 18;
        break;
    case RSX_SPKT_RTN_FLG_30TO041: /* size:12 */
        *size += 12;
        break;
    case RSX_SPKT_RTN_FLG_60TO127: /* size:68 */
        *size += 68;
        break;
    case RSX_SPKT_RTN_FLG_ADDR:    /* size:pkt->length */
        *size += pkt->length;
        break;
    default:
        /* programming error */
        return -1;
        break;
  }

  return EOK;
}

errno_t rsx_pkt_get_rtn_size (rsx_pkt *pkt, uint8_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  if (pkt->flag == RSX_SPKT_RTN_FLG_ACKNACK) {
    *size = 1;
  } else {
    uint8_t payload_size;
    ECALL(get_header_footer_size(pkt, size));
    ECALL(rsx_pkt_get_rtn_payload_size(pkt, &payload_size));
    *size += payload_size;
  }

  return EOK;
}

/* to be obsoluted */
errno_t rsx_pkt_get_size (rsx_pkt *pkt, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  *size = 0;
  *size += sizeof(pkt->header);
  *size += sizeof(pkt->id);
  *size += sizeof(pkt->flag);
  *size += sizeof(pkt->address);
  *size += sizeof(pkt->length);
  *size += sizeof(pkt->count);

  // pkt->flag with non-zero indicates that there are no payload.
  if (pkt->flag == 0x00) {
    *size += pkt->count * pkt->length;
  }

  *size += sizeof(pkt->check_sum);

  return EOK;
}

errno_t rsx_pkt_init (rsx_pkt *pkt, uint8_t length, uint8_t count, void *data) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, data);

  pkt->header = RSX_MAGIC_NUMBER;
  pkt->id     = 0x00;
  pkt->flag   = 0x00;

  pkt->length = length;
  pkt->count  = count;
  pkt->data   = data;

  return EOK;
}

errno_t rsx_pkt_reset (rsx_pkt *pkt) {
  EVALUE(NULL, pkt);

  pkt->header = 0xFAAF;
  pkt->id     = 0x00;
  pkt->flag   = 0x00;

  return EOK;
}

errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, src);
  EVALUE(NULL, size);

  uint8_t pkt_size;
  //ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ECALL(rsx_pkt_get_rtn_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

  size_t idx = 0;
  pkt->header = (((uint16_t)src[idx + 1]) << 8) | (src[idx]);
  idx += 2;

  pkt->id      = src[idx++];
  pkt->flag    = src[idx++];
  pkt->address = src[idx++];
  pkt->length  = src[idx++];
  pkt->count   = src[idx++];

  // TODO: sub-serializer
  if (pkt->flag == 0x00 || pkt->flag == 0x04) {
    for (size_t i = 0; i < pkt->count; i++) {
      for (size_t j = 0; j < pkt->length; j++) {
        pkt->data[(i * pkt->length) + j] = src[idx++];
      }
    }
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= src[i];
  }

  //src[idx] = pkt->check_sum;
  //printf("recv pkt check sum: %02x vx %02x\n", pkt->check_sum, src[idx]);
  idx++;
  *size = idx;

  return EOK;
}

errno_t rsx_pkt_ser (rsx_pkt *pkt, uint8_t dst[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, dst);
  EVALUE(NULL, size);

  uint8_t pkt_size;
  //ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ECALL(rsx_pkt_get_snd_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

  size_t idx = 0;
  dst[idx++] = (pkt->header >> 8);
  dst[idx++] = pkt->header;

  dst[idx++] = pkt->id;
  dst[idx++] = pkt->flag;

  // TODO: sub-serializer
  // pkt->flag with non-zero indicates that there are no payload.
  // 0x00 ==> write packet
  // 0x40 ==> write flash rom    | ADDR:0xFF LENGTH:0x00 COUNT:0x00
  // 0x20 ==> restart servo      | ADDR:0xFF LENGTH:0x00 COUNT:0x00
  // 0x10 ==> store default mmap | ADDR:0xFF LENGTH:0xFF COUNT:0x00 DATA:0xFF?
  if (pkt->flag == (RSX_SPKT_SND_FLG_DEFAULT | RSX_SPKT_RTN_FLG_RTNLESS)) {
    /* --- FLAG 0x00 ---*/
    /* address */
    dst[idx++] = pkt->address;
    /* length  */
    dst[idx++] = pkt->length;
    /* count   */
    dst[idx++] = pkt->count;
    /* data    */
    for (size_t i = 0; i < pkt->count; i++) {
      for (size_t j = 0; j < pkt->length; j++) {
        dst[idx++] = pkt->data[(i * pkt->length) + j];
      }
    }
  } else if ((pkt->flag & RSX_SPKT_SND_FLG_MASK) && (pkt->flag & RSX_SPKT_RTN_FLG_MASK)) {
    /* --- FLAG 0x0* and 0x*0 ---*/
    /* programming error */
    return -1;
  } else if (pkt->flag & RSX_SPKT_RTN_FLG_MASK) {
    /* --- FLAG 0x0* ---*/
    switch (pkt->flag) {
      case RSX_SPKT_RTN_FLG_ACKNACK:
      case RSX_SPKT_RTN_FLG_00TO029: /* size:30 */
      case RSX_SPKT_RTN_FLG_30TO059: /* size:30 */
      case RSX_SPKT_RTN_FLG_20TO029: /* size:10 */
      case RSX_SPKT_RTN_FLG_42TO059: /* size:18 */
      case RSX_SPKT_RTN_FLG_30TO041: /* size:12 */
      case RSX_SPKT_RTN_FLG_60TO127: /* size:68 */
          /* address */
          dst[idx++] = pkt->address;
          /* length  */
          dst[idx++] = pkt->length;
          /* count */
          dst[idx++] = 0x00;
          /* data    */
          for (size_t i = 0; i < pkt->count; i++) {
            for (size_t j = 0; j < pkt->length; j++) {
              dst[idx++] = pkt->data[(i * pkt->length) + j];
            }
          }
          break;
      case RSX_SPKT_RTN_FLG_ADDR:    /* size:pkt->length */
          /* address */
          dst[idx++] = pkt->address;
          /* length  */
          dst[idx++] = pkt->length;
          /* count */
          dst[idx++] = 0x00;
          break;
      default:
          /* programming error */
          return -1;
          break;
    }
  } else if (pkt->flag & RSX_SPKT_SND_FLG_MASK) {
    /* --- FLAG 0x*0 ---*/
    switch (pkt->flag) {
      case RSX_SPKT_SND_FLG_WRFLASH:
      case RSX_SPKT_SND_FLG_RESTART:
          /* address */
          dst[idx++] = 0xFF;
          /* length  */
          dst[idx++] = 0x00;
          /* count */
          dst[idx++] = 0x00;
          break;
      case RSX_SPKT_SND_FLG_RST_MEM:
          /* address */
          dst[idx++] = 0xFF;
          /* length  */
          dst[idx++] = 0xFF;
          /* count */
          dst[idx++] = 0x00;
          /* data? */
          /* dst[idx++] = 0xFF; */
          break;
      default:
          /* programming error */
          return -1;
          break;
    }
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= dst[i];
  }

  dst[idx++] = pkt->check_sum;
  *size = idx;

  return EOK;
}

