#include "ProductionCode.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */


int myfunc()
{
    return 0;
}

#define spi_config_slave_0 \
{ \
    .sck_pin      = SPI_SCK_PIN, \
    .mosi_pin     = SPI_MOSI_PIN, \
    .miso_pin     = SPI_MISO_PIN, \
    .ss_pin       = 27, \
    .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY, \
    .orc          = 0xFF, \
    .frequency    = NRF_DRV_SPI_FREQ_1M, \
    .mode         = NRF_DRV_SPI_MODE_0, \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST \
}

#define spi_config_slave_1 \
{ \
    .sck_pin      = SPI_SCK_PIN, \
    .mosi_pin     = SPI_MOSI_PIN, \
    .miso_pin     = SPI_MISO_PIN, \
    .ss_pin       = 28, \
    .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY, \
    .orc          = 0xFF, \
    .frequency    = NRF_DRV_SPI_FREQ_1M, \
    .mode         = NRF_DRV_SPI_MODE_1, \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST \
}

// This is wrong! Slave #2 is using SPI mode 3, not 0. check out peripherals.json
#define spi_config_slave_2 \
{ \
    .sck_pin      = SPI_SCK_PIN, \
    .mosi_pin     = SPI_MOSI_PIN, \
    .miso_pin     = SPI_MISO_PIN, \
    .ss_pin       = 29, \
    .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY, \
    .orc          = 0xFF, \
    .frequency    = NRF_DRV_SPI_FREQ_1M, \
    .mode         = NRF_DRV_SPI_MODE_0, \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST \
}

// This is wrong! The maximum frequency supported by slave #3 is 1MHz. check out peripherals.json
#define spi_config_slave_3 \
{ \
    .sck_pin      = SPI_SCK_PIN, \
    .mosi_pin     = SPI_MOSI_PIN, \
    .miso_pin     = SPI_MISO_PIN, \
    .ss_pin       = 28, \
    .irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY, \
    .orc          = 0xFF, \
    .frequency    = NRF_DRV_SPI_FREQ_4M, \
    .mode         = NRF_DRV_SPI_MODE_3, \
    .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST \
}

nrf_drv_spi_config_t spi_slaves_configs[4] = {spi_config_slave_0, spi_config_slave_1, spi_config_slave_2, spi_config_slave_3};

void spi_init_slave(int slave_number) {
    nrf_drv_spi_uninit(&spi);
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &(spi_slaves_configs[slave_number]), NULL, NULL));
}

void send_over_spi(int slave_number, uint8_t *tx_buffer, uint8_t tx_buffer_length, uint8_t *rx_buffer, uint8_t rx_buffer_length) {
    spi_init_slave(slave_number);
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx_buffer, tx_buffer_length, rx_buffer, rx_buffer_length));
}
