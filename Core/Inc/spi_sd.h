#include <main.h>

#include <main.h>
#include "ff.h"

#define mount_SD(arg) f_mount(arg, "", 1)
#define unmount_SD() f_mount(0, "", 1)

void close_spi(uint32_t gpio, uint16_t pin);
void open_spi(uint32_t gpio, uint16_t pin);