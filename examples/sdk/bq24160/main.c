
/*
 * (C) Copyright 2012 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */


#include <stdio.h>
#include <stdbool.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"

#define HIGH 1
#define LOW 0
#define DATA_NACK -1
#define OK 1
#define UNSET 0

#define PIN_OUT_CHARGE_USB 6
#define PIN_OUT_CHARGE_IN 5
#define PIN_OUT_DISCHARGE 8
#define PIN_OUT_INT_UP 3
#define PIN_OUT_INT_DOWN 7
#define SCL_PIN 27
#define SDA_PIN 26
#define STAT_PIN 24
#define BQ24160_ADDRESS          (0x6B)

/* instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);

/* Indicates if operation on TWI has ended. */
static volatile char m_xfer_done = UNSET;


#undef FEATURE_BQ24160_DEBUG
#if defined(CONFIG_MACH_SHIRI) || defined(CONFIG_MACH_KOI)
//I2C    -    SDA    -    GPA0_6,    SCL    -    GPA0_7
#define GPA0CON        *(volatile unsigned long *)(0x11400000)
#define GPA0DAT        *(volatile unsigned long *)(0x11400004)
#define GPA0PUD        *(volatile unsigned long *)(0x11400008)
#define IIC0_ESCL_Hi    GPA0DAT |= (0x1<<7)
#define IIC0_ESCL_Lo    GPA0DAT &= ~(0x1<<7)
#define IIC0_ESDA_Hi    GPA0DAT |= (0x1<<6)
#define IIC0_ESDA_Lo    GPA0DAT &= ~(0x1<<6)
#define IIC0_ESCL_INP    GPA0CON &= ~(0xf<<28)
#define IIC0_ESCL_OUTP    GPA0CON = (GPA0CON & ~(0xf<<28))|(0x1<<28)
#define IIC0_ESDA_INP    GPA0CON &= ~(0xf<<24)
#define IIC0_ESDA_OUTP    GPA0CON = (GPA0CON & ~(0xf<<24))|(0x1<<24)
#define GPIO_DAT    GPA0DAT
#define GPIO_DAT_SHIFT    (6)
#define DIS_GPIO_PUD    GPA0PUD &= ~(0xf<<12)
//nPOWER, PMIC_ONOB  XEINT23/GPX2_7
#define GPX2CON    0x11000C40
#define GPX2DAT    0x11000C44
#define GPX2PUD    0x11000C48
#define POWER_KEY_CON    GPX2CON
#define POWER_KEY_DAT    GPX2DAT
#define POWER_KEY_PUD    GPX2PUD
//CHG_INT    XEINT17/GPX2_1
#define BQ24160_CHG_INT_CON    GPX2CON
#define BQ24160_CHG_INT_DAT    GPX2DAT
#define BQ24160_CHG_INT_PUD    GPX2PUD
//CHG_STAT      XEINT11/GPX1_3
#define GPX1CON    0x11000C20
#define GPX1DAT    0x11000C24
#define GPX1PUD    0x11000C28
#define BQ24160_CHG_STAT_CON    GPX1CON
#define BQ24160_CHG_STAT_DAT    GPX1DAT
#define BQ24160_CHG_STAT_PUD    GPX1PUD
#define EXYNOS3250_PS_HOLD_CONTROL 0x1002330C
#define PS_HOLD_CONTROL EXYNOS3250_PS_HOLD_CONTROL
#define PS_HOLD_CONTORL_EN_BIT 9
#else
#endif


