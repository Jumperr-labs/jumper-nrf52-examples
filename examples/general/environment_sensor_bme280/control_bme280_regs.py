import os
from jumper.vlab import Vlab


CHIP_ID_REGISTER = 0xD0
TEMP_LSB_REGISTER = 0xFB
TEMP_MSB_REGISTER = 0xFA

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


vlab = Vlab(working_directory=dir, print_uart=False)
vlab.load(fw_bin)
vlab.run_for_ms(500)
vlab.resume()
print('Virtual device is running')

print('Reading chip ID from register {}'.format(hex(CHIP_ID_REGISTER)))
chip_id = vlab.BME280.get_register_value(CHIP_ID_REGISTER)
print('Chip id is: {}'.format(hex(chip_id)))

chip_id = 0xFF
print('Setting chip ID to {}'.format(hex(chip_id)))
vlab.BME280.set_register_value(CHIP_ID_REGISTER, chip_id)
print('Reading chip ID from register {}'.format(hex(CHIP_ID_REGISTER)))
chip_id = vlab.BME280.get_register_value(CHIP_ID_REGISTER)
print('Chip id is: {}'.format(hex(chip_id)))

vlab.pause()
for i in range(10):
    temperature_lsb = vlab.BME280.get_register_value(TEMP_LSB_REGISTER)
    temperature_msb = vlab.BME280.get_register_value(TEMP_MSB_REGISTER)
    temperature = (temperature_msb << 1) | temperature_lsb
    print('Raw data in temperature  register is: {}'.format(temperature))
    vlab.run_for_ms(1000)

vlab.stop()
