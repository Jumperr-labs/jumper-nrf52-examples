#ifndef PRODUCTION_CODE
#define PRODUCTION_CODE

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

void main_spi_init();

void send_over_spi(int slave_number, uint8_t *tx_buffer, uint8_t tx_buffer_length, uint8_t *rx_buffer, uint8_t rx_buffer_length);

int myfunc();

#endif //PRODUCTION_CODE
