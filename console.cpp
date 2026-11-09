#include "../_config/_config.h"
#include <cmath>
#include <iostream>
#include <string.h>
#include "log.h"
#include "console.h"
#include "../_config/_commands.h"

static size_t	received_cnt;																// Number of characters in receive buffer
static char 	buffer[CONSOLE_COMMAND_LENGTH];												// Buffer for currently processed command
#if CONSOLE_USE_CTRL_Z
	static char buffer2[CONSOLE_COMMAND_LENGTH];											// Buffer for last processed command, recalled with CTRL-Z
#endif

void console_init(void) {
	LOGI("init");
	task_add(console_task, CONSOLE_TASK_PERIOD_MS);
}

static void prompt_show(void) {
	printf("\n" TEXT_YELLOW_BRIGHT FORMAT_BOLD "/> %s", buffer);
}

void console_task(run_mode_t run_mode) {
	if(run_mode == os_run) {
		while(1) {																			// Process all the characters in the input stream

			#if ESP_PLATFORM				
				int chr = getchar();														// Read a character from console input
				if(chr == EOF) {
					return;
				}
			#elif PICO_RP2040 || PICO_RP2350
				int chr = getchar_timeout_us(0);											// Read a character from console input
				if(chr == PICO_ERROR_TIMEOUT) {
					return;
				}
			#else
				#error "Platform not supported"
			#endif

			if(chr == ESCAPE) {																// If the received char is ESCAPE - clear the buffer and display the prompt
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));
				prompt_show();
			}

			#if CONSOLE_USE_CTRL_Z
			else if(chr == CTRL_Z) {														// If the received char is CTRL-Z - clear the buffer, display the prompt and previously entered command
				received_cnt = 0;
				memcpy(buffer, buffer2, CONSOLE_COMMAND_LENGTH);
				received_cnt = strlen(buffer);
				prompt_show();
			}
			#endif

			else if(chr == BACKSPACE1 || chr == BACKSPACE2) {								// If the received char is BACKSPACE - clear the buffer, display prompt and previously entered command
				if(received_cnt) {															// If any character has been received
					buffer[--received_cnt] = 0;												// Remove last character from the buffer and decrement the count
					printf("\b \b");														// Print backspace to the console so the user can see that the last character is erased
				}
			}
			
			else if(chr >= ' ' && chr <= '~') {												// If the received char is any printable character - add it to the buffer if possible or emit sound
				if(received_cnt < CONSOLE_COMMAND_LENGTH-1) {								// If there is space in the receive buffer
					if(received_cnt == 0) {													// If the character is the first one...
						prompt_show();														// ...then display the prompt to make sure it will be printed in the new line
					}
					buffer[received_cnt++] = chr;											// Add the character to the buffer and increment the counter
					printf("%c", chr);														// Print the received character so the user can see it
				}
				else {
					printf("%c", BEL);														// Emit sound signal in case the buffer is out of space
				}
			}
				
			else if((received_cnt > 0) && (chr == CR || chr == LF)) {						// If the received char is ENTER and the buffer is not empty - try to execute the command				
				#if CONSOLE_USE_CTRL_Z
				memcpy(buffer2, buffer, CONSOLE_COMMAND_LENGTH);							// Copy the buffer to CTRL-Z buffer
				#endif
				
				// Create argv array
				int	   argc = 0;															// The count of received arguments
				char * argv[CONSOLE_MAX_ARGC] = {0};										// Array of pointers to the first character of each argument
				char * char_ptr = buffer;													// Pointer to the currently processed character from the buffer
				bool   find_new_argument = true;											// Look for first char of the another argument
				char   argument_terminator;													// What character terminates the argument: space or "

				while(*char_ptr) {															// Loop as long the character is not NUL

					if(find_new_argument) {
						if(*char_ptr == ' ') {												// Skip all spaces
							char_ptr++;
						}
						else {
							if(argc >= CONSOLE_MAX_ARGC) {									// Check if there is free space in argv array
								printf("\n" TEXT_RED "Too many args" FORMAT_RESET "\n");
								argv[0] = nullptr;											// Clear 0th argument (command name) to prevent it from execution
								break;
							}

							if(*char_ptr == '"') {											// If the new argument is "string"
								argv[argc++] = ++char_ptr;
								argument_terminator = '"';
							}
							else {															// If the argument is a value without spaces inside
								argv[argc++] = char_ptr++;
								argument_terminator = ' ';
							}

							find_new_argument = false;
						}
					}

					else {
						if(*char_ptr == argument_terminator) {								// If this character is the last one...
							*char_ptr++ = NUL;												// ...change it to NUL to make proper C-string
							find_new_argument = true;
						}
						else {
							char_ptr++;														// Check another one
						}
					}
				}

				if(argv[0]) {																// If at least one argument is given (command name)
					void (*command_ptr)(int argc, char * argv[]) = nullptr;					// The command pointer

					for(int i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {	// Iterate through command list
						if(strcmp(argv[0], command_list[i].name) == 0) {					// if argv[0] == command_list[i].name
							command_ptr = command_list[i].ptr;								// Assign the command pointer
							break;															// Break iterating
						}
					}

					if(command_ptr) {														// If any command pointer has been found
						printf("\n" FORMAT_RESET);											// New line and default text formatting
						command_ptr(argc, argv);											// Execute the command pointer and pass argument count and argument values
					}
					else {
						printf("\n" TEXT_RED "Unknown cmd" FORMAT_RESET "\n");				// If command not found
					}
				}
				
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));											// Clear command buffer
			}
		}
	}

	else if(run_mode == os_id) {
		task_name = __func__;
	}
}

