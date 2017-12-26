import os
from jumper.vlab import Vlab


CHIP_ID_REGISTER = 0xD0
dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


vlab = Vlab(working_directory=dir, print_uart=False)
vlab.load(fw_bin)
vlab.start()
print('Virtual device is running')

print('Reading chip ID from register {}'.format(hex(CHIP_ID_REGISTER)))
chip_id = int(vlab.BME280.get_register_value(CHIP_ID_REGISTER))
print('Chip id is: {}'.format(hex(chip_id)))

chip_id = 0xFF
print('Setting chip ID to {}'.format(hex(chip_id)))
vlab.BME280.set_register_value(CHIP_ID_REGISTER, chip_id)
print('Reading chip ID from register {}'.format(hex(CHIP_ID_REGISTER)))
chip_id = int(vlab.BME280.get_register_value(CHIP_ID_REGISTER))
print('Chip id is: {}'.format(hex(chip_id)))

vlab.stop()
