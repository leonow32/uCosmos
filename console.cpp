#include "../config.h"
#if COMPONENT_CONSOLE
#include <string.h>
#include <limits>
#include <iostream>
#include "log.h"
#include "console.h"
#include "../command_list.h"

size_t	received_cnt;							// Number of characters in receive buffer
char 	buffer[CONSOLE_COMMAND_LENGTH];			// Buffer for currently processed command
#if CONSOLE_USE_CTRL_Z
	char buffer2[CONSOLE_COMMAND_LENGTH];		// Buffer for last processed command, recalled with CTRL-Z
#endif

void console_init(void) {
	LOGI("init");
	// task_add(console_task, 20);
	task_add(console2_task, 20);
}

static void prompt_show(void) {
	printf("\n" TEXT_YELLOW_BRIGHT FORMAT_BOLD "/> %s", buffer);
}

// Pobieranie jednego znaku z bufora wejściowego i kopiowanie do bufora konsoli lub wykonywanie akcji
static inline console_res char_input(char received_char) {
	if(received_char >= ' ' && received_char <= '~') {							// Znaki printable dodajemy do bufora
		if(received_cnt >= (CONSOLE_COMMAND_LENGTH - 1)) {						// Sprawdzenie czy jest wolne miejsce w buforze
			return con_buffer_full;
		}
		else {
			buffer[received_cnt++] = received_char;								// Dodawanie znaków do bufora 
			if(received_cnt == 1) {												// TODO to jest słabe, trzeba wymyślić coś bardziej sensownego
				return con_recv_begin;											// tu zostanie wyświetlony prompt jeżeli użytkownik skasował polecenie przez backspace
			}
			else {
				printf("%c", received_char);
				return con_ok;
			}
		}
	}
	
	switch(received_char) {														// Interpretowanie znaków kontrolnych dla HMI
		case CR:																// Enter
		case LF:
			if(received_cnt) {													// Zwracanie con_ok tylko jeżeli coś mamy w buforze, jeśli bufor jest pusty to brak reakcji
				return con_recv_cmd;	
			}
			break;
		
		case BACKSPACE1:														// Backspace
		case BACKSPACE2:
			if(received_cnt) {
				received_cnt--;
				buffer[received_cnt] = 0;
				printf("%c", received_char);
			}
			break;
		
		
		#if CONSOLE_USE_CTRL_Z													// Przywrócenie ostatnio wpisywanego polecenia
		case CTRL_Z:
			while(received_cnt) {												// Kasowanie dotychczas wpisanego polecenia
				received_cnt--;
				buffer[received_cnt] = 0;
				printf("%c", BACKSPACE1);
			}

			memcpy(buffer, buffer2, CONSOLE_COMMAND_LENGTH);					// Kopiowanie bufora poprzedniego polecanie do bufora aktywnego polecenia
			buffer[CONSOLE_COMMAND_LENGTH - 1] = NUL;
			received_cnt = strlen(buffer);
			prompt_show();
			break;
		#endif
		
		case ESC:																// ESCAPE - to samo, ale wprowadzone z klawiatury przez użytkownika
			received_cnt = 0;
			memset(buffer, 0, CONSOLE_COMMAND_LENGTH);
			return con_input_cancelled;
		
		default:																// Wszystkie inne znaki ignorujemy - w tym NUL
			break;
	}
	
	return con_ok;
}

// Dzielenie wejściowego stringu z wierza poleceń na pojedyncze argumenty
// - argc -	Wskaźnik, przez który zwracana jest liczba znalezionych argumentów
// - argv -	Wskaźnik do tablicy wskaźników, w której zapisywane są wskaźniki do kolejnych argumentów
static inline console_res split_args(uint8_t * argc, char * argv[]) {
	uint8_t	arg_cnt		=	0;
	char *	char_ptr	=	buffer;
	bool	new_arg		=	true;
	bool	string_mode	=	false;
	
	while(*char_ptr != NUL) {													// Przesuwanie wskaźnika aż do napotkania znaku NUL kończącego string
		switch(*char_ptr) {
		
			case ' ':															// Spacja = nowy argument jeśli to nie jest "string objęty cudzysłowiem"
				if(string_mode == false) {
					new_arg = true;
					*char_ptr = 0;												// zamiana spacji na nullptr
				}
				break;
			
			case '"':
				if(string_mode) {
					string_mode = false;
					*char_ptr = 0;												// zmiana zamykającego " na zero
				}
				else {
					string_mode = true;
					new_arg = true;
				}
				break;
			
			default:															// Dowolny inny znak
				if(new_arg) {													// Jeśli wcześniej była spacja to mamy tutaj początek nowego argumentu
					if(arg_cnt >= CONSOLE_MAX_ARGUMENTS) {
						return con_too_many_args;
					}
					argv[arg_cnt++] = char_ptr;
					new_arg = false;
				}
				break;
		}
		
		char_ptr++;																// Przesuwanie wskaźnika na następny znak linii poleceń
	}
	
	*argc = arg_cnt;
	return con_ok;
}