// ========================================
// Argument parsers
// ========================================

static parse_res debug(const parse_res res, const char * arg) {
	if(res != parse_ok) {
		printf(TEXT_RED);

		if(arg != nullptr) {
			printf("Error in argument \"" TEXT_RED_BRIGHT "%s" TEXT_RED "\": ", arg);
		}

		switch(res) {
			case parse_missing_argument:	printf("missing arg");		break;
			case parse_over_range:			printf("over range");		break;
			case parse_under_range:			printf("under range");		break;
			case parse_error:				printf("parse error");		break;
			case parse_expected_bin:		printf("expected bin");		break;
			case parse_expected_dec:		printf("expected dec");		break;
			case parse_expected_hex:		printf("expected hex");		break;
			case parse_expected_float:		printf("expected float");	break;
			case parse_nullptr:				printf("nullptr");			break;
			case parse_wrong_base:			printf("wrong base");		break;
			case parse_wrong_time:			printf("wrong time");		break;
			default:						printf("?");				break;
		}

		printf(FORMAT_RESET "\n");
	}
	
	return res;
}

static parse_res parse_numeric_char(const char chr, uint8_t * output, uint8_t base) {
	uint8_t value = 0;
	
	if(chr >= '0' && chr <= '9')		value = chr - '0';
	else if(chr >= 'A' && chr <= 'F')	value = chr - 'A' + 10;
	else if(chr >= 'a' && chr <= 'f')	value = chr - 'a' + 10;
	else								return parse_error;

	if(value >= base) {
		switch(base) {
			case 2:		return parse_expected_bin;
			case 10:	return parse_expected_dec;
			case 16:	return parse_expected_hex;
			default:	return parse_error;
		}
	}

	*output = value;
	return parse_ok;
}

