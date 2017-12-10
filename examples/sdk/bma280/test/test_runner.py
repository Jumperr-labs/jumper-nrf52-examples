import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestBma280(unittest.TestCase):
    def setUp(self):
        self.vlab = Vlab(working_directory=dir, print_uart=True)
        self.vlab.load(fw_bin)
        self.uart = self.vlab.uart
        # self.vlab.run_for_ms(500)
        self.vlab.start()
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()

    def test_should_recognize_low_g(self):
        self.vlab.BMA280Device.set_interrupt('low_g')
        self.vlab.run_for_ms(1000)
        self.assertRegexpMatches(self.uart.read(), "Firmware recognized bma280 interrupt: low_g")

    def test_should_recognize_orient_xy(self):
        self.vlab.BMA280Device.set_interrupt('orient_xy')
        self.vlab.run_for_ms(1000)
        self.assertRegexpMatches(self.uart.read(), "Firmware recognized bma280 interrupt: orient_xy")

    def test_should_recognize_slope_x(self):
        self.vlab.BMA280Device.set_interrupt('slope_x')
        self.vlab.run_for_ms(1000)
        self.assertRegexpMatches(self.uart.read(), "Firmware recognized bma280 interrupt: slope_x")

if __name__ == '__main__':
    unittest.main()
