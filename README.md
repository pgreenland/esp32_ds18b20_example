# ESP-32 DS18B20 Example

This repository contains a PlatformIO project for the ESP32-C3 demonstrating sampling temperatures from one or more Analog / Dallas DS18B20 temperature sensors.

It makes use of the following two repositories:

* https://github.com/pgreenland/esp32_one_wire_uart
* https://github.com/pgreenland/esp32_ds18b20

These are included as sub-modules.

Having cloned this repository, the sub-modules may be checked out / updated with:

```
git submodule update --init --recursive
```

The example makes use of UART 1 to implement a 1-Wire bus on GPIO 6.

When run with temperature sensors connected the output should appear as follows:

```
I (489) MAIN: Found device ROM code: 28FF640E7D249B67
I (489) MAIN: .Device is a DS18B20 temperature sensor
I (529) MAIN: Found device ROM code: 28FF640E7F3C4807
I (529) MAIN: .Device is a DS18B20 temperature sensor
I (699) MAIN: Performing individual temperature conversions and readings
I (789) MAIN: Temperature [0]: 19.06 degC
I (869) MAIN: Temperature [1]: 18.94 degC
I (1369) MAIN: Issuing simultaneous temperature conversion to all DS18B20 devices
I (1439) MAIN: Temperature [0]: 19.06 degC
I (1469) MAIN: Temperature [1]: 18.94 degC
```

For more information see my associated blog post: https://www.quantulum.co.uk/blog/1-wire-with-uart-on-esp-32
