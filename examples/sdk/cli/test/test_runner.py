import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestCLI(unittest.TestCase):
    def setUp(self):
        self.vlab = Vlab(working_directory=dir, print_uart=True)
        self.vlab.load(fw_bin)
        self.uart = self.vlab.uart
        self.vlab.start()
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()

    def test_cli(self):
        self.vlab.uart.write(b'log\r\n')
        self.vlab.uart.wait_until_uart_receives(b'status   :Logger status', 500000000)
        self.vlab.uart.write(b'python\r\n')
        self.vlab.uart.wait_until_uart_receives(b'Nice joke ;)', 500000000)
        self.vlab.uart.write(b'history\r\n')
        self.vlab.uart.wait_until_uart_receives(b'[  2] history', 500000000)


if __name__ == '__main__':
    unittest.main()
