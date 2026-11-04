# Change Log

## 5.1.0 - 2026.11.03
* Added `parse_dec()`, `parse_hex()` and `parse_bin()` for greater convenience, eliminating the need to specify the numeral base each time.
* Kay `'` can be used as a digit separator in numeric parsers.
* Fixed bug in verification of the max value given to parse_uint().
* Demos of argument parsers have been optimized.

## 5.0.0 - 2026.10.04
* Initial release to GitHub.
* Ported to Raspberry Pi Pico RP2040, RP2350, RP2354.

## 4.0.0 - 2024
* Ported to Espressif ESP32.

## 3.0.0 - 2018
* Redesigned the entire task management system and command interpreter to provide a robust foundation for various projects using AVR microcontrollers.

## 2.0.0 - 2013
* I developed the second version for my master’s thesis, which was a system for monitoring building tilt. It consisted of a central unit that communicated with sensor modules, which in turn collected data from various inclinometers.

## 1.0.0 - 2012
* The first version was developed as part of my engineering thesis. At the time, it served as an HPGL language interpreter running on an ATmega128 microcontroller, designed to control a pen plotter.