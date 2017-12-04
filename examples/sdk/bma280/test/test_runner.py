import os
import unittest
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')

BMA2x2_STAT1_ADDR = 0x09
BMA2x2_STAT2_ADDR = 0x0A
BMA2x2_STAT_TAP_SLOPE_ADDR = 0x0B
BMA2x2_STAT_ORIENT_HIGH_ADDR = 0x0C

LOW_G_MASK = 0x01
HIGH_G_MASK = 0x02
HIGH_G_X_MASK = 0x01
HIGH_G_Y_MASK = 0x02
HIGH_G_Z_MASK = 0x04
SLOPE_MASK = 0x04
SLOPE_X_MASK = 0x01
SLOPE_Y_MASK = 0x02
SLOPE_Z_MASK = 0x04
DOUBLE_TAP_MASK = 0x10
DOUBLE_TAP_X_MASK = 0x10
DOUBLE_TAP_Y_MASK = 0x20
DOUBLE_TAP_Z_MASK = 0x40
SINGLE_TAP_MASK = 0x20
SINGLE_TAP_X_MASK = 0x10
SINGLE_TAP_Y_MASK = 0x20
SINGLE_TAP_Z_MASK = 0x40
ORIENT_MASK = 0x40
ORIENT_XY_MASK = 0x30
ORIENT_Z_MASK = 0x40
FLAT_MASK = 0x80
FIFO_FULL_MASK = 0x20
FIFO_WATERMARK_MASK = 0x40
NEW_DATA_MASK = 0x80

vlab = Vlab(working_directory=dir)
vlab.load(fw_bin)
vlab.run_for_ms(500)
print('Virtual device is running')

