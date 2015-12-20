
#ifndef RSX_H
#define RSX_H

/* interface */
#include "dpservo.h"

#include "rsx_type.h"

#include "rsx_pkt.h"
// for hr_serial
#include "serial/hr_serial.h"

#define kRSX_LPKT_PAYLOAD_SIZE 20
#define kRSX_SPKT_PAYLOAD_SIZE 16

#define RSX_DECL(name, num_of_servo)                     \
    RSX_LPKT_DECL(name ## _lpkt, num_of_servo, kRSX_LPKT_PAYLOAD_SIZE); \
    RSX_SPKT_DECL(name ## _spkt, kRSX_SPKT_PAYLOAD_SIZE);               \
    rsx name

#define RSX_INIT(name)              \
    RSX_LPKT_INIT(name ## _lpkt);   \
    RSX_SPKT_INIT(name ## _spkt);   \
    rsx_init(&name, &(name ## _lpkt), &(name ## _spkt))

typedef struct {
  dpservo_base base; /* inherit dpservo */

  size_t retry_count;

  rsx_pkt *lpkt;
  rsx_pkt *spkt;
} rsx;

errno_t rsx_init (rsx *rsx, rsx_pkt *lpkt, rsx_pkt *spkt);
errno_t rsx_open (rsx *x, const char8_t *device, const char8_t *port, hr_baudrate baudrate, hr_parity parity);
errno_t rsx_close (rsx *x);
errno_t rsx_spkt_write (rsx *x);
errno_t rsx_spkt_write_read (rsx *x);
errno_t rsx_lpkt_write (rsx *x);

errno_t rsx_spkt_mem_write (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]);
errno_t rsx_spkt_mem_write_int16 (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]);
errno_t rsx_spkt_mem_read (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]);
errno_t rsx_spkt_mem_read_int16 (rsx *rsx, uint8_t id, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]);
errno_t rsx_lpkt_mem_write (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, const uint8_t size, uint8_t **data/*[num][size]*/);
errno_t rsx_lpkt_mem_write_int16 (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t **data/*[num][size]*/);
errno_t rsx_lpkt_mem_write_all (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, uint8_t data[/*size*/]);
errno_t rsx_lpkt_mem_write_int16_all (rsx *rsx, uint8_t id[/*num*/], uint8_t num, uint8_t start_addr, uint8_t size, int16_t data[/*size*/]);

errno_t rsx_set_serial (rsx *x, bool use_serial);

#endif
 
