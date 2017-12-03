import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestAppButton(unittest.TestCase):
    def setUp(self):
        self.vlab = Vlab(working_directory=dir)
        self.vlab.load(fw_bin)
        self.vlab.run_for_ms(500)
        self.vlab.on_pin_level_event(self.pins_listener)
        print('Virtual device is running')
        self.success = False
        self.need_to_be_on = False
        self.pin_number = 25

    def pins_listener(self, pin_number, pin_level):
        if self.need_to_be_on:
            if self.pin_number == pin_number and pin_level == 1:
                self.success = True
        else:
            if self.pin_number == pin_number and pin_level == 0:
                self.success = True

    def tearDown(self):
        self.vlab.stop()

    def test_should_turn_off_int(self):
        self.pin_number = 25
        self.need_to_be_on = False
        self.vlab.bq24160.charge()
        self.vlab.run_for_ms(1)
        self.assertTrue(self.success)

    def test_should_turn_off_sata(self):
        self.pin_number = 24
        self.need_to_be_on = False
        self.vlab.bq24160.charge()
        self.vlab.run_for_ms(1)
        self.assertTrue(self.success)

    def test_should_turn_on_int(self):
        self.pin_number = 25
        self.need_to_be_on = True
        self.vlab.bq24160.discharge()
        self.vlab.run_for_ms(1)
        self.assertTrue(self.success)

    def test_should_turn_on_sata(self):
        self.pin_number = 24
        self.need_to_be_on = True
        self.vlab.bq24160.discharge()
        self.vlab.run_for_ms(1)
        self.assertTrue(self.success)

    def test_should_toggle_int(self):
        self.vlab.bq24160.charge()
        self.pin_number = 25
        self.need_to_be_on = True
        self.vlab.bq24160.interrupt(self.vlab.bq24160.TIMER_FAULT_INTERRUPT)
        self.vlab.run_for_ms(0.10)
        self.assertTrue(self.success)
        self.success = False
        self.need_to_be_on = False
        self.vlab.run_for_ms(0.130)
        self.assertTrue(self.success)
        self.need_to_be_on = True
        self.vlab.run_for_ms(0.130)
        self.assertTrue(self.success)


if __name__ == '__main__':
    unittest.main()