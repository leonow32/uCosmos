# μCOSMOS

What is uCosmos?
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

## Why use uCosmos?

- Architecture based on periodic tasks.
- Deterministic execution.
- Ideal for small MCU applications.
- No context switching.
- Very low RAM usage.
- Easier debugging.
- Powerful command line interpreter to facilitate testing and development.

## Supported platforms

| Platform                 | Toolchain |
|--------------------------|-----------|
| Espressif ESP32          | ESP-IDF   |
| Raspberry Pi Pico RP2xxx | Pico SDK  |

## Architecture overview

The uCosmos system was created to facilitate the management of multiple tasks — not just within a single project, but across many. It was designed with the goal of allowing code written once to be reused in various products and other projects. In the example below, we will look at a simple project where the code repository is set up for two products: a *premium* version that includes a display, a clock, and EEPROM memory, and a *basic* version that features only a clock and EEPROM memory, but lacks a display.

### Folder tree

This is how the project tree should look. The repository consists of four components: `uCosmos`, `clock`, `eeprom`, and `lcd_display`. Each component has its own folder, which stores the **shared** code used across all products. Each component can be set up as a separate submodule with its own repository, allowing the code to be shared among different projects. Furthermore, each component can be adjusted to the specific requirements of your project through configuration files; these are stored in a `_config` folder and belong to your project's repository, rather than to the submodules themselves.

```
your_project
│   .gitignore
│   .gitmodules
│   main.cpp
│
├── _config                        - Folder for all configuration files, stored in your repo
│       _commands.h                - Command list
│       _config.h                  - General config for all your products and components included for each product
│       eeprom_config.h            - Config file for EEPROM component
│       lcd_display_config.h       - Config file for LCD Display component
│       uCosmos_config.h           - Config file for uCosmos component
│
├── clock                          - Example of a very simple component: no config file, no commands
│       clock.cpp
│       clock.h
│
├── eeprom                         - Example of a more advanced component: config file present and a command list
│   │   eeprom.cpp
│   │   eeprom.h
│   │   eeprom.hxx                 - File with command list used in this component to be included in `_config/_commands.h`
│   │
│   └── template
│           eeprom_config.h        - Copy this file to `_config` folder
│
├── lcd_display                    - Example of a component where all the commands are placed in separate files
│   │   lcd_display.cpp
│   │   lcd_display.h
│   │   lcd_display_demo.cpp       - Commands for testing and debug only
│   │   lcd_display_demo.h
│   │   lcd_display_demo.hxx       - File with command list used in this component to be included in `_config/_commands.h`
│   │
│   └── template
│           lcd_display_config.h   - Copy this file to `_config` folder
│
└── uCosmos                        - This repo (cloned as a submodule)
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
    └── template
            uCosmos_config.h       - Copy this file to `_config` folder
            uCosmos_template.h
            _commands.h            - Copy this file to `_config` folder
```

### Cloning the repository

uCosmos is designed to work as a submodule within an existing GIT repository. Navigate to the folder containing your repository—or create a new one—and then clone uCosmos as a submodule using the following command:

```bash
git submodule add git@github.com:leonow32/uCosmos.git uCosmos
git submodule update --init --recursive
```

### Main config