template<typename T> parse_res parse_uint(const char * arg, T * output, uint8_t base, T max) {
	static_assert(std::is_unsigned_v<T>, "This function works with unsigned integers only.");
	if(arg == nullptr)			return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)			return debug(parse_missing_argument, arg);
	if(output == nullptr)		return debug(parse_nullptr, arg);
	if(base < 2 || base > 16)	return debug(parse_wrong_base, arg);

	const char * arg_copy = arg;
	*output = 0;

	while(*arg) {
		if(*arg == '\'') {
			arg++;
			continue;
		}

		uint8_t char_value = 0;
		parse_res res = parse_numeric_char(*arg++, &char_value, base);
		if(res) 												return debug(res, arg_copy);
		if(*output > max / base) 								return debug(parse_over_range, arg_copy);
		if(*output == max / base && char_value > max % base)	return debug(parse_over_range, arg_copy);
		*output = *output * base + char_value;
	}

	return parse_ok;
}

template parse_res parse_uint<uint8_t>(const char *, uint8_t *, uint8_t, uint8_t);
template parse_res parse_uint<uint16_t>(const char *, uint16_t *, uint8_t, uint16_t);
template parse_res parse_uint<uint32_t>(const char *, uint32_t *, uint8_t, uint32_t);
template parse_res parse_uint<uint64_t>(const char *, uint64_t *, uint8_t, uint64_t);

parse_res parse_dec(const char * arg, uint8_t  * output, uint8_t  max)	{return parse_uint(arg, output, 10, max);}
parse_res parse_dec(const char * arg, uint16_t * output, uint16_t max)	{return parse_uint(arg, output, 10, max);}
parse_res parse_dec(const char * arg, uint32_t * output, uint32_t max)	{return parse_uint(arg, output, 10, max);}
parse_res parse_dec(const char * arg, uint64_t * output, uint64_t max)	{return parse_uint(arg, output, 10, max);}

parse_res parse_hex(const char * arg, uint8_t  * output, uint8_t  max)	{return parse_uint(arg, output, 16, max);}
parse_res parse_hex(const char * arg, uint16_t * output, uint16_t max)	{return parse_uint(arg, output, 16, max);}
parse_res parse_hex(const char * arg, uint32_t * output, uint32_t max)	{return parse_uint(arg, output, 16, max);}
parse_res parse_hex(const char * arg, uint64_t * output, uint64_t max)	{return parse_uint(arg, output, 16, max);}

parse_res parse_bin(const char * arg, uint8_t  * output, uint8_t  max)	{return parse_uint(arg, output, 2, max);}
parse_res parse_bin(const char * arg, uint16_t * output, uint16_t max)	{return parse_uint(arg, output, 2, max);}
parse_res parse_bin(const char * arg, uint32_t * output, uint32_t max)	{return parse_uint(arg, output, 2, max);}
parse_res parse_bin(const char * arg, uint64_t * output, uint64_t max)	{return parse_uint(arg, output, 2, max);}

template<typename T> parse_res parse_int(const char * arg, T * output) {
	static_assert(std::is_signed_v<T>, "This function works with signed integers only.");
	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);

	const char * arg_copy = arg;
	bool negative = false;
	
	if(*arg == '-') {
		negative = true;
		arg++;
	} else if(*arg == '+') {
		arg++;
	}
	
	if(*arg == NUL)			return debug(parse_expected_dec, arg_copy);
	
	*output = 0;

	while(*arg) {
		if(*arg == '\'') {
			arg++;
			continue;
		}
		
		uint8_t char_value = 0;
		parse_res res = parse_numeric_char(*arg++, &char_value, 10);
		if(res) return debug(res, arg_copy);

		if(negative) {
			if(*output < (std::numeric_limits<T>::min() + char_value) / 10) return debug(parse_under_range, arg_copy);
			*output = *output * 10 - char_value;
		}
		else {
			if(*output > (std::numeric_limits<T>::max() - char_value) / 10) return debug(parse_over_range, arg_copy);
			*output = *output * 10 + char_value;
		}
	} 

	return parse_ok;
}

template parse_res parse_int<int8_t>(const char *, int8_t *);
template parse_res parse_int<int16_t>(const char *, int16_t *);
template parse_res parse_int<int32_t>(const char *, int32_t *);
template parse_res parse_int<int64_t>(const char *, int64_t *);