class TestBma280(unittest.TestCase):
    def setUp(self):
        vlab.run_for_ms(100)
        vlab.BMA280Device.reset_interrupt('reset_interrupt')
        vlab.run_for_ms(100)

    def tearDown(self):
        pass

    def check_3_axis_interrupts(self, int_reg, axis_mask, int_name, int_mask, int_axis_reg):
        vlab.BMA280Device.set_interrupt(int_name)
        vlab.run_for_ms(64)
        int_data = vlab.BMA280Device.get_register_value(int_reg)
        int_axis_data = vlab.BMA280Device.get_register_value(int_axis_reg)
        if (int(int_data) & int_mask != 0) and (int(int_axis_data) & axis_mask != 0):
            vlab.BMA280Device.unset_interrupt(int_name)
            vlab.run_for_ms(64)
            int_data = vlab.BMA280Device.get_register_value(int_reg)
            int_axis_data = vlab.BMA280Device.get_register_value(int_axis_reg)
            if int(int_data) | int(int_axis_data) == 0:
                return True
        return False

    def orient_interrupts(self, int_reg, axis_mask, int_name, int_mask, int_axis_reg):
        vlab.BMA280Device.set_interrupt(int_name)
        vlab.run_for_ms(64)
        int_data = vlab.BMA280Device.get_register_value(int_reg)
        int_axis_data = vlab.BMA280Device.get_register_value(int_axis_reg)
        if (int(int_data) & int_mask != 0) and (int(int_axis_data) & axis_mask != 0):
            vlab.BMA280Device.unset_interrupt('orient')
            vlab.run_for_ms(64)
            int_data = vlab.BMA280Device.get_register_value(int_reg)
            int_axis_data = vlab.BMA280Device.get_register_value(int_axis_reg)
            if int(int_data) | int(int_axis_data) == 0:
                return True
        return False

    def check_interrupts_without_axis(self, int_reg, int_name, int_mask):
        vlab.BMA280Device.set_interrupt(int_name)
        vlab.run_for_ms(64)
        int_data = vlab.BMA280Device.get_register_value(int_reg)
        if int(int_data) & int_mask != 0:
            vlab.BMA280Device.unset_interrupt(int_name)
            vlab.run_for_ms(64)
            int_data = vlab.BMA280Device.get_register_value(int_reg)
            if int(int_data) == 0:
                return True
        return False

    def check_reset_interrupt(self):
        vlab.BMA280Device.set_interrupt('high_g_x')
        vlab.BMA280Device.set_interrupt('single_tap_x')
        vlab.BMA280Device.set_interrupt('new_data')
        vlab.run_for_ms(64)
        int_data_1 = vlab.BMA280Device.get_register_value(BMA2x2_STAT1_ADDR)
        int_data_2 = vlab.BMA280Device.get_register_value(BMA2x2_STAT2_ADDR)
        int_data_3 = vlab.BMA280Device.get_register_value(BMA2x2_STAT_TAP_SLOPE_ADDR)
        int_data_4 = vlab.BMA280Device.get_register_value(BMA2x2_STAT_ORIENT_HIGH_ADDR)
        if int(int_data_1) != 0 and int(int_data_2) != 0 and int(int_data_3) != 0 and int(int_data_4) != 0:
            vlab.BMA280Device.reset_interrupt('reset_interrupt')
            vlab.run_for_ms(64)
            int_data_1 = vlab.BMA280Device.get_register_value(BMA2x2_STAT1_ADDR)
            int_data_2 = vlab.BMA280Device.get_register_value(BMA2x2_STAT2_ADDR)
            int_data_3 = vlab.BMA280Device.get_register_value(BMA2x2_STAT_TAP_SLOPE_ADDR)
            int_data_4 = vlab.BMA280Device.get_register_value(BMA2x2_STAT_ORIENT_HIGH_ADDR)
            if int(int_data_1) == 0 and int(int_data_2) == 0 and int(int_data_3) == 0 and int(int_data_4) == 0:
                return True
        return False

    def test_reset_interrupt(self):
        self.assertTrue(self.check_reset_interrupt())

    def test_should_recognize_low_g(self):
        self.assertTrue(self.check_interrupts_without_axis(BMA2x2_STAT1_ADDR, 'low_g', LOW_G_MASK))

    def test_should_recognize_high_g_x(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, HIGH_G_X_MASK, 'high_g_x', HIGH_G_MASK, BMA2x2_STAT_ORIENT_HIGH_ADDR))

    def test_should_recognize_high_g_y(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, HIGH_G_Y_MASK, 'high_g_y', HIGH_G_MASK, BMA2x2_STAT_ORIENT_HIGH_ADDR))

    def test_should_recognize_high_g_z(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, HIGH_G_Z_MASK, 'high_g_z', HIGH_G_MASK, BMA2x2_STAT_ORIENT_HIGH_ADDR))

    def test_should_recognize_slope_in_axis_x(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SLOPE_X_MASK, 'slope_x', SLOPE_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_slope_in_axis_y(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SLOPE_Y_MASK, 'slope_y', SLOPE_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_slope_in_axis_z(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SLOPE_Z_MASK, 'slope_z', SLOPE_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_double_tap_x(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, DOUBLE_TAP_X_MASK, 'double_tap_x', DOUBLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_double_tap_y(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, DOUBLE_TAP_Y_MASK, 'double_tap_y', DOUBLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_double_tap_z(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, DOUBLE_TAP_Z_MASK, 'double_tap_z', DOUBLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_single_tap_x(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SINGLE_TAP_X_MASK, 'single_tap_x', SINGLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_single_tap_y(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SINGLE_TAP_Y_MASK, 'single_tap_y', SINGLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_single_tap_z(self):
        self.assertTrue(self.check_3_axis_interrupts(BMA2x2_STAT1_ADDR, SINGLE_TAP_Z_MASK, 'single_tap_z', SINGLE_TAP_MASK, BMA2x2_STAT_TAP_SLOPE_ADDR))

    def test_should_recognize_orient_xy(self):
        self.assertTrue(self.orient_interrupts(BMA2x2_STAT1_ADDR, ORIENT_XY_MASK, 'orient_xy', ORIENT_MASK, BMA2x2_STAT_ORIENT_HIGH_ADDR))

    def test_should_recognize_orient_z(self):
        self.assertTrue(self.orient_interrupts(BMA2x2_STAT1_ADDR, ORIENT_Z_MASK, 'orient_z', ORIENT_MASK, BMA2x2_STAT_ORIENT_HIGH_ADDR))

    def test_should_recognize_flat(self):
        self.assertTrue(self.check_interrupts_without_axis(BMA2x2_STAT1_ADDR, 'flat', FLAT_MASK))

    def test_should_recognize_fifo_full(self):
        self.assertTrue(self.check_interrupts_without_axis(BMA2x2_STAT2_ADDR, 'fifo_full', FIFO_FULL_MASK))

    def test_should_recognize_fifo_watermark(self):
        self.assertTrue(self.check_interrupts_without_axis(BMA2x2_STAT2_ADDR, 'fifo_watermark', FIFO_WATERMARK_MASK))

    def test_should_recognize_new_data(self):
        self.assertTrue(self.check_interrupts_without_axis(BMA2x2_STAT2_ADDR, 'new_data', NEW_DATA_MASK))


if __name__ == '__main__':
    unittest.main()
    vlab.stop()
