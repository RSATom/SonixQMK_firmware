#!/bin/bash -e
./make_filco_m10sp.sh
#sudo ./sonixflasher --vidpid 2efd/2372 --reboot sonix --file ./filco_m10sp_default.bin
sudo ./sonixflasher --vidpid 0c45/7040 --file ./filco_m10sp_default.bin
