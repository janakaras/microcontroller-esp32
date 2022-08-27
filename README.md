# Microcontroller ESP32 Project

Project for Embedded Systems Course @unibz. 

## Overview 
For this project, two Microcontroller of type ESP32c3 were used. 

Setup: 
* 2 Microcontroller (A and B) that are connected with SPI 
* Microcontroller A measures the temperature and sends it to Microcontroller B. 
* Microcontroller B controls the blinking frequency of an LED according to the temperature and sends back a confirmation to Microcontroller A. 

## Details 

* ESP32-A: The voltage is read from the ADC / NTC Temperature Sensor and transformed into a temperature. 
* ESP32-A: The temperature is sent to ESP32-B via SPI. ESP32-A is the master. 
* ESP32-B: The temperature is received from ESP32-A via SPI. ESP32-B is the slave. 
* ESP32-B: The LED is controlled depending on the temperature using PWM. It is blinking faster when the temperature is higher and slower when the temperature is lower. 
* Design Description: The Design Description can be found in the root folder of this project. 

Command for flashing firmware A: 
$ idf.py -B build/fw-a/ -DBZ_BUILD_TARGET="fw-a" flash monitor

Command for flashing firmware B: 
$ idf.py -B build/fw-b/ -DBZ_BUILD_TARGET="fw-b" flash monitor
