
#include "rsx.h"

/* for error print */
#include <stdio.h>

errno_t rsx_pkt_get_size (rsx_pkt *pkt, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, size);

  *size = 0;
  *size += sizeof(pkt->header);
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

errno_t rsx_pkt_init (rsx_pkt *pkt, uint8_t length, uint8_t count, uint8_t *data) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, data);

  pkt->length = length;
  pkt->count  = count;
  pkt->data   = data;

  return EOK;
}

errno_t rsx_lpkt_init (rsx_pkt *pkt) {
  EVALUE(NULL, pkt);

  pkt->header = 0xFAAF;
  pkt->id     = 0x00;
  pkt->flag   = 0x00;

  return EOK;
}

errno_t rsx_spkt_init (rsx_pkt *pkt, uint8_t id, uint8_t flag) {
  EVALUE(NULL, pkt);

  pkt->header = 0xFAAF;
  pkt->id     = id;
  pkt->flag   = flag;

  return EOK;
}

errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, src);
  EVALUE(NULL, size);

  size_t idx = 0;
  pkt->header = (((uint16_t)src[idx + 1]) << 8) | (src[idx]);
  idx += 2;

  pkt->id      = src[idx++];
  pkt->flag    = src[idx++];
  pkt->address = src[idx++];
  pkt->length  = src[idx++];
  pkt->count   = src[idx++];

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

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

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

  size_t idx = 0;
  dst[idx++] = (pkt->header >> 8);
  dst[idx++] = pkt->header;

  dst[idx++] = pkt->id;
  dst[idx++] = pkt->flag;
  dst[idx++] = pkt->address;
  dst[idx++] = pkt->length;

  // TODO: sub-serializer
  // pkt->flag with non-zero indicates that there are no payload.
  if (pkt->flag == 0x00) {
    dst[idx++] = pkt->count;
    for (size_t i = 0; i < pkt->count; i++) {
      for (size_t j = 0; j < pkt->length; j++) {
        dst[idx++] = pkt->data[(i * pkt->length) + j];
      }
    }
  } else {
    dst[idx++] = 0x00;
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= dst[i];
  }

  dst[idx++] = pkt->check_sum;
  *size = idx;

  return EOK;
}

