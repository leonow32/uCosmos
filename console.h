#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <time.h>
#include "task.h"

// Struct used to build table of commands and pointers to specified functions
struct command_struct {
	const char *name;
	void (*ptr)(int argc, char * argv[]);
};

// Command list
extern const command_struct	command_list[];

// Command line interpreter
void console_init(void);
void console_task(run_mode_t run_mode);

// Result type for argument parser
enum parse_res {
	parse_ok = 0,
	parse_missing_argument,
	parse_over_range,
	parse_under_range,
	parse_error,
	parse_expected_bin,
	parse_expected_dec,
	parse_expected_hex,
	parse_wrong_base,
	parse_wrong_time,
	parse_nullptr,
};

// Argument parsers
template<typename T> parse_res parse_uint(const char * arg, T * output, uint8_t base = 10);
template<typename T> parse_res parse_int(const char * arg, T * output);
parse_res parse_ascii_char(const char * arg, char * output);
parse_res parse_ascii_string(const char * arg, char * output, size_t * output_length, const size_t buffer_size, const size_t min_length = 0);
parse_res parse_hex_string(const char * arg, uint8_t * output, size_t * output_length, const size_t buffer_size, const size_t min_length = 0);
parse_res parse_time(const char * arg, time_t * output);

void print_ok(void);

// Demo commands
#if USE_CMD_ALL
	void all_commands_cmd(int argc, char * argv[]);
#endif

#endif
