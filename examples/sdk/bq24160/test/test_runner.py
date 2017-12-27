import os
import unittest
from jumper.vlab import Vlab
from time import sleep

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')

class TestBq24160(unittest.TestCase):
    def setUp(self):
        self.vlab = Vlab(working_directory=dir)
        self.vlab.load(fw_bin)
        self.uart = self.vlab.uart
        self.vlab.run_for_ms(500)
        self.vlab.on_pin_level_event(self.pins_listener)
        print('Virtual device is running')
        self.success = False
        self.need_to_be_on = False
        self.pin_number_should_toggle = 6

    def pins_listener(self, pin_number, pin_level):
        if self.pin_number_should_toggle == pin_number:
            self.success = True

    def tearDown(self):
        self.vlab.stop()

    def test_should_raise_interrupt_bq24160(self):
        print ("raise watchdog interrupt")
        self.pin_number_should_toggle = 3
        self.success = False
        self.vlab.bq24160.interrupt(self.vlab.bq24160.WATCHDOG_EXPIRATION_INTERRUPT)
        self.vlab.run_for_ms(0.5)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Watchdog int up")
        self.pin_number_should_toggle = 7
        self.success = False
        self.vlab.run_for_ms(1.2)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Watchdog int down")

    def test_should_charge_discharge_bq24160(self):
        print ("charge usb")
        self.pin_number_should_toggle = 6
        self.success = False
        self.vlab.bq24160.charge("usb")
        self.vlab.run_for_ms(500)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Charging from usb")
        self.pin_number_should_toggle = 8
        self.success = False
        print ("discharge")
        self.vlab.bq24160.discharge()
        self.vlab.run_for_ms(500)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Stopped charging")
        self.pin_number_should_toggle = 5
        self.success = False
        print ("charge in")
        self.success = False
        self.vlab.bq24160.charge("in")
        self.vlab.run_for_ms(500)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Charging from in")
        self.pin_number_should_toggle = 8
        self.success = False
        print ("discharge")
        self.vlab.bq24160.discharge()
        self.vlab.run_for_ms(500)
        self.assertTrue(self.success)
        self.assertRegexpMatches(self.uart.read(), "Stopped charging")


if __name__ == '__main__':
    unittest.main()
