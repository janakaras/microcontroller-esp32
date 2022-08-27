# Project template Jana Karas

Project template for the lab project of the course Embedded Systems Design and Implementation (76088A).

## Progress 

The project is finished! All desired functions have been implemented: 
* ESP32-A: The voltage is read from the ADC / NTC Temperature Sensor and transformed into a temperature. 
* ESP32-A: The temperature is sent to ESP32-B via SPI. ESP32-A is the master. 
* ESP32-B: The temperature is received from ESP32-A via SPI. ESP32-B is the slave. 
* ESP32-B: The LED is controlled depending on the temperature using PWM. It is blinking faster when the temperature is higher and slower when the temperature is lower (this change has been discussed with the professor). 
* Design Description: The Design Description can be found in the root folder of this project. 

## Project layout

```
├── CMakeLists.txt         # Root CMakeLists.txt of the project
├── README.md
├── sdkconfig.defaults     # Where you can put your Kconfig overrides
├── sdkconfig.fw_a         # Where you can put your Kconfig overrides that apply only to firmware "a"
├── sdkconfig.fw_b         # Where you can put your Kconfig overrides that apply only to firmware "b"
└── src
    ├── components         # Folder of components shared between firmware "a" and firmware "b"
    │   └── shared_foo     # Example of shared component
    ├── fw-a               # Source code for firmware "a"
    │   ├── components     # Folder of components specific to firmware "a"
    │   │   └── foo        # Example of component used only in firmware "a"
    │   └── main           # Main component of firmware "a"
    └── fw-b               # Source code for firmware "b"
        ├── components     # Folder of components specific to firmware "b"
        │   └── foo        # Example of component used only in firmware "b"
        └── main           # Main component of firmware "b"
```

## Configurable CMake variables

* `BZ_BUILD_TARGET` (type: string). The firmware to build. Available values are:
    * `fw-a`: build the firmware a
    * `fw-b`: build the firmware b

## Building the project

* Note: the CMake cache variables need to be passed only once and then they are, as the name says, cached.
* Note: since I already set the IDF_TARGET CMake cache variable in the [CMakeLists](./CMakeLists.txt#L43), there is no need to run `idf.py set-target "esp32c3"`.
* Note: you may want to pass the `-p PORT` option to idf.py to specify the serial port to which your esp32c3 is connected. This is especially useful if you're working with two ESP32C3 connected at the same time.

```sh
# Working with firmware a, for which we use build/fw-a as build directory
# Build the firmware
# The first time we "initialize" (configure is a more correct term in the CMake world) the build directory, we must pass the cache variables.
$ idf.py -B build/fw-a/ -DBZ_BUILD_TARGET="fw-a" build
# Flash the firmware
$ idf.py -B build/fw-a/ flash
# Monitor the firmware via the serial port
$ idf.py -B build/fw-a/ monitor
# Or, if you prefer to speed up things. Build, flash and monitor in one command
$ idf.py -B build/fw-a/ flash monitor

# Working with firmware b, for which we use build/fw-b as build directory
# It is the same of firmware a, but instead of `-B build/fw-a/` we pass `-B build/fw-b/` 
# and instead of -DBZ_BUILD_TARGET="fw-a" we pass -DBZ_BUILD_TARGET="fw-b".
$ idf.py -B build/fw-b/ -DBZ_BUILD_TARGET="fw-b" flash monitor
```

## Useful materials

* [chenlijun99/unibz-76088A-materials](https://github.com/chenlijun99/unibz-76088A-materials) (shameless plug)
* [ESP32-C3-DevKitM-1, your dev board](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html)

### Build System and project configuration

* [Documentation of the ESP32-flavoured CMake](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html)
    * [idf_component_register](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#component-requirements)
    * [Kconfig](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#example-component-cmakelists)
* [Further details of Kconfig](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/kconfig.html)

### FreeRTOS

* ["Vanilla" FreeRTOS documentation](https://www.freertos.org/features.html)
* [FreeRTOS API reference](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos.html)
* [ESP additions to FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/freertos_additions.html)

### Useful components

For most of the following component, you should be able to find examples in the SDK of the ESP32-C3, in the `examples` folder.

* Communication between the two ESP32C3s.
    * [SPI master](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/spi_master.html)
    * [SPI slave](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/spi_slave.html)
* [Console](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/system/console.html). A better way to interface with your firmware via UART, instead of using `scanf()`. You can create a simple shell using this component.
* [ADC](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/adc.html). You will use this (plus some formulae that we saw during the lectures) to measure the temperature via a NTC.
* [I2C](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/i2c.html). You need this to interface with the light sensor.
* [GPIO](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/gpio.html).
    * You can use a GPIO to turn on and off the simple LED available on the custom PCB.
    * You can use a GPIO as a way for the slave to signal the master (via an interrupt) to initiate an SPI communication.
* [LED Control](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/ledc.html)
    * You can use this to control the brightness of the LED on the custom PCB via PWM (Pulse Width Modulation).
* [led_strip](https://github.com/chenlijun99/unibz-76088A-materials/tree/dev/3-blink/components/led_strip). This component contains the driver to control the RGB LED on your development board, as we saw during the lecture. When dealing with the RGB LED, you don't need PWM to control brightness. You just need to decrease the intensity of the RGB colors.
    * [WS2812B](http://www.world-semi.com/Certifications/WS2812B.html): the RGB LED controller module
         * [Download the datasheet](http://www.world-semi.com/DownLoadFile/108)
    * [RMT (Remote Control) module API reference](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/rmt.html): the RMT component is used by the `led_strip` component to communicate with the WS2812B, i.e. to control the RGB LEDs.

## Misc

* I also included an `.clang-format` in the root of the project. You can configure your editor/IDE to use `clang-format` to auto format your code following the rules defined in `.clang-format` file. If you have time for bikeshedding or have some strong opinions on the format of the source code, feel free to adapt the `.clang-format` file to you preference.
