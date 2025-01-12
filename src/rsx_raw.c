#include "rsx/rsx_raw.h"

/* for error print */
#include <stdio.h>

#include <assert.h>
#include <stdbool.h>

#include "rsx/rsx_common.h"

errno_t rsx_pkt_get_size (const rsx_pkt *pkt, size_t *size) {
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

/* count : num of servos */
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

  pkt->count  = 1;

  return EOK;
}

errno_t rsx_spkt_conv_read_cmd (rsx_pkt *pkt) {
  EVALUE(NULL, pkt);

  assert(pkt->header == 0xFAAF);
  pkt->flag  = 0x0F;
  assert(pkt->length);
  pkt->count = 0x00;

  return EOK;
}

errno_t rsx_pkt_deser (rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num, size_t *size) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, src);
  EVALUE(NULL, size);

  size_t idx = 0;
  pkt->header = (((uint16_t)src[idx + 1]) << 8) | (src[idx]);
  idx += 2;

  // pkt->id      = src[idx++];
  if (pkt->id != src[idx]) {
    RSX_DEBUG_EPRINT("pkt mismatch(ID): %02x:%02x (%zd)\n", pkt->address, src[idx], idx);
    return EILSEQ;
    // assert(false);
  }
  idx++;

  pkt->flag    = src[idx++];

  // pkt->address = src[idx++];
  if (pkt->address != src[idx]) {
    RSX_DEBUG_EPRINT("pkt mismatch(ADDR): %02x:%02x (%zd)\n", pkt->address, src[idx], idx);
    return EILSEQ;
    // assert(false);
  }
  idx++;

  // pkt->length  = src[idx++];
  if (pkt->length != src[idx]) {
    RSX_DEBUG_EPRINT("pkt mismatch(LEN): %02x:%02x (%zd)\n", pkt->address, src[idx], idx);
    return EILSEQ;
  }
  idx++;

  // pkt->count   = src[idx++];
  if (pkt->count != src[idx]) {
    RSX_DEBUG_EPRINT("pkt mismatch(CNT): %02x:%02x (%zd)\n", pkt->address, src[idx], idx);
    return EILSEQ;
  }
  idx++;

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

   // TODO: sub-serializer
  for (size_t i = 0; i < pkt->count; i++) {
    for (size_t j = 0; j < pkt->length; j++) {
      pkt->data[(i * pkt->length) + j] = src[idx++];
    }
  }

  pkt->check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    pkt->check_sum ^= src[i];
  }

  if (pkt->check_sum != src[idx]) {
    RSX_DEBUG_EPRINT("check sum error: %02x vs %02x\n", pkt->check_sum, src[idx]);
    for (size_t i = 0; i < idx; i++) {
      RSX_DEBUG_EPRINT(" %02x", src[i]);
    }
    RSX_DEBUG_EPRINT("\n");
    RSX_DEBUG_EPRINT("============================\n");
    // assert(false);
    return EILSEQ;
  }
  idx++;
  *size = idx;

  return EOK;
}

errno_t rsx_pkt_deser_and_check (  //
    const rsx_pkt *pkt, uint8_t src[/*max_num*/], size_t max_num) {
  EVALUE(NULL, pkt);
  EVALUE(NULL, src);

  size_t idx = 0;
  uint16_t header = (((uint16_t)src[idx + 1]) << 8) | (src[idx]);
  if (pkt->header != 0xFAAF) {
    RSX_DEBUG_EPRINT("pkt illegal(delim): %04x (%zd)\n", pkt->header, idx);
    return EINVAL;
  }
  if (header != 0xDFFD) {
    RSX_DEBUG_EPRINT("pkt mismatch(delim): %04x (%zd)\n", header, idx);
    return EILSEQ;
  }
  idx += 2;

  if (pkt->id != src[idx]) {
    RSX_DEBUG_EPRINT("pkt mismatch(ID): %02x:%02x (%zd)\n", pkt->id, src[idx], idx);
    return EILSEQ;
  }
  idx++;

  // skip flag
  if (pkt->flag != 0x0F) {
    RSX_DEBUG_EPRINT("pkt illegal(flag): %02x:%02x (%zd)\n", pkt->flag, src[idx], idx);
    return EINVAL;
  }
  idx++;

  uint8_t address = src[idx++];
  if (pkt->address != address) {
    RSX_DEBUG_EPRINT("pkt mismatch(addr): %02x:%02x (%zd)\n", pkt->address, address, idx);
    return EILSEQ;
  }

  uint8_t length = src[idx++];
  if (pkt->length != length) {
    RSX_DEBUG_EPRINT("pkt mismatch(length): %02x:%02x (%zd)\n", pkt->length, length, idx);
    return EILSEQ;
  }

  uint8_t count = src[idx++];
  if (pkt->count != 0) {
    RSX_DEBUG_EPRINT("pkt illegal(count): %02x:%02x (%zd)\n", pkt->count, count, idx);
    return EINVAL;
  }
  if (count != 1) {
    RSX_DEBUG_EPRINT("pkt mismatch(count): %02x:%02x (%zd)\n", pkt->count, count, idx);
    return EILSEQ;
  }

  size_t pkt_size;
  ECALL(rsx_pkt_get_size(pkt, &pkt_size));
  ELTGT(0, max_num, pkt_size);

  // TODO: sub-serializer
  for (size_t i = 0; i < count; i++) {
    for (size_t j = 0; j < length; j++) {
      if (pkt->data[(i * length) + j] != src[idx++]) {
        return EILSEQ;
      }
    }
  }

  uint8_t check_sum = 0x00;
  for (size_t i = 2/*from ID*/; i < idx; i++) {
    check_sum ^= src[i];
  }

  if (check_sum != src[idx]) {
    RSX_DEBUG_EPRINT("check sum error: %02x vs %02x\n", check_sum, src[idx]);
    for (size_t i = 0; i < idx; i++) {
      RSX_DEBUG_EPRINT(" %02x", src[i]);
    }
    RSX_DEBUG_EPRINT("\n");
  }
  idx++;

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

