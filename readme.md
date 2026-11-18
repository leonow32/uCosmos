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

The uCosmos system was created to facilitate the management of multiple tasks — not just within a single project, but across many. It was designed with the goal of allowing code written once to be reused in various products and other projects.

```
your_project
│   .gitignore
│   .gitmodules
│   main.cpp
│
├───_config                        - Folder for all configuration files, stored in your repo
│       _commands.h                - Command list
│       _config.h                  - General config for all your products and components included for each product
│       eeprom_config.h            - Config file for EEPROM component
│       lcd_display_config.h       - Config file for LCD Display component
│       uCosmos_config.h           - Config file for uCosmos component
│
├───clock                          - Example of a very simple component: no config file, no commands
│       clock.cpp
│       clock.h
│
├───eeprom                         - Example of a more advanced component: config file present and a command list
│   │   eeprom.cpp
│   │   eeprom.h
│   │   eeprom.hxx                 - File with command list used in this component to be included in `_config/_commands.h`
│   │
│   └───template
│           eeprom_config.h        - Copy this file to `_config` folder
│
├───lcd_display                    - Example of a component where all the commands are placed in separate files
│   │   lcd_display.cpp
│   │   lcd_display.h
│   │   lcd_display_demo.cpp       - Commands for testing and debug only
│   │   lcd_display_demo.h
│   │   lcd_display_demo.hxx       - File with command list used in this component to be included in `_config/_commands.h`
│   │
│   └───template
│           lcd_display_config.h   - Copy this file to `_config` folder
│
└───uCosmos                        - This repo (cloned as a submodule)
    │   commands.cpp
    │   commands.h
    │   commands.hxx
    │   console.cpp
    │   console.h
    │   log.h
    │   readme.md                  - You are reading this file
    │   task.cpp
    │   task.h
    │   uCosmos.cpp
    │   uCosmos.h
    │
    └───template
            uCosmos_config.h       - Copy this file to `_config` folder
            uCosmos_template.h
            _commands.h            - Copy this file to `_config` folder
```


Example of main.cpp.

```c++

#include "_config/_config.h"
#include "uCosmos/uCosmos.h"

#if COMPONENT_CLOCK
	#include "clock/clock.h"
#endif

#if COMPONENT_EEPROM
	#include "eeprom/eeprom.h"
#endif

#if COMPONENT_LCD_DISPLAY
	#include "lcd_display/lcd_display.h"
#endif

int main() {
	// Hardware init here
    // ...

    // Initialize the system before any tasks are added
	uCosmos_init();

    #if COMPONENT_CLOCK
        clock_init();
    #endif

    #if COMPONENT_EEPROM
        eeprom_init();
    #endif

    #if COMPONENT_LCD_DISPLAY
        lcd_display_init();
    #endif

	// Infinite loop is here
    uCosmos_scheduler();

	// Return from uCosmos_scheduler is possible to power down the product
    LOGW("End of main()");
}

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