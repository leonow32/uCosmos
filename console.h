#ifndef CONSOLE_H_
#define CONSOLE_H_
#if COMPONENT_CONSOLE

#include <time.h>
#include "uCosmos.h"

#include "log.h"

// Result type for command interpreter
enum console_res {
	con_ok = 0,					// Zwracane przez wszystkie funkcje, jeżeli zakończyły się prawidłowo
	con_recv_begin,				// Odebrano pierwszy znak nowego polecenia
	con_recv_cmd,				// Odebrano pełne polecenie gotowe do dalszej analizy
	con_buffer_full,			// Przepełnienie bufora
	con_input_cancelled,		// Wciśnięto ESC, trzeba wyświetlić ponownie znak zachęty wiersza poleceń
	con_too_many_args,			// Użytkownik wpisał więcej argumentów niż jest to dopuszczalne
	con_error_to_fix = 255,		// Tylko do celów deweloperskich, normalnie żadna funkcja nie powinna zwracać czegoś takiego
};

// Result type for argument parser
enum parse_res {
	parse_ok = 0,				// Zwracane przez wszystkie funkcje, jeżeli zakończyły się prawidłowo
	parse_missing_argument,
	parse_over_range,
	parse_under_range,
	parse_error,
	parse_expected_bin,
	parse_expected_dec,
	parse_expected_hex,
	parse_wrong_base,
	parse_wrong_time,
	parse_nullptr,				// No result pointer was given
};

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
void console2_task(run_mode_t run_mode);

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
#endif /* CONSOLE_H_ */
