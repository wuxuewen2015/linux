cmd_/opt/driver/led/led_module.ko := arm-linux-ld -EL -r  -T /opt/FriendlyARM/mini2440/linux-2.6.32.2/scripts/module-common.lds  -o /opt/driver/led/led_module.ko /opt/driver/led/led_module.o /opt/driver/led/led_module.mod.o