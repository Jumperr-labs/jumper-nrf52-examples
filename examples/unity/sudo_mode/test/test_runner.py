import os
from time import sleep
from jumper.vlab import Vlab

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')

vlab = Vlab(working_directory=dir, sudo_mode=True, print_uart=True)
vlab.load(fw_bin)

with vlab as v:
    print('Virtual device is running')
    while v.is_running():
        sleep(0.1)