static inline console_res split_args(int * argc, char * argv[]) {
	uint8_t	arg_cnt		=	0;
	char *	char_ptr	=	buffer;
	bool	new_arg		=	true;
	bool	string_mode	=	false;
	
	while(*char_ptr != NUL) {													// Przesuwanie wskaźnika aż do napotkania znaku NUL kończącego string
		switch(*char_ptr) {
		
			case ' ':															// Spacja = nowy argument jeśli to nie jest "string objęty cudzysłowiem"
				if(string_mode == false) {
					new_arg = true;
					*char_ptr = 0;												// zamiana spacji na nullptr
				}
				break;
			
			case '"':
				if(string_mode) {
					string_mode = false;
					*char_ptr = 0;												// zmiana zamykającego " na zero
				}
				else {
					string_mode = true;
					new_arg = true;
				}
				break;
			
			default:															// Dowolny inny znak
				if(new_arg) {													// Jeśli wcześniej była spacja to mamy tutaj początek nowego argumentu
					if(arg_cnt >= CONSOLE_MAX_ARGUMENTS) {
						return con_too_many_args;
					}
					argv[arg_cnt++] = char_ptr;
					new_arg = false;
				}
				break;
		}
		
		char_ptr++;																// Przesuwanie wskaźnika na następny znak linii poleceń
	}
	
	*argc = arg_cnt;
	return con_ok;
}

// Wyszukiwanie funkcji dla wpisanego polecenia
// Nazwa polecenia jest przechowywana w argumencie zerowym argv[0]
// Funkcja zwraca wskaźnik do funkcji typu void pobierającej argumenty int argc, char * argv[]
// Argumentem przyjmowanym przez Console_FindCommand() jest wskaźnik do stringu EnteredName zawierającego polecenie wpisane przez użytkownika
// Jeżeli Console_FindCommand nie znajdzie funkcji odpowiadającej poleceniu to zwraca wskaźnik nullptr
static inline void (*find_ptr(const char * what_to_find))(int argc, char * argv[]) {
	for(int i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
		if(strcmp(what_to_find, command_list[i].name) == 0) {
			return command_list[i].ptr;
		}
	}
	return nullptr;
}

// System task to periodicaly execute interpreter routine
void console_task(run_mode_t run_mode) {
	
	// Normal execution
	if(run_mode == os_run) {
		while(1) {

			#if ESP_PLATFORM
				int chr = getchar();
				if(chr == EOF) {
					return;
				}
			#elif PICO_RP2040 || PICO_RP2350
				int chr = getchar_timeout_us(0);
				if (chr == PICO_ERROR_TIMEOUT) {
					return;
				}
			#endif
			
			console_res res = char_input(chr);											// Przekazywanie znaków z bufora wejściowego do bufora konsoli i podejmywanie dalszych działa w zależności od console_res
			
			switch(res) {
				case con_recv_cmd: {
					uint8_t	argc = 0;													// Liczba wpisanych argumentów
					char * argv[CONSOLE_MAX_ARGUMENTS];									// Tablica wskaźników do argumentów
					memset(argv, 0, sizeof(argv));
					
					#if CONSOLE_USE_CTRL_Z												// Kopiowanie do bufora CTRL-Z
						memcpy(buffer2, buffer, CONSOLE_COMMAND_LENGTH);
					#endif
					
					console_res split_res = split_args(&argc, argv);					// Dzielenie bufora na poszczególne argumenty
					if(split_res == con_too_many_args) {
						printf(TEXT_RED "\nToo many args\n" FORMAT_RESET);
					}
					else {
						void (*cmd_ptr)(int argc, char * argv[]) = find_ptr(argv[0]);	// Wyszukiwanie wskaźnika 
						
						if(cmd_ptr) {													// Wykonanie polecenia, jeśli rozpoznano
							printf(FORMAT_RESET "\n");									// Przejście do kolejnej linii
							cmd_ptr(argc, argv);										// Wywołanie funkcji odpowiadającej poleceniu
						}
						else {
							printf(TEXT_RED "\nUnknown cmd\n" FORMAT_RESET);			// Jeżeli nie rozpoznano polecenia
						}
					}
					
					memset(buffer, 0, CONSOLE_COMMAND_LENGTH);							// Czyszczenie aktualnego bufora wiersza poleceń
					received_cnt = 0;
					break;
				}
			
				case con_buffer_full:													// Koniec bufora
					printf("%c", BEL);													// Dźwięk jeżeli przepełniono bufor konsoli
					break;

				case con_input_cancelled:												// Wciśnięto ESCAPE
				case con_recv_begin:													// Jeżeli to pierwszy znak polecenia to wyświetla prompt, który wcześniej był ukryty
					prompt_show();
					break;

				case con_too_many_args:					// TODO z jakiegoś powodu to jest potrzebne na ESP, a na PICO nie
					LOGW("Too many args");
					break;

				case con_error_to_fix:					// TODO to musi być na ESP32
					LOGW("Er to fix");
					break;

				case con_ok:					// TODO to musi być na ESP32
					// LOGW("response ok");
					break;
			}
		}
	}

	else if(run_mode == os_id) {
		task_name = __func__;
	}
}

