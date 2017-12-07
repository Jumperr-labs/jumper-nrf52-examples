import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestBma280(unittest.TestCase):
    def setUp(self):
        self.vlab = Vlab(working_directory=dir)
        self.uart = self.vlab.uart
        self.vlab.load(fw_bin)
        self.vlab.run_for_ms(500)
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()

    def test_should_recognize_flat(self):
        self.vlab.BMA280Device.set_interrupt('flat')
        self.vlab.run_for_ms(1000)
        self.assertRegexpMatches(self.uart.read(), "Firmware recognized bma280 interrupt: flat")

if __name__ == '__main__':
    unittest.main()