template<typename T> parse_res parse_float(const char * arg, T * output) {
	static_assert(std::is_floating_point_v<T>, "This function works with floating point types only.");
	if(arg == nullptr)      return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)       return debug(parse_missing_argument, arg);
	if(output == nullptr)   return debug(parse_nullptr, arg);
	
	const char *arg_copy = arg;
	bool negative = false;
	bool digit_found = false;
	*output = 0;

	// Sign
	if(*arg == '-') {
		arg++;
		negative = true;
	}
	else if(*arg == '+') {
		arg++;
	}

	// Single sign error
	if(*arg == NUL)			return debug(parse_expected_float, arg_copy);

	// Not a number
	if(strcmp(arg, "nan") == 0) {
		*output = negative ? -std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::quiet_NaN();
		return parse_ok;
	}

	// Infinity
	if(strcmp(arg, "inf") == 0) {
		*output = negative ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::infinity();
		return parse_ok;
	}

	// Integer
	while(*arg >= '0' && *arg <= '9') {
		*output = *output * 10 + *arg++ - '0';
		digit_found = true;
	}

	// Fractional
	if(*arg == '.' || *arg == ',') {
		arg++;
		T divider = 10;

		while(*arg >= '0' && *arg <= '9') {
			*output += (*arg++ - '0') / divider;
			divider *= 10;
			digit_found = true;
		}
	}

	// Single dot error
	if(digit_found == false) 		return debug(parse_expected_float, arg_copy);

	// Exponent
	if(*arg == 'e' || *arg == 'E') {
		arg++;

		T scale = 1;
		int16_t exponent = 0;
		bool exp_negative = false;

		if(*arg == '-') {
			arg++;
			exp_negative = true;
		}
		else if(*arg == '+') {
			arg++;
		}

		if(*arg < '0' || *arg > '9') 					return debug(parse_expected_float, arg_copy);

		while(*arg >= '0' && *arg <= '9') {
			uint8_t digit = *arg++ - '0';
			if(exponent > (INT16_MAX - digit) / 10)		return debug(parse_over_range, arg_copy);
			exponent = exponent * 10 + digit;
		}

		while(exponent--) {
			scale *= 10;
		}

		*output = exp_negative ? (*output / scale) : (*output * scale);
	}

	if(negative) *output = -*output;
	if(*arg != NUL) 				return debug(parse_expected_float, arg_copy);
	if(std::isinf(*output)) 		return debug(parse_over_range, arg_copy);
	return parse_ok;
}

template parse_res parse_float<float>(const char *, float *);
template parse_res parse_float<double>(const char *, double *);

parse_res parse_ascii_char(const char * arg, char * output) {
	if(arg == nullptr)				return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)				return debug(parse_missing_argument, arg);
	if(output == nullptr)			return debug(parse_nullptr, arg);
	*output = arg[0];
	return parse_ok;
}

parse_res parse_ascii_string(const char * arg, char * output, size_t * output_length, const size_t buffer_size, const size_t min_length) {
	if(arg == nullptr)				return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)				return debug(parse_missing_argument, arg);
	if(output == nullptr)			return debug(parse_nullptr, arg);
	if(output_length == nullptr)	return debug(parse_nullptr, arg);
	if(buffer_size == 0)			return debug(parse_error, arg);
	if(buffer_size <= min_length)	return debug(parse_error, arg);
	
	const char * arg_copy = arg;
	*output_length = 0;
	
	while(1) {
		*output = *arg;
		if(*arg++) {
			(*output_length)++;
			if(*output_length >= buffer_size) {
				*output = NUL;
				return debug(parse_over_range, arg_copy);
			}
			output++;
		}
		else {
			break;
		}
	}

	if(*output_length < min_length) return debug(parse_under_range, arg_copy);
	return parse_ok;
}