// ========================================
// New Command Line Interpreter
// ========================================

// System task to periodicaly execute interpreter routine
void console2_task(run_mode_t run_mode) {
	if(run_mode == os_run) {
		while(1) {

			#if ESP_PLATFORM									// Read a character from console input
				int chr = getchar();
				if(chr == EOF) {
					return;
				}
			#elif PICO_RP2040 || PICO_RP2350
				int chr = getchar_timeout_us(0);
				if(chr == PICO_ERROR_TIMEOUT) {
					return;
				}
			#endif

			if(chr == ESCAPE) {									// If the received char is ESCAPE - clear the buffer and display the prompt
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));
				prompt_show();
			}

			else if(chr == CTRL_Z) {							// If the received char is CTRL-Z - clear the buffer, display prompt and previously entered command
				received_cnt = 0;
				strcpy(buffer, buffer2);
				received_cnt = strlen(buffer);
				prompt_show();
			}

			else if(chr == BACKSPACE1 || chr == BACKSPACE2) {	// If the received char is BACKSPACE - clear thbuffer, display prompt and previously entered command
				if(received_cnt) {								// If any character has been received
					received_cnt--;								// Decrement characters counter
					buffer[received_cnt] = 0;					// Remove from the buffer
					printf("%c", chr);							// Print backspace to the console so the user can see that the last character is erased
				}
			}
			
			else if(chr >= ' ' && chr <= '~') {					// If the received char is any printable character - add it to the buffer if possible or emit sound
				if(received_cnt < CONSOLE_COMMAND_LENGTH) {		// IF there is space in the receive buffer
					if(received_cnt == 0) {
						prompt_show();
					}
					buffer[received_cnt++] = chr;				// Add the character to the buffer and increment the counter
					printf("%c", chr);							// Print the received character so the user can see it
				}
				else {
					printf("%c", BEL);							// Emit sound signal
				}
			}
				
			else if((received_cnt > 0) && (chr == CR || chr == LF)) {			// If the received char is ENTER and the buffer is not empty - try to execute the command
				printf(FORMAT_RESET "\n");
				printf("received_cnt: %u\n", received_cnt);
				printf("buffer: '%s'\n", buffer);
				
				strcpy(buffer2, buffer);											// Copy the buffer to CTRL-Z buffer
				
				// Create argv array
				int	   argc = 0;													// Liczba wpisanych argumentów
				char * argv[CONSOLE_MAX_ARGUMENTS] = {0};									// Tablica wskaźników do argumentów
				bool   string_mode	=	false;
				
				// split_args(&argc, argv);					// Dzielenie bufora na poszczególne argumenty
				char * char_ptr = buffer;
				bool pick_new_argument = true;
				while(*char_ptr) {
					if(*char_ptr == NUL) {
						break;
					}

					else if(*char_ptr == ' ' && string_mode == false) {
						*char_ptr = NUL;
						char_ptr++;
						pick_new_argument = true;
					}

					else if(*char_ptr == '"' && pick_new_argument) {
						string_mode = true;
						char_ptr++;
						if(*char_ptr != NUL) {
							if(argc >= CONSOLE_MAX_ARGUMENTS) {
								printf(TEXT_RED "\nToo many args\n" FORMAT_RESET);
								argv[0] = nullptr;
								break;
							}
							argv[argc++] = char_ptr;
							char_ptr++;
						}
					}

					else if(*char_ptr == '"' && string_mode == true) {
						*char_ptr++ = NUL;
						// char_ptr++;
						pick_new_argument = true;
						string_mode = false;
					}

					else if(*char_ptr != ' ' && pick_new_argument) {
						pick_new_argument = false;
						if(argc >= CONSOLE_MAX_ARGUMENTS) {
							printf(TEXT_RED "\nToo many args\n" FORMAT_RESET);
							argv[0] = nullptr;
							break;
						}
						argv[argc++] = char_ptr;
					}

					else {
						char_ptr++;
					}
				}

				// debug
				printf("argc = %u\n", argc);
				for(int i=0; i<CONSOLE_MAX_ARGUMENTS; i++) {
					printf("%u:\t%08lX\t'%s'\n", i, uint32_t(argv[i]), argv[i] ? argv[i] : "nullptr");
				}

				if(argv[0]) {
					// Find command pointer
					void (*command_ptr)(int argc, char * argv[]) = nullptr;

					if(argv[0]) {
						for(int i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
							if(strcmp(argv[0], command_list[i].name) == 0) {
								command_ptr = command_list[i].ptr;
								break;
							}
						}
					}

					// Execute the pointer and pass argc and argv
					if(command_ptr) {
						printf(FORMAT_RESET "\n");									// Przejście do kolejnej linii
						command_ptr(argc, argv);
					}
					else {
						printf(TEXT_RED "\nUnknown cmd\n" FORMAT_RESET);			// Jeżeli nie rozpoznano polecenia
					}
				}

				// Clear command buffer
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));
			}
		}
	}

	else if(run_mode == os_id) {
		task_name = __func__;
	}
}

