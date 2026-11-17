# μCOSMOS

What's all about it?
* **μC** - Microcontroller
* **OS** - Operating System
* **MOS** - Metal Oxide Semiconductor (yeah, that's dumb...)

This is a simple, single-threaded, single-core operating system and command-line interpreter designed for microcontrollers. It currently supports microcontrollers such as the Espressif ESP32, as well as the Raspberry Pi RP2040, RP2350, and RP2354; however, adding support for other devices is very straightforward. All that is required is a single timer that triggers an interrupt at regular intervals and an input stream for keyboard data, such as a UART or USB interface.

## Features

- Cooperative, non-preemptive scheduler driven by a single hardware timer.
- Periodic tasks with configurable periods.
- Full task lifecycle supported: constructor, destructor, run and identification.
- A built-in command-line interpreter that generates standard `argc` and `argv` data.
- CTRL-Z to recall the last command.
- Rich, type-safe set of argument parsers for 8/16/32/64-bit signed integers, 8/16/32/64 bit unsigned integers, float/double values, ASCII characters, ASCII strings, HEX strings and timestamps.
- Optional diagnostic subsystem measuring task execution time, overruns, total execution time and total CPU usage.
- Portable and compact code: the hardware abstraction layer consists of a handful of macros, so porting to a new MCU is very easy.

## Supported platforms

| Platform                 | Toolchain |
|--------------------------|-----------|
| Espressif ESP32          | ESP-IDF   |
| Raspberry Pi Pico RP2xxx | Pico SDK  |

## Folder tree

```
your_project
│   .gitignore
│   .gitmodules
│   main.cpp
│
├───_config                     - Folder for all configuration files, stored in your repo
│       _commands.h             - Command list
│       _config.h               - General config for all your products and components included for each product
│       module2_config.h        - Config file for example module 2
│       module3_config.h        - Config file for example module 3
│       uCosmos_config.h        - Config file for uCosmos
│
├───module1                     - Example of a very simple submodule: no config file, no commands
│       module1.cpp
│       module1.h
│
├───module2                     - Example of a more advanced submodule: config file present and a command list
│   │   module2.cpp
│   │   module2.h
│   │   module2.hxx             - File with command list used in this submodule to be included in `_config/_commands.h`
│   │
│   └───template
│           module2_config.h    - Copy this file to `_config` folder
│
├───module3                     - Example of a submodule where all the commands are placed in separate files
│   │   module3.cpp
│   │   module3.h
│   │   module3_demo.cpp        - Commands for testing and debug only
│   │   module3_demo.h
│   │   module3_demo.hxx        - File with command list used in this submodule to be included in `_config/_commands.h`
│   │
│   └───template
│           module3_config.h    - Copy this file to `_config` folder
│
└───uCosmos                     - This repo
    │   commands.cpp
    │   commands.h
    │   commands.hxx
    │   console.cpp
    │   console.h
    │   log.h
    │   readme.md               - You are reading this file
    │   task.cpp
    │   task.h
    │   uCosmos.cpp
    │   uCosmos.h
    │
    └───template
            uCosmos_config.h    - Copy this file to `_config` folder
            uCosmos_template.h
            _commands.h         - Copy this file to `_config` folder
```

## Change log

### 5.3.0 - 2026.11.17
* Added quite advanced task diagnostics, including task time measurement, maximum and average time, overrun control, CPU usage, etc.
* Added `CMD_HELP` macro to facilitate the creation of user guides for console commands.
* Added `os_period_rounded` warning that appears if the period of the task being created is not a multiple of TICK_PERIOD_MS.
* Added version control of config files.
* Changed `task_close` to `task_remove` because this function in fact doesn't close the task (it may be currently running) but only removes it from `task_table` so it will not be called anymore unless it is added again.
* All comments translated to English.

### 5.2.0 - 2026.11.09
* Added float parser.
* Added `uCosmos_deinit` to shut down the system before power is cut off.
* Added `print_hexstr` function to make it easier to print arrays in HEX format.

### 5.1.0 - 2026.11.03
* Added `parse_dec()`, `parse_hex()` and `parse_bin()` for greater convenience, eliminating the need to specify the numeral base each time.
* Key `'` can be used as a digit separator in numeric parsers.
* Fixed bug in verification of the max value given to `parse_uint()`.
* Demos of argument parsers have been optimized.

### 5.0.0 - 2026.10.04
* Initial release to GitHub.
* Ported to Raspberry Pi Pico RP2040, RP2350, RP2354.

### 4.0.0 - 2024
* Ported to Espressif ESP32.

### 3.0.0 - 2018
* Redesigned the entire task management system and command interpreter to provide a robust foundation for various projects using AVR microcontrollers.

### 2.0.0 - 2013
* I developed the second version for my master's thesis, which was a system for monitoring building tilt. It consisted of a central unit that communicated with sensor modules, which in turn collected data from various inclinometers.

### 1.0.0 - 2011
* The first version was developed as part of my engineering thesis. At the time, it served as an HPGL language interpreter running on an ATmega128 microcontroller, designed to control a pen plotter.