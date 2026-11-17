#ifndef UCOSMOS_CONSOLE_H_
#define UCOSMOS_CONSOLE_H_

#include "../_config/_config.h"
#include "../_config/uCosmos_config.h"
#include <time.h>
#include <limits>
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
	parse_expected_float,
	parse_wrong_base,
	parse_wrong_time,
	parse_nullptr,
};

// Argument parsers
template<typename T> parse_res parse_uint(const char * arg, T * output, uint8_t base = 10, T max = std::numeric_limits<T>::max());
template<typename T> parse_res parse_int(const char * arg, T * output);
template<typename T> parse_res parse_float(const char * arg, T * output);

parse_res parse_dec(const char * arg, uint8_t  * output, uint8_t  max=UINT8_MAX);
parse_res parse_dec(const char * arg, uint16_t * output, uint16_t max=UINT16_MAX);
parse_res parse_dec(const char * arg, uint32_t * output, uint32_t max=UINT32_MAX);
parse_res parse_dec(const char * arg, uint64_t * output, uint64_t max=UINT64_MAX);

parse_res parse_hex(const char * arg, uint8_t  * output, uint8_t  max=UINT8_MAX);
parse_res parse_hex(const char * arg, uint16_t * output, uint16_t max=UINT16_MAX);
parse_res parse_hex(const char * arg, uint32_t * output, uint32_t max=UINT32_MAX);
parse_res parse_hex(const char * arg, uint64_t * output, uint64_t max=UINT64_MAX);

parse_res parse_bin(const char * arg, uint8_t  * output, uint8_t  max=UINT8_MAX);
parse_res parse_bin(const char * arg, uint16_t * output, uint16_t max=UINT16_MAX);
parse_res parse_bin(const char * arg, uint32_t * output, uint32_t max=UINT32_MAX);
parse_res parse_bin(const char * arg, uint64_t * output, uint64_t max=UINT64_MAX);

parse_res parse_ascii_char(const char * arg, char * output);
parse_res parse_ascii_string(const char * arg, char * output, size_t * output_length, const size_t buffer_size, const size_t min_length = 0);
parse_res parse_hex_string(const char * arg, uint8_t * output, size_t * output_length, const size_t buffer_size, const size_t min_length = 0);
parse_res parse_time(const char * arg, time_t * output);

void print_ok(void);
void print_hexstr(const uint8_t array[], size_t len, bool new_line = false, char separator = 0);

#if CONSOLE_USE_HELP
	#define CMD_HELP(text)          \
		if(argc == 1) {             \
			printf(text "\n");      \
			return;                 \
		}
#else
	#define CMD_HELP(text) ((void)0)
#endif

// Demo commands
#if USE_CMD_ALL
	void all_commands_cmd(int argc, char * argv[]);
#endif

#endif
