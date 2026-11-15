# μCOSMOS

What's all about it?
* **μC** - Microcontroller
* **OS** - Operating System
* **MOS** - Metal Oxide Semiconductor (yeah, that's dumb...)

This is a simple, single-threaded, single-core operating system and command-line interpreter designed for microcontrollers. It currently supports microcontrollers such as the Espressif ESP32, as well as the Raspberry Pi RP2040, RP2350, and RP2354; however, adding support for other devices is very straightforward. All that is required is a single timer that triggers an interrupt at regular intervals and an input stream for keyboard data, such as a UART or USB interface.

## Table of contents

- [Features](#features)
- [Requirements and supported platforms](#requirements-and-supported-platforms)
- [Folder tree](#folder-tree)
- [Installation](#installation)
  - [Adding uCosmos as a git submodule](#adding-ucosmos-as-a-git-submodule)
  - [Preparing the configuration files](#preparing-the-configuration-files)
  - [Integrating uCosmos with a new project](#integrating-ucosmos-with-a-new-project)
- [Configuration files](#configuration-files)
  - [The `_config` folder](#the-_config-folder)
  - [uCosmos configuration](#ucosmos-configuration)
  - [Component configuration](#component-configuration)
  - [Command list configuration](#command-list-configuration)
- [Task system](#task-system)
  - [Task model](#task-model)
  - [Run modes](#run-modes)
  - [Creating a task](#creating-a-task)
  - [Removing a task](#removing-a-task)
  - [Changing the period of a task](#changing-the-period-of-a-task)
  - [Helper functions](#helper-functions)
  - [Shutting down the system](#shutting-down-the-system)
  - [The system tick and the scheduler](#the-system-tick-and-the-scheduler)
- [Console and commands](#console-and-commands)
  - [Console task](#console-task)
  - [Command list and registry](#command-list-and-registry)
  - [Built-in commands](#built-in-commands)
  - [Adding a new command](#adding-a-new-command)
  - [Registering a command from a component](#registering-a-command-from-a-component)
  - [Argument parsers](#argument-parsers)
  - [Help strings and utilities](#help-strings-and-utilities)
- [Diagnostic tools](#diagnostic-tools)
- [Change log](#change-log)

## Features

- Cooperative, non-preemptive scheduler driven by a single hardware timer.
- Periodic tasks with configurable periods, expressed as multiples of the system tick.
- Task lifecycle callbacks: constructor, destructor, run and identification.
- Command-line interpreter with line editing, backspace, escape and CTRL-Z history recall.
- Rich, type-safe set of argument parsers for unsigned/signed integers, floating point
  numbers, ASCII characters, strings, hex strings and timestamps.
- Optional diagnostic subsystem measuring per-task execution time, overruns, total
  execution time and total CPU usage.
- Portable and compact code: the hardware abstraction layer consists of a handful of
  macros, so porting to a new MCU is trivial.

## Requirements and supported platforms

- A C++17 (or newer) toolchain. The code uses `std::numeric_limits`, `std::is_unsigned_v`,
  `std::is_floating_point_v` and designated initializers.
- CMake, Make or any other build system that can compile C++ sources.
- One of the following platform SDKs:
  - **ESP-IDF** for the ESP32 family (`ESP_PLATFORM` macro must be defined).
  - **Raspberry Pi Pico SDK** for RP2040 / RP2350 / RP2354
    (`PICO_RP2040` or `PICO_RP2350` macro must be defined).
  - Any other platform, provided that you implement a repeating timer interrupt, an input
    stream (`getchar()` equivalent) and a 64-bit microsecond counter. See `task.cpp`,
    `console.cpp` and `log.h` for the platform-dependent blocks.

## Folder tree

```
your_project
│   .editorconfig
│   .gitignore
│   .gitmodules
│   CMakeLists.txt
│   main.cpp
│   terminal.py
│
├───module1
│       module1.cpp
│       module1.h
│       module1.hxx
│
├───module2
│       module2.cpp
│       module2.h
│       module2.hxx
│
├───module3
│   │   module3.cpp
│   │   module3.h
│   │   module3_demo.cpp
│   │   module3_demo.h
│   │   module3_demo.hxx
│   │
│   └───template
│           module3_config.h
│
├───uCosmos
│   │   commands.cpp
│   │   commands.h
│   │   commands.hxx
│   │   console.cpp
│   │   console.h
│   │   log.h
│   │   readme.md
│   │   task.cpp
│   │   task.h
│   │   uCosmos.cpp
│   │   uCosmos.h
│   │
│   └───template
│           uCosmos_config.h
│           uCosmos_template.h
│           _commands.h
│
└───_config
        module1.h
        module2.h
        module3.h
        uCosmos_config.h
        _commands.h
        _config.h
```

A few rules that make the structure above work:

- **`uCosmos`** (and every other module) is a **git submodule** - it is the upstream code
  and should not be modified directly.
- **`_config`** belongs to **your project**, not to the submodules. It contains the
  *effective* configuration used to build the firmware. Every module ships a `template`
  folder with config files that you copy into `_config` and then adjust to your needs.
- **`main.cpp`** is the entry point of the project and is the only place that has to
  include `_config/_config.h` before `uCosmos/uCosmos.h`.

## Installation

### Adding uCosmos as a git submodule

From the root of your project, add uCosmos as a submodule:

```bash
git submodule add <url_to_uCosmos_repository> uCosmos
git submodule update --init --recursive
```

If you are cloning an existing project that already contains `uCosmos` as a submodule,
make sure to fetch the submodules with:

```bash
git submodule update --init --recursive
```

Repeat the process for every additional module you want to use (for example
`cipher_aes128`, `cipher_sbox`, `crc`, `tools_rp`, ...).

### Preparing the configuration files

Each module keeps its default configuration in a `template` subfolder. These templates
are *not* used directly - they only serve as a starting point. Copy them into your
project's `_config` folder and adjust them:

```bash
mkdir -p _config
cp uCosmos/template/uCosmos_config.h _config/uCosmos_config.h
cp uCosmos/template/_commands.h      _config/_commands.h
```

If the module also provides a task template (`uCosmos_template.h`), it is only a
skeleton demonstrating how to write a task - copy it into your own source folder if you
want to use it as a base for a new task.

For each additional component, do the same:

```bash
cp cipher_aes128/template/cipher_aes128_config.h _config/
cp cipher_sbox/template/cipher_sbox_config.h     _config/
cp crc/template/crc_config.h                     _config/
cp tools_rp/template/tools_rp_config.h           _config/
```

### Integrating uCosmos with a new project

A minimal `main.cpp` looks like the one below. Notice that `_config/_config.h` is
included first, because it decides which modules are enabled and which platform is
selected.

```cpp
#include "_config/_config.h"
#include "uCosmos/uCosmos.h"

// Include component headers only if the corresponding component is enabled
#if COMPONENT_CIPHER_AES128
    #include "cipher_aes128/aes128_cbc_soft.h"
    #include "cipher_aes128/aes128_ecb_soft.h"
#endif

// ...

int main() {
    stdio_init_all();                 // platform-specific init
    sleep_ms(300);

    uCosmos_init();                   // start the timer, register the console

    task_add(my_task,    100);        // add your own tasks
    task_add(other_task, 500);

    uCosmos_scheduler();              // never returns unless shutdown requested

    LOGW("End of main()");
}
```

The four calls that make up the integration are:

| Function             | Responsibility                                                     |
|----------------------|--------------------------------------------------------------------|
| `uCosmos_init()`     | Initializes the task table, the system tick and the console.       |
| `task_add(...)`      | Registers tasks (each task's constructor is invoked immediately).   |
| `uCosmos_scheduler()`| Runs the endless scheduling loop until `uCosmos_deinit()` is called.|
| `uCosmos_deinit()`   | Closes every task (running their destructors) and stops the timer. |

`uCosmos_deinit()` is normally called from inside a task or from a console command
(`exit`) and causes the scheduler to return. You can then de-initialize the platform
before the device is reset or powered off.

## Configuration files

### The `_config` folder

Everything that belongs to your project lives in `_config`. There is no fixed set of
files in it - it is entirely driven by what the enabled modules need. A typical project
keeps:

| File                     | Purpose                                                       |
|--------------------------|---------------------------------------------------------------|
| `_config.h`              | Top-level header: selects platform, defines component macros, includes everything else. |
| `_commands.h`            | Declares the `command_list[]` array used by the console.      |
| `uCosmos_config.h`       | Configuration of the OS itself (`PRODUCT_DEMO` preset).       |
| `<component>_config.h`   | Configuration of each additional component.                   |

`_config/_config.h` is your main header and should be included first everywhere. It is
responsible for:

- defining the platform macro (`ESP_PLATFORM`, `PICO_RP2040`, `PICO_RP2350`, ...),
- selecting a preset (`PRODUCT_DEMO`, `PRODUCT_RELEASE`, ...),
- enabling or disabling components with `COMPONENT_*` macros,
- including every other config header (`uCosmos_config.h`, `<module>_config.h`, ...),
- providing a guard that fails the build if a required config is missing.

The template `uCosmos_config.h` uses a `PRODUCT_DEMO` preset. All the options that you
might want to adjust are listed there, grouped by functionality:

| Group             | Option                      | Description                                     |
|-------------------|-----------------------------|-------------------------------------------------|
| Task manager      | `TASK_MAXCOUNT`             | Maximum number of tasks running simultaneously. |
|                   | `TICK_PERIOD_MS`            | Base tick period in ms (1-1000).                |
|                   | `SHOW_SPLASH_SCREEN_AT_START` | Display the logo when `uCosmos_init()` runs.  |
| Console           | `CONSOLE_USE`               | Add the console task automatically.             |
|                   | `CONSOLE_TASK_PERIOD_MS`    | Period of the console task (in ms).             |
|                   | `CONSOLE_COMMAND_LENGTH`    | Size of the input buffer.                       |
|                   | `CONSOLE_MAX_ARGC`          | Maximum number of arguments in a command.       |
|                   | `CONSOLE_USE_HELP`          | Enable `CMD_HELP` support in commands.          |
|                   | `CONSOLE_USE_CTRL_Z`        | CTRL-Z recalls the previously entered command.  |
| Commands          | `USE_CMD_ALL`               | `?` command listing every registered command.   |
|                   | `USE_CMD_EXIT`              | `exit` command that shuts down the system.      |
|                   | `USE_CMD_TIME`              | `time` command printing the current time.       |
|                   | `USE_CMD_TASK_MONITOR`      | `` ` `` command showing all running tasks.      |
|                   | `USE_CMD_TASK_COMMANDS`     | `add`, `close`, `per`, `exe` commands.          |
|                   | `USE_CMD_PARSE_DEMO`        | Demonstrations of every parser.                 |
|                   | `USE_CMD_TASK_DEMO`         | Demonstrations of the task API.                 |
| Diagnostics       | `DIAG_USE`                  | Enable per-task timing diagnostics.             |
|                   | `DIAG_COUNT`                | Maximum number of tasks to be tracked.          |

### uCosmos configuration

The `uCosmos_config.h` file is split into logical sections (see table above) and ends
with an `#ifndef OS_CONFIG_DONE` block that produces a compile error if the configuration
is incomplete. When you define a new preset, copy the existing one and adjust the values,
but make sure `OS_CONFIG_DONE` is always defined at the end.

### Component configuration

Every component follows the same scheme: a `template/<component>_config.h` in the
submodule, copied into `_config/<component>_config.h` and included from
`_config/_config.h`. The component itself is only compiled when its `COMPONENT_*` macro
is enabled:

```cpp
#define COMPONENT_CIPHER_AES128  1
#define COMPONENT_CIPHER_SBOX    1
#define COMPONENT_CRC            1
#define COMPONENT_TOOLS_RP       1
```

All the `#if COMPONENT_*` guards in `main.cpp`, `_config/_commands.h` and the various
`*.hxx` files depend on these macros, so if a component is disabled its source files
and its console commands are compiled out entirely.

### Command list configuration

`_config/_commands.h` builds the single `command_list[]` array that the console
searches through. It is built by textual inclusion of per-module `.hxx` files:

```cpp
#include "_config/_config.h"
#ifndef INTERPRETER_COMMANDS_H_
#define INTERPRETER_COMMANDS_H_

#include "uCosmos/console.h"
#include "uCosmos/commands.h"

#if COMPONENT_CIPHER_AES128
    #include "cipher_aes128/aes128_demo.h"
#endif
// ...

const command_struct command_list[] = {
    #include "uCosmos/commands.hxx"
    #include "cipher_aes128/aes128_demo.hxx"
    #include "cipher_sbox/sbox_demo.hxx"
    #include "crc/crc_demo.hxx"
    #include "tools_esp32/tools_esp32.hxx"
    #include "tools_rp/tools_rp.hxx"
};

#endif
```

Because the `.hxx` files contain only `{"name", function}` initializers, adding or
removing commands is as simple as adding or removing one `#include` line here.

## Task system

### Task model

A task is a plain function with the signature:

```cpp
void my_task(run_mode_t run_mode);
```

There is no per-task stack, context or heap allocation. The scheduler just calls the
function pointer every time the task becomes due. This means tasks must be
**short-lived** and must not block for long periods. Blocking primitives such as
`sleep_ms()` are available, but they stop **the whole system**, not just the current
task, so they should be used only for demos or hardware initialization. The diagnostic
subsystem (see below) reports tasks whose execution time exceeds their period as
overruns.

The task table is fixed in size and is configured with `TASK_MAXCOUNT`. Each entry
contains:

| Field      | Description                                                        |
|------------|--------------------------------------------------------------------|
| `task_ptr` | Pointer to the task function; `nullptr` means the slot is free.    |
| `counter`  | Countdown (in ticks) until the task should be triggered.           |
| `period`   | Task period expressed in ticks (`period_ms / TICK_PERIOD_MS`).     |
| `flag`     | Set by the timer interrupt, cleared by the scheduler.              |

### Run modes

Each call of the task function passes one of the following `run_mode_t` values:

| Mode             | When it is invoked                                       |
|------------------|-----------------------------------------------------------|
| `os_run`         | Every time the task's period elapses.                     |
| `os_constructor` | Right after `task_add()` registers the task.              |
| `os_destructor`  | Right after `task_close()` unregisters the task.          |
| `os_id`          | Only from `task_get_name()` to let the task report its name. |

Tasks must always handle all four modes, even if only `os_run` contains real work. A
canonical skeleton is shipped in `uCosmos/template/uCosmos_template.h`:

```cpp
void template_task(run_mode_t run_mode) {
    switch(run_mode) {
        case os_run:         /* periodic work */          break;
        case os_constructor: /* one-time initialization */ break;
        case os_destructor:  /* one-time cleanup */        break;
        case os_id:          task_name = __func__;         break;
    }
}
```

The `os_id` case sets the global `task_name` pointer to a string stored in flash
memory. This is how the task monitor and diagnostic output know the name of a task.
Never store a dynamically allocated string there.

### Creating a task

```cpp
os_t result = task_add(my_task, 100);   // 100 ms period
```

- `period_ms` must be greater than or equal to `TICK_PERIOD_MS`.
- `task_add` fails with `os_nullptr` if the pointer is null,
  `os_task_already_created` if the same function is already registered,
  `os_task_period_under_range` if the period is too small,
  and `os_no_free_slot` if no free slot exists.
- On success, `os_constructor` is invoked immediately (from `task_add`).

The scheduler does not call a task twice if the previous run has not finished - it only
clears the `flag` when the task starts and re-sets it from the timer interrupt if the
next period has already elapsed (this is how overruns are produced).

### Removing a task

```cpp
os_t result = task_close(my_task);
```

`task_close` clears the corresponding table entry, then calls `my_task(os_destructor)`.
Use the destructor callback to release resources that were acquired by the constructor.
A task can also be closed from within itself - for example, a task that runs only once
can call `task_close(my_task)` at the end of its `os_run` branch.

### Changing the period of a task

```cpp
os_t result = task_period_change(my_task, 500);
```

The period is changed on the fly, the counter is reset and the flag is cleared, so the
new period takes effect immediately. The same range checks as `task_add` apply.

### Helper functions

| Function                                    | Description                                                     |
|---------------------------------------------|-----------------------------------------------------------------|
| `bool task_is_running(void (*task_ptr)(...))` | `true` if the task is registered (not "currently executing").   |
| `const char * task_get_name(void (*task_ptr)(...))` | Returns the string set by the `os_id` branch. Not re-entrant. |
| `uint64_t get_time_us(void)`                | Monotonic time in microseconds.                                 |

`ExecutionTimer` (defined in `task.h`) is a convenience class for measuring elapsed
time around a block of code:

```cpp
{
    ExecutionTimer timer;
    do_something_heavy();
    timer.print("do_something_heavy");   // prints "do_something_heavy: 12.345 ms"
}
```

### Shutting down the system

```cpp
uCosmos_deinit();          // equivalent to task_deinit()
```

Calling `task_deinit()` sets the `exit_request` flag. On its next pass through the
scheduler loop, every registered task is closed (running its destructor) and then the
system timer is stopped, so `uCosmos_scheduler()` returns to `main()`. This is what the
built-in `exit` command does.

### The system tick and the scheduler

The system is driven by a hardware timer with a fixed period of `TICK_PERIOD_MS`. On
every tick:

1. For each task, the counter is decremented. When it reaches zero, the `flag` is set
   and the counter is reloaded with `period - 1`.
2. When the scheduler's main loop notices a set `flag`, it calls `task_ptr(os_run)` and
   clears the flag. If the next period elapses before the task returns, the flag is set
   again, producing a missed-run (overrun) - the diagnostic subsystem reports it.

The scheduler loop never sleeps longer than one tick: on ESP32 it uses `vTaskDelay(1)`,
on RP2040/RP2350 it executes `wfi` after `tight_loop_contents()`, so the CPU idles
when there is no work.

## Console and commands

### Console task

When `CONSOLE_USE` is enabled, `console_init()` registers `console_task` with a period
of `CONSOLE_TASK_PERIOD_MS` (default `20`). On every execution the task drains the input
stream one character at a time:

- Printable characters are appended to the input buffer.
- Backspace removes the last character.
- Escape clears the whole line.
- CTRL-Z (if `CONSOLE_USE_CTRL_Z`) recalls the previously entered command.
- Enter parses the buffer into `argc/argv` and executes the matching command.

The parser splits the buffer on spaces, but quoted sections (`"..."`) are kept as a
single argument. A maximum of `CONSOLE_MAX_ARGC` arguments (including the command name)
is allowed.

### Command list and registry

The console searches for the command in the `command_list[]` array that is defined in
`_config/_commands.h`. Each entry is:

```cpp
{ "name", function_pointer }
```

Entries live in per-module `.hxx` files that are `#include`d between the braces of the
array. A `.hxx` file contains only `{"name", ptr},` lines - no `#include` guards, no
declarations. The matching declarations belong in the corresponding `.h` file.

### Built-in commands

The following commands are provided by the uCosmos module itself. Every command is
enabled by one of the `USE_CMD_*` macros in `uCosmos_config.h`.

| Command  | Enabled by              | Description                                                    |
|----------|--------------------------|----------------------------------------------------------------|
| `?`      | `USE_CMD_ALL`            | List every registered command with its address.                |
| `exit`   | `USE_CMD_EXIT`           | Shut down the system (`task_deinit()`).                        |
| `time`   | `USE_CMD_TIME`           | Print the current system time in `YYYY-MM-DD hh:mm:ss`.        |
| `` ` ``  | `USE_CMD_TASK_MONITOR`   | Show all running tasks (period, name) plus diagnostics.        |
| `add`    | `USE_CMD_TASK_COMMANDS`  | `add <ptr:hex32> <period:dec16>` - register a task.            |
| `close`  | `USE_CMD_TASK_COMMANDS`  | `close <ptr:hex32>` - unregister a task.                       |
| `per`    | `USE_CMD_TASK_COMMANDS`  | `per <ptr:hex32> <period:dec16>` - change a task's period.     |
| `exe`    | `USE_CMD_TASK_COMMANDS`  | `exe <ptr:hex32>` - execute a task's `os_run` immediately.     |
| `demo1_add` / `demo2_add` | `USE_CMD_TASK_DEMO` | Add the demo tasks with a given period.              |
| `demo1_cls` / `demo2_cls` | `USE_CMD_TASK_DEMO` | Close the demo tasks.                                |
| `args`   | `USE_CMD_PARSE_DEMO`     | Dump the raw `argc`/`argv` contents.                           |
| `echo`   | `USE_CMD_PARSE_DEMO`     | Echo the first argument back.                                  |
| `dec8/16/32/64` | `USE_CMD_PARSE_DEMO` | Demo of `parse_dec()` for the corresponding width.          |
| `hex8/16/32/64` | `USE_CMD_PARSE_DEMO` | Demo of `parse_hex()`.                                       |
| `bin8/16/32/64` | `USE_CMD_PARSE_DEMO` | Demo of `parse_bin()`.                                       |
| `int8/16/32/64` | `USE_CMD_PARSE_DEMO` | Demo of `parse_int()`.                                       |
| `float` / `double` | `USE_CMD_PARSE_DEMO` | Demo of `parse_float()`.                                    |
| `ascchr` / `ascstr` | `USE_CMD_PARSE_DEMO` | Demo of ASCII parsers.                                      |
| `hexstr` / `timestr` | `USE_CMD_PARSE_DEMO` | Demo of hex-string and time parsers.                        |
| `wrong1..4` | `USE_CMD_PARSE_DEMO` | Deliberately broken parser calls to demonstrate error output. |
| `` `` ``  | `DIAG_USE`               | Clear the diagnostic counters.                                 |

The command arguments are passed as strings, so the command usually starts with one
`CMD_HELP(...)` line describing the expected arguments, followed by calls to parsers.
For example, the `add` command is implemented as:

```cpp
void task_add_cmd(int argc, char * argv[]) {
    CMD_HELP("ptr:hex32 period:dec16");

    uint32_t ptr;
    if(parse_uint(argv[1], &ptr, 16)) return;          // default max = UINT32_MAX
    void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);

    uint16_t period;
    if(parse_uint(argv[2], &period)) return;            // default base  = 10

    task_add(task_ptr, period);
}
```

### Adding a new command

To add a new command inside your project (not inside a submodule) follow the same
pattern that the components use:

1. **Create the implementation** (`myproject/my_cmds.cpp`):

   ```cpp
   #include "_config/_config.h"
   #include "uCosmos/console.h"
   #include "uCosmos/commands.h"
   #include "myproject/my_cmds.h"

   void led_cmd(int argc, char * argv[]) {
       CMD_HELP("on|off");

       if(strcmp(argv[1], "on") == 0) {
           gpio_put(LED_PIN, 1);
           print_ok();
       }
       else if(strcmp(argv[1], "off") == 0) {
           gpio_put(LED_PIN, 0);
           print_ok();
       }
       else {
           printf("expected \"on\" or \"off\"\n");
       }
   }
   ```

2. **Declare it** (`myproject/my_cmds.h`):

   ```cpp
   #ifndef MY_CMDS_H_
   #define MY_CMDS_H_
   void led_cmd(int argc, char * argv[]);
   #endif
   ```

3. **Register it** (`myproject/my_cmds.hxx`):

   ```cpp
   {"led", led_cmd},
   ```

4. **Wire it up** in `_config/_commands.h`:

   ```cpp
   #include "myproject/my_cmds.h"

   const command_struct command_list[] = {
       #include "uCosmos/commands.hxx"
       #include "myproject/my_cmds.hxx"
   };
   ```

5. Make sure `myproject/my_cmds.cpp` is added to your build system.

### Registering a command from a component

The pattern described above is exactly what every component already does. A submodule
that wants to add commands provides three files:

- `<name>_demo.cpp` (implementation),
- `<name>_demo.h` (declarations),
- `<name>_demo.hxx` (the `{"name", ptr},` entries),

and the project's `_config/_commands.h` includes the `.hxx` file inside the
`command_list[]` array. This keeps the commands close to the code they exercise, while
still allowing the project to opt-in.

### Argument parsers

All parsers live in `uCosmos/console.h` and return the `parse_res` enum. A value of
`parse_ok` (which is `0`) means success, anything else is an error and the parser has
already printed a coloured message to the console. For this reason commands typically
use the `if(parse_xxx(...)) return;` idiom.

| Function                                                                  | Notes                                                                 |
|---------------------------------------------------------------------------|-----------------------------------------------------------------------|
| `parse_uint<T>(arg, &out, base = 10, max = MAX)`                          | Template. `<T>` must be an unsigned integer. `'` is accepted as a digit separator. |
| `parse_int<T>(arg, &out)`                                                 | Template. `<T>` must be a signed integer. Accepts `+`/`-` signs.      |
| `parse_float<T>(arg, &out)`                                               | Template. Accepts `nan`, `inf`, `+`/`-`, `.`/`,`, fractional part and `e`/`E` exponent. |
| `parse_dec(arg, &out, max = MAX)`                                         | Overloads for `uint8_t/16/32/64`. Uses base 10.                       |
| `parse_hex(arg, &out, max = MAX)`                                         | Overloads. Uses base 16.                                              |
| `parse_bin(arg, &out, max = MAX)`                                         | Overloads. Uses base 2.                                               |
| `parse_ascii_char(arg, &out)`                                             | First character of the argument.                                      |
| `parse_ascii_string(arg, out, &len, buffer_size, min_len = 0)`            | Null-terminated copy with `length` (including the terminator).        |
| `parse_hex_string(arg, out, &len, buffer_size, min_len = 0)`              | Parses `"DEADBEEF"` or `"DE AD BE EF"` into bytes.                   |
| `parse_time(arg, &time_t)`                                                | Parses `YYMMDDhhmmss` into a `time_t`, validating calendar limits.    |

Return codes are one of: `parse_ok`, `parse_missing_argument`, `parse_over_range`,
`parse_under_range`, `parse_error`, `parse_expected_bin`, `parse_expected_dec`,
`parse_expected_hex`, `parse_expected_float`, `parse_wrong_base`, `parse_wrong_time`,
`parse_nullptr`. The `debug()` helper (called inside each parser) turns the code into a
human-readable message and colourises it in red.

### Help strings and utilities

- `CMD_HELP("arg1:type arg2:type")` - if `CONSOLE_USE_HELP` is set and the command is
  called without arguments (`argc == 1`), prints the string and returns. Otherwise it
  compiles to `((void)0)`.
- `print_ok()` - prints a green `OK` and new line.
- `print_hexstr(array, len, new_line = false, separator = 0)` - prints a byte array in
  upper-case hex, optionally separated by a chosen character.
- `print_hexstr_demo` - see `commands.cpp` for the different calling conventions.

## Diagnostic tools

When `DIAG_USE` is set to `true`, the scheduler measures the execution time of every
task using `get_time_us()` and stores the result in a dedicated diagnostic table of
`DIAG_COUNT` entries. For each task it tracks:

| Field           | Meaning                                                          |
|-----------------|------------------------------------------------------------------|
| `run_count`     | Number of completed executions.                                  |
| `overrun_count` | Number of executions whose duration exceeded the task's period.  |
| `time_total`    | Total time spent inside the task (µs).                           |
| `time_max`      | Worst-case execution time (µs).                                  |
| `time_avg`      | `time_total / run_count`.                                        |

Two console commands expose the table:

- `` ` `` (single backtick, `USE_CMD_TASK_MONITOR`) - prints a compact list of every
  registered task (slot, function pointer, period and name), followed by the diagnostic
  table (sorted by total time, if `DIAG_BUBBLE_SORT_BEFORE_PRINT` or
  `DIAG_INSERTION_SORT_BEFORE_PRINT` is enabled), measurement duration and total CPU
  usage.
- `` `` `` (double backtick, `DIAG_USE`) - resets every counter and the measurement
  start time.

Typical output looks like this:

```
 No  Run count   Overruns            Total [ms]              Max [ms]              Avg [ms]  Usage [%]  Name
  0       8500          0             1.234                    0.150                    0.145     1.234  console_task
  1       1700        120            25.400                   50.100                   14.941    12.700  sleep_500ms_task
...
Total CPU usage:  88.812345 %
```

`DIAG_COUNT` must be at least as large as the number of tasks you want to inspect.
Tasks that exceed the table are silently ignored - enable debug logs if you want to
notice that.

The same mechanism also underlies `ExecutionTimer`, which can be used to time
arbitrary blocks of code outside the scheduler - see the `task.h` class for details.

## Change log

### 5.3.0 - 2026.11.
* Added `CMD_HELP` macro to facilitate the creation of user guides for console commands.

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