/*
void console3_task(run_mode_t run_mode) {
	if(run_mode == os_run) {
		while(1) {

			#if ESP_PLATFORM									// Read a character from console input
				int chr = getchar();
				if(chr == EOF) {
					return;
				}
			#elif PICO_RP2040 || PICO_RP2350
				int chr = getchar_timeout_us(0);
				if(chr == PICO_ERROR_TIMEOUT) {
					return;
				}
			#endif

			if(chr == ESCAPE) {									// If the received char is ESCAPE - clear the buffer and display the prompt
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));
				prompt_show();
			}

			else if(chr == CTRL_Z) {							// If the received char is CTRL-Z - clear the buffer, display prompt and previously entered command
				received_cnt = 0;
				strcpy(buffer, buffer2);
				received_cnt = strlen(buffer);
				prompt_show();
			}

			else if(chr == BACKSPACE1 || chr == BACKSPACE2) {	// If the received char is BACKSPACE - clear thbuffer, display prompt and previously entered command
				if(received_cnt) {								// If any character has been received
					received_cnt--;								// Decrement characters counter
					buffer[received_cnt] = 0;					// Remove from the buffer
					printf("%c", chr);							// Print backspace to the console so the user can see that the last character is erased
				}
			}
			
			else if(chr >= ' ' && chr <= '~') {					// If the received char is any printable character - add it to the buffer if possible or emit sound
				if(received_cnt < CONSOLE_COMMAND_LENGTH) {		// IF there is space in the receive buffer
					if(received_cnt == 0) {
						prompt_show();
					}
					buffer[received_cnt++] = chr;				// Add the character to the buffer and increment the counter
					printf("%c", chr);							// Print the received character so the user can see it
				}
				else {
					printf("%c", BEL);							// Emit sound signal
				}
			}
				
			else if((received_cnt > 0) && (chr == CR || chr == LF)) {			// If the received char is ENTER and the buffer is not empty - try to execute the command
				printf("\n");
				printf("received_cnt: %u\n", received_cnt);
				printf("buffer: '%s'\n", buffer);
				
				strcpy(buffer2, buffer);											// Copy the buffer to CTRL-Z buffer
				
				// Create argv array
				int	   argc = 0;													// Liczba wpisanych argumentów
				char * argv[CONSOLE_MAX_ARGUMENTS] = {0};									// Tablica wskaźników do argumentów
				// bool   string_mode	=	false;
				console_state state = console_idle;
				
				// split_args(&argc, argv);					// Dzielenie bufora na poszczególne argumenty
				char * char_ptr = buffer;
				// bool pick_new_argument = true;
				while(*char_ptr) {

					switch(state) {
						case console_idle:
							if(*char_ptr == '"') {
								state = console_string;
							}
							else {
								state = console_argument;
							}
							break;

						case console_argument:

							break;

						case console_string:


							break;
					}




					if(*char_ptr == NUL) {
						break;
					}

					else if(*char_ptr == ' ' && string_mode == false) {
						*char_ptr = NUL;
						char_ptr++;
						pick_new_argument = true;
					}

					else if(pick_new_argument && *char_ptr == '"') {
						string_mode = true;
						char_ptr++;
					}

					else if(*char_ptr == '"' && string_mode == true) {
						*char_ptr++ = NUL;
						*char_ptr++ = NUL;
						pick_new_argument = true;
						string_mode = false;
					}

					else {
						if(pick_new_argument) {
							pick_new_argument = false;
							if(argc >= CONSOLE_MAX_ARGUMENTS) {
								printf(TEXT_RED "\nToo many args\n" FORMAT_RESET);
								// argc = 0;
								argv[0] = nullptr;
								break;
							}
							argv[argc++] = char_ptr;
						}
						char_ptr++;
					}
				}

				// debug
				printf("argc = %u\n", argc);
				for(int i=0; i<CONSOLE_MAX_ARGUMENTS; i++) {
					printf("%u:\t%08lX\t'%s'\n", i, uint32_t(argv[i]), argv[i] ? argv[i] : "nullptr");
				}

				if(argv[0]) {
					// Find command pointer
					void (*command_ptr)(int argc, char * argv[]) = nullptr;

					if(argv[0]) {
						for(int i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
							if(strcmp(argv[0], command_list[i].name) == 0) {
								command_ptr = command_list[i].ptr;
								break;
							}
						}
					}

					// Execute the pointer and pass argc and argv
					if(command_ptr) {
						printf(FORMAT_RESET "\n");									// Przejście do kolejnej linii
						command_ptr(argc, argv);
					}
					else {
						printf(TEXT_RED "\nUnknown cmd\n" FORMAT_RESET);			// Jeżeli nie rozpoznano polecenia
					}
				}

				// Clear command buffer
				received_cnt = 0;
				memset(buffer, 0, sizeof(buffer));
			}
		}
	}

	else if(run_mode == os_id) {
		task_name = __func__;
	}
}
*/

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
			case parse_missing_argument:	printf("Missing arg");		break;
			case parse_over_range:			printf("Over range");		break;
			case parse_under_range:			printf("Under range");		break;
			case parse_error:				printf("Parse error");		break;
			case parse_expected_bin:		printf("Expected bin");		break;
			case parse_expected_dec:		printf("Expected dec");		break;
			case parse_expected_hex:		printf("Expected hex");		break;
			case parse_nullptr:				printf("Null ptr");			break;
			case parse_wrong_base:			printf("Wrong base");		break;
			case parse_wrong_time:			printf("Wrong time");		break;
			default:						printf("?");				break;
		}

		printf(FORMAT_RESET "\n");
	}
	
	return res;
}