// BQ24160 Registers
#define REG_STATUS          0x00
#define REG_BR_STATUS       0x01
#define REG_CONTROL         0x02
#define REG_BR_VOLTAGE      0x03
#define REG_VENDOR          0x04
#define REG_TERMINATION     0x05
#define REG_DPM         0x06
#define REG_NTC         0x07
#define REG_STATUS_TMR_RST_BIT    7
#define REG_STATUS_STAT_MASK    0x70
#define REG_STATUS_SUPPLY_SEL_BIT    3
#define REG_STATUS_FAULT_MASK       0x07
#define REG_CONTROL_RESET_BIT       7
#define REG_CONTROL_IUSB_LIM_MASK       0x70
#define REG_CONTROL_IUSB_LIM_BIT 4
#define REG_CONTROL_EN_STAT_BIT     3
#define REG_CONTROL_TE_BIT          2
#define REG_CONTROL_CE_BIT          1
#define REG_CONTROL_HZ_MODE_BIT     0
#define REG_NTC_2XTMR_EN_BIT        7
#define REG_NTC_TMR_MASK        0x60
#define REG_NTC_TMR_BIT     5
#define REG_NTC_BATGD_EN_BIT        4
#define REG_NTC_TS_EN_BIT           3
#define REG_NTC_TS_FAULT_MASK       0x06
#define REG_NTC_LOW_CHG_BIT         0
#define STAT_NO_VALID_SOURCE    0x00
#define STAT_IN_READY    0x10
#define STAT_USB_READ    0x20
#define STAT_CHARGING_FROM_IN    0x30
#define STAT_CHARGING_FROM_USB 0x40
#define STAT_CHARGING_DONE    0x50
#define STAT_CHARGING_NA    0x60
#define STAT_CHARGING_FAULT    0x70
#define TMR_27MIN 0x0
#define TMR_6H    0x1
#define TMR_9H    0x2
#define TMR_OFF    0x3
#define NORMAL_FAULT 0x0
#define WATCHDOG_EXPIRATION_INT 0x03
#define IIN_LIM_1500MA 0x0
#define IIN_LIM_2500MA 0x1
#define IUSB_LIM_100MA 0x0
#define IUSB_LIM_150MA 0x1
#define IUSB_LIM_500MA 0x2
#define IUSB_LIM_800MA 0x3
#define IUSB_LIM_900MA 0x4
#define IUSB_LIM_1500MA 0x5
#define REG_BR_VOLTAGE_MASK 0xFC
#define REG_BR_VOLTAGE_IIN_LIM_MASK 0x1
#define REG_BR_VOLTAGE_IIN_LIM_BIT 1
#define BATTERY_REGULATION_VOLTAGE_640mV (1<<7)
#define BATTERY_REGULATION_VOLTAGE_320mV (1<<6)
#define BATTERY_REGULATION_VOLTAGE_160mV (1<<5)
#define BATTERY_REGULATION_VOLTAGE_80mV (1<<4)
#define BATTERY_REGULATION_VOLTAGE_40mV (1<<3)
#define BATTERY_REGULATION_VOLTAGE_20mV (1<<2)
#define BQ24160_SET_BIT(bit, val, data) ((val << bit) | ((data) & ~(1 << bit)))
#define BQ24160_SET_BIT_2(bit, val, data) ((val << bit) | ((data) & ~(11 << bit)))
#define BQ24160_SET_BIT_3(bit, val, data) ((val << bit) | ((data) & ~(111 << bit)))
#define DATA_MASK(mask, data) ((data) << (ffs(mask) - 1))
#define SET_MASK(mask, val, data) (((data) & ~(mask)) | (val))

bool interrupt_up = false;
// extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
typedef enum {
    UNPRESS_POWER_KEY = 0,
    PRESS_POWER_KEY
} power_key;
#define DELAY_TIME        100
#define BQ24160_ADDR  0xD6
void BQ24160_Delay(void)
{
    unsigned long i;
    for (i = 0; i < DELAY_TIME; i++)
        ;
}
    


/****************** Global Function Definitions *******************************/

void Bq24160_handler(nrf_drv_twi_evt_t const * p_event, void * p_context) {
	switch (p_event->type) {
		case NRF_DRV_TWI_EVT_DONE:
			m_xfer_done = OK;
			break;
		case NRF_DRV_TWI_EVT_ADDRESS_NACK:
		case NRF_DRV_TWI_EVT_DATA_NACK:
			m_xfer_done = DATA_NACK;
			break;
		default:
			break;
	}
}

