#include <main.h>


#define AUTOEND true    //true = stop i2c link at the end of i2c_xfer7()
#define I2C_OLED I2C1


void write_update_oled(char*);
void i2c_xfer7(uint32_t i2c, uint8_t addr, uint8_t command, uint8_t *w, size_t wn, bool autoend);
void oled_setup();

