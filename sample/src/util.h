#ifndef UTIL_H_
#define UTIL_H_

// #define DATA_DUMP
#include "rsx/rsx.h"

errno_t dump (rsx* rsx);

errno_t search_servo (rsx* rsx, hr_serial* hrs, uint8_t* id);

errno_t get_current_status_all (rsx* rsx, hr_serial* hrs, uint8_t* id, uint8_t num);
errno_t get_current_status (rsx* rsx, hr_serial* hrs, uint8_t id);

int GetDeviceInfo(rsx* rsx, hr_serial* hrs, uint8_t id);

int GetMemInfo(rsx* rsx, hr_serial* hrs, uint8_t id, uint8_t addr, uint8_t size);

int GetROMInfo(rsx* rsx, hr_serial* hrs, uint8_t id);

int GetRAMInfo(rsx* rsx, hr_serial* hrs, uint8_t id);

#endif  // UTIL_H_