Let’s examine the contents of the `_config/_config.h` file. It contains the general configuration for all products that can be compiled using the code in this repository. We can see two products defined here, represented by `PRODUCT_BASIC` and `PRODUCT_PREMIUM`. Each product consists of several components, which are included via `define COMPONENT_XXX` directives; a value greater than 0 indicates that a component is present (and this value may represent the component's version number, if the component supports it). The absence of a definition, or a setting of `define COMPONENT_XXX 0`, means that the specified component should not be included in the given product.

```c++
// File _config/_config.h
#define PRODUCT_BASIC               0
#define PRODUCT_PREMIUM             1  // This will be built

#if PRODUCT_BASIC
    #define COMPONENT_CLOCK         1
    #define COMPONENT_EEPROM        1

#endif

#if PRODUCT_PREMIUM
    #define COMPONENT_CLOCK         1
    #define COMPONENT_EEPROM        1
    #define COMPONENT_LCD_DISPLAY   1   // Please note this component is not present in PRODUCT_BASIC
#endif
```

### Startup

Below is an example of a `main.cpp` file. At the very beginning, there are `#include` directives for various components; however, these are wrapped in `#if COMPONENT_XXX` preprocessor directives to ensure that only the components specified in the main configuration file—located at `_config/_config.h`—are included.

The `main()` function can begin with the initialization of hardware peripherals (excluding those already handled by the system itself). If this is not required, you may skip this step. Next, initialize the system by calling `uCosmos_init()`. From this point on, you can add tasks to be executed by the task scheduler.

Components typically require initialization during the application's startup. To achieve this, you must call the `component_init()` function for each component; as with the `#include` directives, each initialization call must be enclosed within an `#if COMPONENT_XXX` conditional block.

The task manager is launched by calling the `uCosmos_scheduler()` function. This function contains an infinite `while(true)` loop, meaning the program never exits it—unless a system shutdown request is issued. Such a scenario might occur, for example, prior to powering down the device.

```c++
// File main.cpp
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

### Component config

Let’s look at how component configuration files work, using the `clock` component as an example. Once you have downloaded the component submodule, you must copy its configuration template from `component/template/component_config.h` to `_config/component_config.h`.

Next, copy the default configuration as many times as you have products; for instance, the example below shows two configurations — one for `PRODUCT_BASIC` and one for `PRODUCT_PREMIUM`. If several products share an identical configuration, you can group them using the `||` operator, like this: `PRODUCT_BASIC || PRODUCT_PREMIUM`. In our example, the configuration for these two products differs slightly.

Note two important details. At the end of the configuration for each product, we define `#define CLOCK_CONFIG_DONE`. This helps identify errors that might occur if you add a new product to the main configuration but forget to include a specific component. In such a case, the compilation process will trigger an `#error "Missing config"` rather than a flood of messages reporting that certain variables could not be found. Secondly, there is `#define CLOCK_CONFIG_VERSION 261116`, which records the date the configuration template was last updated, in `YYMMDD` format. Keep in mind that the component code resides in the submodule, while the configuration file belongs to your own repository. If you update the submodule, the new version may include configuration changes that you will need to transfer from the template to your file in the `_config` folder. If it turns out that a component submodule requires an update to its configuration file, you will receive the following message: `#error "The version of the configuration file stored in the repository is not supported.. Please copy clock/template/clock_config.h to your _config folder"`.

```c++
#ifndef CLOCK_CONFIG_H_
#define CLOCK_CONFIG_H_

#define CLOCK_CONFIG_VERSION            261116

#if PRODUCT_BASIC
    #define CLOCK_GPIO					10
    #define CLOCK_FREQUENCY				32768
    #define CLOCK_USE_DEMO_COMMANDS     true
    #define CLOCK_CONFIG_DONE
#endif

#if PRODUCT_PREMIUM
    #define CLOCK_GPIO					11
    #define CLOCK_FREQUENCY				32768
    #define CLOCK_USE_DEMO_COMMANDS     false
    #define CLOCK_CONFIG_DONE
#endif

// ========================================
// Error handling
// ========================================

#ifndef CLOCK_CONFIG_DONE
    #error "Missing config"
#endif

#endif /* CLOCK_CONFIG_H_ */
```

## Task management

A task is, in essence, a C++ function written in a specific way. This function is invoked by the system scheduler at a configured period (or, more precisely, no more frequently than at a configured period) and continues to run until it completes its execution and returns via `return` instruction. There are no mechanisms for task preemption, nor are there separate stacks for each task—unlike in FreeRTOS. This solution is simple and effective, but it is important to note that **if a single task hangs, the entire system will hang**.

A task function always returns `void` and accepts a single argument, `run_mode`, which defines the mode in which the task is called. Possible values ​​include:
- `os_run` – a standard, periodic execution.
- `os_constructor` – the task constructor, called immediately after the task is added to the scheduler’s task array.
- `os_destructor` – the task destructor, called after the task is removed from the scheduler’s task array.
- `os_id` – an identification call, used solely to retrieve the task's name for diagnostic purposes.

There is an important detail regarding the task destructor when a task removes itself. In such a case, the task is invoked by the scheduler with the `os_run` argument. To initiate its own removal, the task calls the `task_remove()` function, passing itself as an argument. `task_remove()` then triggers the task's destructor; however, once the destructor finishes, execution returns to the main body of the task function. This means that a task that has initiated its own removal continues to execute for a short period until it reaches its own `return` statement. Therefore, **it is good practice for the removal of a task to be the last instruction that the task executes, immediately followed by a `return` statement**.

### Task template

Below are two task templates that are equivalent; the choice between them depends solely on your preference. One uses a `switch-case` statement, while the other uses `if-else`.

Each task must support being called with the arguments `os_run` and `os_id`. Identification is performed by assigning the task name in the form of a C-string to the global variable `const char * task_name`. It is important that this string must remain in memory after the task function has returned. Therefore, it is best to store it in Flash memory and not to use RAM for this purpose. The `__func__` identifier is useful here, as it automatically inserts the name of the function in which it is used. Handling the `os_constructor` and `os_destructor` arguments is optional. If your task needs to store variables between calls, it is good practice to define them as static variables at the beginning of the task.

```c++
void template_task(run_mode_t run_mode) {
    // Declare static variables here

    switch(run_mode) {
        case os_run: {						// Mandatory

            break;
        }

        case os_constructor: {				// Optional

            break;
        }

        case os_destructor: {				// Optional

            break;
        }

        case os_id: {						// Mandatory
            task_name = __func__;			// Do not change this line
            break;
        }
    }
}
```

```c++
void template_task(run_mode_t run_mode) {
    // Declare static variables here

    if(run_mode == os_run) {				// Mandatory

    }
    else if(run_mode == os_constructor) {	// Optional

    }
    else if(run_mode == os_destructor) {	// Optional

    }
    else if(run_mode == os_id) {			// Mandatory
        task_name = __func__;				// Do not change this line
    }
}
```

### Adding a task

To add a task to the scheduler's task table, call the function `os_t task_add(void (*task_ptr)(run_mode_t), uint16_t period_ms)`, where `task_ptr` is a pointer to the task and `period_ms` is the task's execution period given in milliseconds. After adding the task, it is called with the `os_constructor` argument. The constructor is called immediately and synchronously from `task_add()`.

Examples:

```c++
task_add(some_task, 1000);
task_add(other_task, 500);
task_add(another_task, 200);
```

This function can return one of the following values:

- `os_ok` – success.
- `os_nullptr` – the `task_ptr` argument is `nullptr`.
- `os_task_period_under_range` – the task was not added because its execution period is shorter than the system tick period, defined by the `TICK_PERIOD_MS` parameter in the configuration file.
- `os_task_already_created` – the specified task has already been added; duplicate tasks cannot be added.
- `os_no_free_slot` – no space available in the task array; consider increasing the `TASK_MAXCOUNT` parameter in the configuration file.
- `os_period_rounded` – the task was added, but since the requested period is not a multiple of `TICK_PERIOD_MS`, the period has been rounded.

### Removing a tasks

To remove a task from the scheduler's task array, use the function `os_t task_remove(void (*task_ptr)(run_mode_t))`, where `task_ptr` is a pointer to the task.

Examples:

```c++
task_remove(some_task);
task_remove(other_task);
task_remove(another_task);
```

This function can return one of the following values:

- `os_ok` – success.
- `os_nullptr` – the `task_ptr` argument is `nullptr`.
- `os_not_found` - the specified task is not present in the scheduler's task array.

### Changing the period of a task

To change the execution period of a task, call the function `os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms)`, where `task_ptr` is a pointer to the task and `period_ms` is the task's execution period given in milliseconds.

Examples:

```c++
task_period_change(some_task, 1234);
task_period_change(other_task, 5678);
task_period_change(another_task, 31415);
```

This function can return one of the following values:

- `os_ok` – success.
- `os_nullptr` – the `task_ptr` argument is `nullptr`.
- `os_not_found` - the specified task is not present in the scheduler's task array.
- `os_task_period_under_range` – the task was not added because its execution period is shorter than the system tick period, defined by the `TICK_PERIOD_MS` parameter in the configuration file.
- `os_period_rounded` – the task was added, but since the requested period is not a multiple of `TICK_PERIOD_MS`, the period has been rounded.

### Checking whether a task is added

To check whether a task has been added to the scheduler's task array, use the function `bool task_is_running(void (*task_ptr)(run_mode_t))`. Important: this function does not check whether the specified task is currently being executed; rather, it checks whether the task is scheduled for execution within the schedule.

Example:

```c++
if(task_is_running(my_task)) {
    LOGI("The task has been added");
}
else {
    LOGI("The task has been removed");
}
```

This function can return one of the following values:

- `true`
- `false`

### Getting the Task Name

To get the task name, use the const char * task_get_name(void (*task_ptr)(run_mode_t)) function. This function returns a pointer to a C-string containing the task's name. If the task does not support identification, the function returns the string `"unknown"`; if it is called with a null pointer, it returns `"nullptr"`.

Example

```c++
printf("My task is called %s.\n", get_task_name(my_task));
```

## TODO Console

To be ~~completed~~...

C~2~H~5~OH

C<sub>2</sub>H<sub>5</sub>OH

a^2^ + b^2^ = c^2^

a<sup>2</sup> + b<sup>2</sup> = c<sup>2</sup>

## Logs

The uCosmos system provides a convenient library for displaying logs. There are four types of messages, distinguished by color and a prefix at the beginning of the line:

| Log  | Prefix | Color  | Purpose     |
|------|--------|--------|-------------|
|`LOGE`| E      | Red    | Error       |
|`LOGW`| W      | Yellow | Warning     |
|`LOGI`| I      | Green  | Information |
|`LOGD`| D      | White  | Debug       |

The code below presents basic examples of all four types of log messages:

```c++
LOGE("Error (red)");
LOGW("Warning (yellow)");
LOGI("Information (green)");
LOGD("Debug (White)");
```

Here is the result:

```bash
E (1369) main.cpp: Error (red)
W (1373) main.cpp: Warning (yellow)
I (1377) main.cpp: Information (green)
I (1389) main.cpp: Debug (White)
```

The number in parentheses represents the time in milliseconds, measured from system startup. The *.cpp filename is added automatically to make it easier to locate the source of these log entries. There is no need to add "\n", as a newline character is added automatically.

Within logs, you can display variables in exactly the same way as you would using the `printf()` function. Here are a few examples.

```c++
char character = 'a';
int  integer   = 123;
uint8_t  u8    = 255;
uint16_t u16   = 65535;
uint32_t u32   = 4294967295UL;

LOGI("character = %c", character);
LOGI("integer = %d", integer);
LOGI("u8 = %u", u8);
LOGI("u16 = %u", u16);
LOGI("u32 = %lu", u32);
```

### Formatting and color



## Change log

The project has been under continuous development since 2011.

### 5.3.0 - 2026.11.17
* Added quite advanced task diagnostics, including task time measurement, maximum and average time, overrun control, CPU usage, etc.
* Added `CMD_HELP` macro to facilitate the creation of user guides for console commands.
* Added `os_period_rounded` warning that appears if the period of the task being created is not a multiple of TICK_PERIOD_MS.
* Added version control of config files.
* Changed `task_remove` to `task_remove` because this function in fact doesn't close the task (it may be currently running) but only removes it from `task_table` so it will not be called anymore unless it is added again.
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