/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
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
#include "jumper.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t res_command[4] = {0xAB, 0x00, 0x00, 0x00};
static uint8_t rems_command_add0[4] = {0x90, 0x00, 0x00, 0x00};
static uint8_t rems_command_add1[4] = {0x90, 0x00, 0x00, 0x01};
static uint8_t rdid_command[1] = {0x9f};


struct __attribute__((__packed__)) rdid {
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t memory_density;
};

struct __attribute__((__packed__)) rems_add0 {
    uint8_t manufacturer_id;
    uint8_t memory_density;
};

struct __attribute__((__packed__)) rems_add1 {
    uint8_t memory_density;
    uint8_t manufacturer_id;
};


/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
}

void MX25R6435F_command(uint8_t *command_buffer, uint8_t command_length, uint8_t *rx_buffer, uint8_t rx_buffer_length) {
    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, command_buffer, command_length, NULL, 0));
    while (!spi_xfer_done)
    {
        __WFE();
    }

    spi_xfer_done = false;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, NULL, 0, rx_buffer,  rx_buffer_length));
    while (!spi_xfer_done)
    {
        __WFE();
    }
}

void send_rdid_command(struct rdid *rdid_response) {
    MX25R6435F_command(rdid_command, sizeof(rdid_command), (uint8_t*) rdid_response, sizeof(struct rdid));
}

void send_rems_command_add0(struct rems_add0 *rems_response) {
    MX25R6435F_command(rems_command_add0, sizeof(rems_command_add0), (uint8_t*) rems_response, sizeof(struct rems_add0));
}

void send_rems_command_add1(struct rems_add1 *rems_response) {
    MX25R6435F_command(rems_command_add1, sizeof(rems_command_add1), (uint8_t*) rems_response, sizeof(struct rems_add1));
}

void send_res_command(uint8_t *res_response) {
    MX25R6435F_command(res_command, sizeof(res_command), res_response, sizeof(*res_response));
}

int main(void)
{
    bsp_board_leds_init();

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    
    NRF_LOG_INFO("SPI example.");
    NRF_LOG_INFO("-----------------------------------------------------------------");
    NRF_LOG_FLUSH();

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    NRF_LOG_INFO("RDID Command");
    struct rdid device_rdid;
    send_rdid_command(&device_rdid);
    NRF_LOG_INFO("manufacturer id: 0x%x", device_rdid.manufacturer_id);
    NRF_LOG_INFO("memory_type: 0x%x", device_rdid.memory_type);
    NRF_LOG_INFO("memory_density: 0x%x", device_rdid.memory_density);

    NRF_LOG_INFO("-----------------------------------------------------------------");
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("REMS Command (ADD=0)");
    struct rems_add0 device_rems_add0;
    send_rems_command_add0(&device_rems_add0);
    NRF_LOG_INFO("manufacturer id: 0x%x", device_rems_add0.manufacturer_id);
    NRF_LOG_INFO("memory_density: 0x%x", device_rems_add0.memory_density);

    NRF_LOG_INFO("-----------------------------------------------------------------");
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("REMS Command (ADD=1)");
    struct rems_add1 device_rems_add1;
    send_rems_command_add1(&device_rems_add1);
    NRF_LOG_INFO("manufacturer id: 0x%x", device_rems_add1.manufacturer_id);
    NRF_LOG_INFO("memory_density: 0x%x", device_rems_add1.memory_density);

    NRF_LOG_INFO("-----------------------------------------------------------------");
    NRF_LOG_FLUSH();

    NRF_LOG_INFO("RES Command");
    uint8_t res;
    send_res_command(&res);
    NRF_LOG_INFO("manufacturer id: 0x%x", res);
    NRF_LOG_INFO("-----------------------------------------------------------------");
    NRF_LOG_FLUSH();

    jumper_sudo_exit_with_exit_code(0);
}