static parse_res parse_numeric_char(const char chr, uint8_t * output, uint8_t base) {
	uint8_t value = 0;
	
	if(chr >= '0' && chr <= '9')
		value = chr - '0';
	else if(chr >= 'A' && chr <= 'F')
		value = chr - 'A' + 10;
	else if(chr >= 'a' && chr <= 'f')
		value = chr - 'a' + 10;
	else
		return parse_error;

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

template<typename T> parse_res parse_uint(const char * arg, T * output, uint8_t base) {
	static_assert(std::is_unsigned_v<T>, "This function works with unsigned integers only.");
	const char * arg_copy = arg;
	
	if(arg == nullptr)			return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)			return debug(parse_missing_argument, arg);
	if(output == nullptr)		return debug(parse_nullptr, arg);
	if(base < 2 || base > 16)	return debug(parse_wrong_base, arg);

	*output = 0;

	while(*arg) {
		uint8_t char_value = 0;
		parse_res res = parse_numeric_char(*arg++, &char_value, base);
		if(res) return debug(res, arg_copy);
		if(*output > (std::numeric_limits<T>::max() - char_value) / base) return debug(parse_over_range, arg_copy);
		*output = *output * base + char_value;
	} 

	return parse_ok;
}

template parse_res parse_uint<uint8_t>(const char *, uint8_t *, uint8_t);
template parse_res parse_uint<uint16_t>(const char *, uint16_t *, uint8_t);
template parse_res parse_uint<uint32_t>(const char *, uint32_t *, uint8_t);
template parse_res parse_uint<uint64_t>(const char *, uint64_t *, uint8_t);

