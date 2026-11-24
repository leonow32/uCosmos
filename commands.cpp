#include "../_config/_config.h"

#include <string.h>
#include "commands.h"
#include "console.h"
#include "log.h"

#if USE_CMD_EXIT
void exit_cmd(int argc, char * argv[]) {
	task_deinit();
}
#endif

// ========================================
// Task control commands (add, close, kill)
// ========================================

#if USE_CMD_TASK_COMMANDS
	
	// Add new task with requested period and phase shift
	void task_add_cmd(int argc, char * argv[]) {
		CMD_HELP("ptr:hex32 period:dec16");
		
		// Argument 1 - task ptr
		uint32_t ptr;
		if(parse_uint(argv[1], &ptr, 16)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Argument 2 - period
		uint16_t period;
		if(parse_uint(argv[2], &period)) return;
		
		// Execute command
		task_add(task_ptr, period);
	}
	
	// Close task with its destructor
	void task_remove_cmd(int argc, char * argv[]) {
		CMD_HELP("ptr:hex32");
		
		// Argument 1 - task ptr
		uint32_t ptr;
		if(parse_uint(argv[1], &ptr, 16)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Execute command
		task_remove(task_ptr);
	}
	
	// Change task period
	void task_period_change_cmd(int argc, char * argv[]) {
		CMD_HELP("ptr:hex32 period:dec16");
		
		// Argument 1 - task ptr
		uint32_t ptr;
		if(parse_uint(argv[1], &ptr, 16)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Argument 2 - period
		uint16_t period;
		if(parse_uint(argv[2], &period)) return;
		
		// Execute command
		task_period_change(task_ptr, period);
	}

	// Execute task manually
	void task_exe_cmd(int argc, char * argv[]) {
		CMD_HELP("ptr:hex32");
		
		// Argument 1 - task ptr
		uint32_t ptr;
		if(parse_uint(argv[1], &ptr, 16)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Execute command
		task_ptr(os_run);
	}
#endif

// ========================================
// Time
// ========================================

// Print system time in YYYY-MM-DD hh:mm:ss format
#if USE_CMD_TIME
	void time_print_cmd(int argc, char * argv[]) {
		time_t time_now = 0;
		time(&time_now);

		tm time_struct;
		localtime_r(&time_now, &time_struct);
		
		printf("%04u-%02u-%02u %02u:%02u:%02u\n",
			time_struct.tm_year + 1900,
			time_struct.tm_mon + 1,
			time_struct.tm_mday,
			time_struct.tm_hour,
			time_struct.tm_min,
			time_struct.tm_sec
		);
	}
#endif

// ========================================
// Demo tasks
// ========================================

#if USE_CMD_TASK_DEMO
	void demo1_task(run_mode_t run_mode) {
		static int counter = 0;

		switch(run_mode) {
			case os_run: {
				LOGI("%s %d", __func__, counter++);
				break;
			}

			case os_constructor: {
				LOGI("%s constructor", __func__);
				break;
			}

			case os_destructor: {
				LOGI("%s destructor", __func__);
				break;
			}

			case os_id: {
				task_name = __func__;
				break;
			}
		}
	}

	void demo2_task(run_mode_t run_mode) {
		static int counter = 0;

		if(run_mode == os_run) {
			LOGI("%s %d", __func__, counter++);
		}
		else if(run_mode == os_constructor) {
			LOGI("%s constructor", __func__);
		}
		else if(run_mode == os_destructor) {
			LOGI("%s destructor", __func__);
		}
		else if(run_mode == os_id) {
			task_name = __func__;
		}
	}

	void demo1_add_cmd(int argc, char * argv[]) {
		CMD_HELP("period:dec16");
		
		uint16_t period;
		if(parse_uint(argv[1], &period)) return;
		
		task_add(demo1_task, period);
	}

	void demo2_add_cmd(int argc, char * argv[]) {
		CMD_HELP("period:dec16");
		
		uint16_t period;
		if(parse_uint(argv[1], &period)) return;
		
		task_add(demo2_task, period);
	}

	void demo1_remove_cmd(int argc, char * argv[]) {
		task_remove(demo1_task);
	}

	void demo2_remove_cmd(int argc, char * argv[]) {
		task_remove(demo2_task);
	}
#endif

#if USE_CMD_PARSE_DEMO

void args_cmd(int argc, char * argv[]) {
	printf("argc = %u\n", argc);
	printf(TEXT_WHITE_BRIGHT "No\tPtr\t\tLen\tVal" FORMAT_RESET "\n");
	
	for(uint8_t i=0; i<CONSOLE_MAX_ARGC; i++) {
		printf("%u\t%08lX\t%u\t\"%s\"\n", 
			i, 
			uint32_t(argv[i]), 
			argv[i] ? strlen(argv[i]) : 0, 
			argv[i] ? argv[i] : "nullptr"
		);
	}
}

void echo_cmd(int argc, char * argv[]) {
	if(argc != 1) {
		printf("%s", argv[1]);
	}
	printf("\n");
}

#define GENERATE_UINT_CMD(command_name, var_type, print_format, parse_function) \
	void command_name(int argc, char *argv[]) {                                 \
		CMD_HELP("value:dec/hex/bin (max:dec)");                                \
		var_type max = std::numeric_limits<var_type>::max();                    \
		                                                                        \
		if(argc == 3) {                                                         \
			if(parse_dec(argv[2], &max)) return;                                \
		}                                                                       \
		                                                                        \
		var_type value;                                                         \
		if(parse_function(argv[1], &value, max)) return;                        \
		                                                                        \
		printf("Result: " print_format "\n", value);                            \
	}

GENERATE_UINT_CMD(dec8_cmd,  uint8_t,  "%u",   parse_dec)
GENERATE_UINT_CMD(dec16_cmd, uint16_t, "%u",   parse_dec)
GENERATE_UINT_CMD(dec32_cmd, uint32_t, "%lu",  parse_dec)
GENERATE_UINT_CMD(dec64_cmd, uint64_t, "%llu", parse_dec)

GENERATE_UINT_CMD(hex8_cmd,  uint8_t,  "%u",   parse_hex)
GENERATE_UINT_CMD(hex16_cmd, uint16_t, "%u",   parse_hex)
GENERATE_UINT_CMD(hex32_cmd, uint32_t, "%lu",  parse_hex)
GENERATE_UINT_CMD(hex64_cmd, uint64_t, "%llu", parse_hex)

GENERATE_UINT_CMD(bin8_cmd,  uint8_t,  "%u",   parse_bin)
GENERATE_UINT_CMD(bin16_cmd, uint16_t, "%u",   parse_bin)
GENERATE_UINT_CMD(bin32_cmd, uint32_t, "%lu",  parse_bin)
GENERATE_UINT_CMD(bin64_cmd, uint64_t, "%llu", parse_bin)

#define GENERATE_INT_CMD(command_name, var_type, print_format) \
	void command_name(int argc, char * argv[]) {               \
		CMD_HELP("value:dec");                                 \
		var_type value;                                        \
		if(parse_int(argv[1], &value)) return;                 \
		printf(print_format "\n", value);                      \
	}                                                          \

GENERATE_INT_CMD(int8_cmd,  int8_t,  "%d");
GENERATE_INT_CMD(int16_cmd, int16_t, "%d");
GENERATE_INT_CMD(int32_cmd, int32_t, "%ld");
GENERATE_INT_CMD(int64_cmd, int64_t, "%lld");

#define GENERATE_FLOAT_CMD(command_name, var_type) \
	void command_name(int argc, char * argv[]) {   \
		CMD_HELP("value:float");                   \
		var_type value = 0;                        \
		if(parse_float(argv[1], &value)) return;   \
		printf("%f\n",     value);                 \
		printf("%.20f\n",  value);                 \
		printf("%1.20e\n", value);                 \
	}

GENERATE_FLOAT_CMD(float_cmd,  float);
GENERATE_FLOAT_CMD(double_cmd, double);

void ascchr_cmd(int argc, char * argv[]) {
	CMD_HELP("char:hexstr");
	char chr;
	if(parse_ascii_char(argv[1], &chr)) return;
	printf("%c\n", chr);
}

void ascstr_cmd(int argc, char * argv[]) {
	CMD_HELP("array:ascii");

	char buffer[16] = {0};
	size_t length;
	if(parse_ascii_string(argv[1], buffer, &length, sizeof(buffer), 0)) return;
	printf("length: %u\n", length);
	printf("ASC: %s\n", buffer);
	printf("HEX: ");
	print_hexstr((const uint8_t*)buffer, length, true, ' ');
}

void hexstr_cmd(int argc, char * argv[]) {
	CMD_HELP("array:hexstr");

	uint8_t buffer[16];
	size_t length;
	if(parse_hex_string(argv[1], buffer, &length, sizeof(buffer))) return;
	printf("length: %u\n", length);
	printf("HEX: ");
	print_hexstr(buffer, length, true, ' ');
}

void timestr_cmd(int argc, char * argv[]) {
	CMD_HELP("time:YYMMDDhhmmss");

	time_t time_seconds;
	if(parse_time(argv[1], &time_seconds)) return;

	tm time_struct;
	localtime_r(&time_seconds, &time_struct);
	
	printf("%04u-%02u-%02u %02u:%02u:%02u\n",
		time_struct.tm_year + 1900,
		time_struct.tm_mon + 1,
		time_struct.tm_mday,
		time_struct.tm_hour,
		time_struct.tm_min,
		time_struct.tm_sec
	);
}

void wrong1_cmd(int argc, char * argv[]) {
	uint8_t value = 123;
	if(parse_uint("10", &value)) return;
	printf("val: %u\n", value);
}

void wrong2_cmd(int argc, char * argv[]) {
	uint8_t * value = nullptr;
	if(parse_uint("10", value)) return;
}

void wrong3_cmd(int argc, char * argv[]) {
	uint8_t value = 123;
	if(parse_uint("", &value)) return;
	printf("val: %u\n", value);
}

void wrong4_cmd(int argc, char * argv[]) {
	uint8_t value = 123;
	if(parse_uint(nullptr, &value)) return;
	printf("val: %u\n", value);
}

#endif

#if USE_CMD_COLORS
void colors_cmd(int argc, char * argv[]) {
	printf("Formatting:\n");
	printf("- "                   "Default"		FORMAT_RESET "\n");
	printf("- " FORMAT_BOLD       "Bold"		FORMAT_RESET "\n");
	printf("- " FORMAT_FAINT      "Faint"		FORMAT_RESET "\n");
	printf("- " FORMAT_ITALIC     "Italic"		FORMAT_RESET "\n");
	printf("- " FORMAT_UNDERLINE  "Underline"	FORMAT_RESET "\n");
	printf("- " FORMAT_BLINK_SLOW "Blink slow"	FORMAT_RESET "\n");
	printf("- " FORMAT_BLINK_FAST "Blink fast"	FORMAT_RESET "\n");

	printf("Text color:\n");
	printf("- Black   "	TEXT_BLACK		"abcdefghijklmnopqrstuvwxyz "	TEXT_BLACK_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Red     "	TEXT_RED		"abcdefghijklmnopqrstuvwxyz "	TEXT_RED_BRIGHT			"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Yellow  "	TEXT_YELLOW		"abcdefghijklmnopqrstuvwxyz "	TEXT_YELLOW_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Green   "	TEXT_GREEN		"abcdefghijklmnopqrstuvwxyz "	TEXT_GREEN_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Cyan    "	TEXT_CYAN		"abcdefghijklmnopqrstuvwxyz "	TEXT_CYAN_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Blue    "	TEXT_BLUE		"abcdefghijklmnopqrstuvwxyz "	TEXT_BLUE_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Magenta "	TEXT_MAGENTA	"abcdefghijklmnopqrstuvwxyz "	TEXT_MAGENTA_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- White   "	TEXT_WHITE		"abcdefghijklmnopqrstuvwxyz "	TEXT_WHITE_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");

	printf("Background color:\n");
	printf("- Black   "	BACK_BLACK		"abcdefghijklmnopqrstuvwxyz "	BACK_BLACK_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Red     "	BACK_RED		"abcdefghijklmnopqrstuvwxyz "	BACK_RED_BRIGHT			"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Yellow  "	BACK_YELLOW		"abcdefghijklmnopqrstuvwxyz "	BACK_YELLOW_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Green   "	BACK_GREEN		"abcdefghijklmnopqrstuvwxyz "	BACK_GREEN_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Cyan    "	BACK_CYAN		"abcdefghijklmnopqrstuvwxyz "	BACK_CYAN_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Blue    "	BACK_BLUE		"abcdefghijklmnopqrstuvwxyz "	BACK_BLUE_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- Magenta "	BACK_MAGENTA	"abcdefghijklmnopqrstuvwxyz "	BACK_MAGENTA_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
	printf("- White   "	BACK_WHITE		"abcdefghijklmnopqrstuvwxyz "	BACK_WHITE_BRIGHT		"abcdefghijklmnopqrstuvwxyz" FORMAT_RESET "\n");
}
#endif
