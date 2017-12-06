#include "ProductionCode.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */


int myfunc()
{
    return 0;
}

void main_spi_init() {
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
}

void send_over_spi(uint8_t *tx_buffer, uint8_t tx_buffer_length, uint8_t *rx_buffer, uint8_t rx_buffer_length) {
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_buffer, tx_buffer_length, rx_buffer, rx_buffer_length));
}