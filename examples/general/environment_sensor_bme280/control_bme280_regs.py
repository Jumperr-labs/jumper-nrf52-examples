import os
from time import sleep
from jumper.vlab import Vlab


CHIP_ID_REGISTER = 0xD0
TEMP_LSB_REGISTER = 0xFB
TEMP_MSB_REGISTER = 0xFA

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


vlab = Vlab(working_directory=dir, print_uart=False)
vlab.load(fw_bin)
vlab.start()
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

print("Pausing the system")
vlab.pause()
for i in range(10):
    print("Reading temperature")
    temperature_lsb = vlab.BME280.get_register_value(TEMP_LSB_REGISTER)
    temperature_msb = vlab.BME280.get_register_value(TEMP_MSB_REGISTER)
    temperature = (temperature_msb << 1) | temperature_lsb
    print('Raw data in temperature  register is: {}'.format(temperature))

    print('Running the system for 500ms')
    vlab.run_for_ms(500)
    sleep(0.3)
    print('System is paused')

vlab.stop()