template<typename T> parse_res parse_int(const char * arg, T * output) {
	static_assert(std::is_signed_v<T>, "This function works with signed integers only.");
	const char * arg_copy = arg;
	bool negative = false;
	
	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);
	
	if(*arg == '-') {
		negative = true;
		arg++;
	}

	*output = 0;

	while(*arg) {
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

parse_res parse_ascii_char(const char * arg, char * output) {
	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);
	*output = arg[0];
	return parse_ok;
}

parse_res parse_ascii_string(const char * arg, char * output, size_t * out_len, const size_t max_len, const size_t min_len) {
	const char * arg_copy = arg;
	
	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);
	if(out_len == nullptr)	return debug(parse_nullptr, arg);
	if(max_len < min_len)	return debug(parse_error, arg);
	
	*out_len = 0;
	
	while(1) {
		*output = *arg;
		if(*arg++) {
			(*out_len)++;
			if(*out_len >= max_len) {
				*output = NUL;
				return debug(parse_over_range, arg_copy);
			}
			output++;
		}
		else {
			break;
		}
	}

	if(*out_len < min_len) return debug(parse_under_range, arg_copy);
	return parse_ok;
}

parse_res parse_hex_string(const char * arg, uint8_t * output, size_t * out_len, const size_t max_len, const size_t min_len) {
	parse_res res = parse_ok;
	const char * arg_copy = arg;
	uint8_t nibble_h;
	uint8_t nibble_l;

	if(arg == nullptr)		return debug(parse_missing_argument, arg);
	if(arg[0] == NUL)		return debug(parse_missing_argument, arg);
	if(output == nullptr)	return debug(parse_nullptr, arg);
	if(out_len == nullptr)	return debug(parse_nullptr, arg);
	if(max_len < min_len)	return debug(parse_error, arg);

	*out_len = 0;

	while(*arg != 0) {
		if(*arg == ' ') {									// Pomijanie spacji
			arg++;
			continue;
		}

		if(*out_len >= max_len) return debug(parse_over_range, arg_copy);

		res = parse_numeric_char(*arg++, &nibble_h, 16);				// Przetwarzanie starszego nibble
		if(res) return debug(res, arg_copy);
		
		res = parse_numeric_char(*arg++, &nibble_l, 16);				// Przetwarzanie młodszego nibble
		if(res) return debug(res, arg_copy);
		
		*output++ = nibble_h << 4 | nibble_l;				// Sklejanie wyniku
		(*out_len)++;										// Licznie bajtów w tablicy wynikowej
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

	new_time.tm_year	= 10*digits[0]  + digits[1] + 100;
	new_time.tm_mon		= 10*digits[2]  + digits[3] - 1;
	new_time.tm_mday	= 10*digits[4]  + digits[5];
	new_time.tm_hour	= 10*digits[6]  + digits[7];
	new_time.tm_min		= 10*digits[8]  + digits[9];
	new_time.tm_sec		= 10*digits[10] + digits[11];

	bool leap_year = ((new_time.tm_year % 4 == 0) && (new_time.tm_year % 100 != 0)) || ((new_time.tm_year-100) % 400 == 0);
	uint8_t days_in_month[] = {31, uint8_t(leap_year ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t dm = days_in_month[new_time.tm_mon];

	if(new_time.tm_mon > 11)  return debug(parse_wrong_time, arg);
	if(new_time.tm_mon < 0)   return debug(parse_wrong_time, arg);
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

// ========================================
// Commands
// ========================================

#if USE_CMD_ALL
	void all_commands_cmd(int argc, char * argv[]) {
		printf(TEXT_WHITE_BRIGHT "Num\tPointer\t\tName\n" FORMAT_RESET);
		
		for(uint16_t i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
			printf("%u:\t%08lX\t%s\n", i, uint32_t(command_list[i].ptr), command_list[i].name);
		}
	}
#endif
#endif