parse_res parse_hex_string(const char * arg, uint8_t * output, size_t * out_len, const size_t max_len, const size_t min_len) {
	if(arg == nullptr)				return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)				return debug(parse_missing_argument, arg);
	if(output == nullptr)			return debug(parse_nullptr, arg);
	if(out_len == nullptr)			return debug(parse_nullptr, arg);
	if(max_len < min_len)			return debug(parse_error, arg);
	
	parse_res res = parse_ok;
	const char * arg_copy = arg;
	uint8_t nibble_h;
	uint8_t nibble_l;
	*out_len = 0;

	while(*arg != 0) {
		if(*arg == ' ') {
			arg++;
			continue;
		}

		if(*out_len >= max_len) 	return debug(parse_over_range, arg_copy);

		res = parse_numeric_char(*arg++, &nibble_h, 16);
		if(res) return debug(res, arg_copy);
		
		res = parse_numeric_char(*arg++, &nibble_l, 16);
		if(res) return debug(res, arg_copy);
		
		*output++ = nibble_h << 4 | nibble_l;
		(*out_len)++;
	}

	if(*out_len < min_len) return debug(parse_under_range, arg_copy);
	return parse_ok;
}

// Parse time YYMMDDhhmmss
parse_res parse_time(const char * arg, time_t * output) {
	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);
	if(strlen(arg) != 12)	return debug(parse_error, arg);

	tm new_time = {};
	uint8_t digits[12] = {};
	parse_res res = parse_ok;

	for(uint8_t i=0; i<sizeof(digits); i++) {
		res = parse_numeric_char(arg[i], &digits[i], 10);
		if(res) return debug(res, arg);
	}

	new_time.tm_year = 10*digits[0]  + digits[1] + 100;
	new_time.tm_mon	 = 10*digits[2]  + digits[3] - 1;
	new_time.tm_mday = 10*digits[4]  + digits[5];
	new_time.tm_hour = 10*digits[6]  + digits[7];
	new_time.tm_min  = 10*digits[8]  + digits[9];
	new_time.tm_sec  = 10*digits[10] + digits[11];

	if(new_time.tm_mon < 0)   return debug(parse_wrong_time, arg);
	if(new_time.tm_mon > 11)  return debug(parse_wrong_time, arg);
	
	bool leap_year = ((new_time.tm_year % 4 == 0) && (new_time.tm_year % 100 != 0)) || ((new_time.tm_year-100) % 400 == 0);
	uint8_t days_in_month[] = {31, uint8_t(leap_year ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t dm = days_in_month[new_time.tm_mon];

	if(new_time.tm_mday > dm) return debug(parse_wrong_time, arg);
	if(new_time.tm_mday == 0) return debug(parse_wrong_time, arg);
	if(new_time.tm_hour > 23) return debug(parse_wrong_time, arg);
	if(new_time.tm_min > 59)  return debug(parse_wrong_time, arg);
	if(new_time.tm_sec > 59)  return debug(parse_wrong_time, arg);

	time_t time_seconds;
	time_seconds = mktime(&new_time);

	if(time_seconds == time_t(-1)) return debug(parse_error, arg);

	*output = time_seconds;
	return parse_ok;
}

void print_ok(void) {
	printf(TEXT_GREEN "OK" FORMAT_RESET "\n");
}

void print_hexstr(uint8_t array[], size_t len, bool new_line, char separator) {
	if(len) {
		while(--len) {
			printf("%02X", *array++);
			if(separator) printf("%c", separator);
		}

		printf("%02X", *array++);
	}

	if(new_line) printf("\n");
}

// ========================================
// Commands
// ========================================

#if USE_CMD_ALL
	void all_commands_cmd(int argc, char * argv[]) {
		printf(TEXT_WHITE_BRIGHT "Num\tPointer\t\tName\n" FORMAT_RESET);
		
		for(uint16_t i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
			printf("%u\t%08lX\t%s\n", i, uint32_t(command_list[i].ptr), command_list[i].name);
		}
	}
#endif
