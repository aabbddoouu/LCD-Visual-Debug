#include <spi_sd.h>

void open_spi(uint32_t gpio, uint16_t pin){
	gpio_clear(gpio, pin);
}
void close_spi(uint32_t gpio, uint16_t pin){
	gpio_set(gpio, pin);
}