void twi_init(void) {
	ret_code_t err_code;
   const nrf_drv_twi_config_t m24c04_config = {
	   .scl                = SCL_PIN,
	   .sda                = SDA_PIN,
	   .frequency          = NRF_TWI_FREQ_100K,
	   .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
	   .clear_bus_init     = false
	};

	err_code = nrf_drv_twi_init(&m_twi, &m24c04_config, Bq24160_handler, NULL);
	APP_ERROR_CHECK(err_code);

	nrf_drv_twi_enable(&m_twi);
}

void WaitForDone()  {
	do {
		__WFE();
	} while (m_xfer_done == UNSET);
}

void BQ24160_IIC0_EWriteByte(unsigned char ChipId, unsigned char IicAddr, unsigned char IicData) {

    unsigned char tx_buff[] = {IicAddr, IicData};
    m_xfer_done = false;
	ret_code_t err_code;
	err_code = nrf_drv_twi_tx(&m_twi, ChipId, tx_buff, 2, false);
	APP_ERROR_CHECK(err_code);
	WaitForDone();
}

void BQ24160_IIC0_EReadByte(unsigned char ChipId, unsigned char IicAddr, unsigned char *IicData) 
{
    unsigned char tx_buff[] = {IicAddr};
    m_xfer_done = false;
	ret_code_t err_code;
	err_code = nrf_drv_twi_tx(&m_twi, ChipId, tx_buff, 1, true);
	APP_ERROR_CHECK(err_code);
    WaitForDone();
    
    m_xfer_done = false;
	err_code = nrf_drv_twi_rx(&m_twi, ChipId, IicData, 1);
	APP_ERROR_CHECK(err_code);
	WaitForDone();
}


void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    unsigned char rx_buff_status[2];
	BQ24160_IIC0_EReadByte(BQ24160_ADDRESS, REG_STATUS, rx_buff_status);
	if (!interrupt_up && (rx_buff_status[0] & REG_STATUS_FAULT_MASK) == NORMAL_FAULT) {
	    if ((rx_buff_status[0] & REG_STATUS_STAT_MASK) == STAT_CHARGING_FROM_USB) {
	        NRF_LOG_INFO("\r\nCharging from usb\r\n");
	        nrf_drv_gpiote_out_toggle(PIN_OUT_CHARGE_USB);
	    } else if ((rx_buff_status[0] & REG_STATUS_STAT_MASK) == STAT_CHARGING_FROM_IN) {
	         NRF_LOG_INFO("\r\nCharging from in\r\n");
	         nrf_drv_gpiote_out_toggle(PIN_OUT_CHARGE_IN);
	    } else if ((rx_buff_status[0] & REG_STATUS_STAT_MASK) == STAT_CHARGING_DONE) {
	           NRF_LOG_INFO("\r\nStopped charging\r\n");
	           nrf_drv_gpiote_out_toggle(PIN_OUT_DISCHARGE);
	    }
	} else if (!interrupt_up && (rx_buff_status[0] & REG_STATUS_STAT_MASK) == STAT_CHARGING_FAULT &&
	    (rx_buff_status[0] & REG_STATUS_FAULT_MASK) == WATCHDOG_EXPIRATION_INT) {
             NRF_LOG_INFO("\r\nWatchdog int up\r\n");
            interrupt_up = true;
            nrf_drv_gpiote_out_toggle(PIN_OUT_INT_UP);
    } else if (interrupt_up) {
             NRF_LOG_INFO("\r\nWatchdog int down\r\n");

            interrupt_up = false;
            nrf_drv_gpiote_out_toggle(PIN_OUT_INT_DOWN);
    }
}

int main() {
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    twi_init();

    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT_CHARGE_USB, &out_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT_CHARGE_IN, &out_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT_DISCHARGE, &out_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT_INT_UP, &out_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT_INT_DOWN, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(STAT_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(STAT_PIN, true);
	
    while (true)
    {
        __WFE();
        NRF_LOG_FLUSH();
    }
}
     
 
