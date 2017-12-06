#include "ProductionCode.h"
#include "unity.h"

#define ECHO_STRING "HELLO"

/* sometimes you may want to get at local data in a module.
 * for example: If you plan to pass by reference, this could be useful
 * however, it should often be avoided */

void setUp(void)
{
    /* This is run before EACH TEST */
    main_spi_init();
}

void tearDown(void)
{
}

void test_1(void)
{
    TEST_ASSERT_EQUAL(myfunc(), 0);
}

void test_spi_sanity_echo_slave(void)
{
    uint8_t rx_buffer [sizeof(ECHO_STRING)];
    uint8_t tx_buffer[] = ECHO_STRING;
    send_over_spi(tx_buffer, sizeof(ECHO_STRING), rx_buffer, sizeof(ECHO_STRING));
    TEST_ASSERT_EQUAL_MEMORY(ECHO_STRING, rx_buffer, sizeof(ECHO_STRING));
}
