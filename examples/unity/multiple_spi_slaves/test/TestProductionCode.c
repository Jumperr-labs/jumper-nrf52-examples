#include "ProductionCode.h"
#include "unity.h"

#define ECHO_STRING "HELLO"

/* sometimes you may want to get at local data in a module.
 * for example: If you plan to pass by reference, this could be useful
 * however, it should often be avoided */

void setUp(void)
{
    /* This is run before EACH TEST */
}

void tearDown(void)
{
}

uint8_t tx_buffer[] = ECHO_STRING;

void test_spi_echo_slave_0(void)
{
    uint8_t rx_buffer [sizeof(ECHO_STRING)];
    send_over_spi(0, tx_buffer, sizeof(ECHO_STRING), rx_buffer, sizeof(ECHO_STRING));
    TEST_ASSERT_EQUAL_MEMORY(ECHO_STRING, rx_buffer, sizeof(ECHO_STRING));
}

void test_spi_echo_slave_1(void)
{
    uint8_t rx_buffer [sizeof(ECHO_STRING)];
    send_over_spi(1, tx_buffer, sizeof(ECHO_STRING), rx_buffer, sizeof(ECHO_STRING));
    TEST_ASSERT_EQUAL_MEMORY(ECHO_STRING, rx_buffer, sizeof(ECHO_STRING));
}

void test_spi_echo_slave_2_THIS_FAILS_BECAUSE_WE_ARE_USING_A_WRONG_SPI_MODE(void)
{
    uint8_t rx_buffer [sizeof(ECHO_STRING)];
    send_over_spi(3, tx_buffer, sizeof(ECHO_STRING), rx_buffer, sizeof(ECHO_STRING));
    TEST_ASSERT_EQUAL_MEMORY(ECHO_STRING, rx_buffer, sizeof(ECHO_STRING));
}

void test_spi_echo_slave_3_THIS_FAILS_BECAUSE_THE_FREQUENCY_IS_TOO_HIGH(void)
{
    uint8_t rx_buffer [sizeof(ECHO_STRING)];
    send_over_spi(4, tx_buffer, sizeof(ECHO_STRING), rx_buffer, sizeof(ECHO_STRING));
    TEST_ASSERT_EQUAL_MEMORY(ECHO_STRING, rx_buffer, sizeof(ECHO_STRING));
}
