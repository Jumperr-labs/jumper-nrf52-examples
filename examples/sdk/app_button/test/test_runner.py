import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestAppButton(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        self.vlab = Vlab(working_directory=dir)
        self.vlab.load(fw_bin)
        self.vlab.on_pin_level_event(self.pins_listener)
        self.vlab.run_for_ms(500)
        print('Virtual device is running')
        self.is_led_on = False

    def pins_listener(self, pin_number, pin_level):
        if pin_number == 17 and pin_level == 0:
            self.is_led_on = True

    def tearDown(self):
        self.vlab.stop()

    def print_device_time(self):
        print('Device time: ' + str(self.vlab.get_device_time_ms()) + ' ms')

    def test_led_should_turn_on_on_button_push(self):
        self.print_device_time()
        print('Button on')
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(60)
        self.print_device_time()
        print('Button off')
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(500)
        
        # in this case we are going to poll for the pin level  
        self.assertEqual(self.vlab.get_pin_level(17), 0)
  
    def test_should_ignore_button_noise(self):
        self.print_device_time()
        print('Button on')
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(1)
        self.print_device_time()
        print('Button off')
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(500)
        self.assertFalse(self.is_led_on)
          

if __name__ == '__main__':
    unittest.main()
