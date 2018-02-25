import os
import unittest
from jumper import Vlab, Interrupts

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestWatchDogTimer(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        self.vlab = Vlab(working_directory=dir, print_uart=True)
        self.vlab.load(fw_bin)
        print('Virtual device is running')
        self.vlab.run_for_ms(1)
        self.vlab.on_interrupt(self.on_interrupt)

    def tearDown(self):
        self.vlab.stop()

    def on_interrupt(self, interrupt):
        if interrupt == Interrupts.WDT:
            self.found_interrupt = True

    def test_wdt(self):
        self.found_interrupt = False
        self.vlab.run_for_ms(2000)
        self.assertEqual( self.found_interrupt, True)

    def test_wdt_not_triggered(self):
        self.found_interrupt = False
        self.vlab.run_for_ms(1000)
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(60)
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(1000)
        self.assertEqual(self.found_interrupt, False)


if __name__ == '__main__':
    unittest.main()
