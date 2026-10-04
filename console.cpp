#include "../config.h"
#if COMPONENT_CONSOLE
static const char *TAG = "console";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "console.h"
#include "../commands.h"

namespace console {

size_t	received_cnt;							// Number of characters in receive buffer
char 	buffer[CONSOLE_COMMAND_LENGTH];			// Buffer for currently processed command
#if CONSOLE_USE_CTRL_Z
	char buffer2[CONSOLE_COMMAND_LENGTH];		// Buffer for last processed command, recalled with CTRL-Z
#endif

void init(void) {
	// ESP_LOGI(TAG, "init");
	task_add(console_task, 20);
}

static void prompt_show(void) {
	printf("\n" TEXT_YELLOW_BRIGHT FORMAT_BOLD "/> %s", buffer);
}

// Porównuje badany string do wzorcowego aż do:
// - napotkania NULL w obu stringach
// - pierwszego różnego znaku w obu stringach
// Tak skonstrukowana funkcja zajmuje mniej miejsca niż strcmp() z biblioteki string.h
static inline bool _strcmp(const char *string1, const char *string2) {
	while(1) {
		if((*string1 == 0) && (*string2 == 0)) return true;						// Doszliśmy do końca obu stringów, czyli są sobie równe
		if(*string1++ != *string2++) return false;								// Przy pierwszej napotkanej różnicy zwróć fałsz
	}
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
					*char_ptr = 0;												// zamiana spacji na NULL
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
// Jeżeli Console_FindCommand nie znajdzie funkcji odpowiadającej poleceniu to zwraca wskaźnik NULL
static inline void (*find_ptr(const char * what_to_find))(int argc, char * argv[]) {
	for(uint8_t i=0; i<(sizeof(console::command_list)/sizeof(console::command_struct)); i++) {
		const char * cmd_name = (const char *)(command_list[i].name);
		
		if(_strcmp(what_to_find, cmd_name)) {
			void (*cmd_ptr)(int argc, char * argv[]);							// Pusty wskaźnik do polecenia
			cmd_ptr = command_list[i].ptr;										// Odczytanie wskaźnika do polecenia z pamięci flash i rzutowanie go na właściwy typ
			return cmd_ptr;
		}
	}
	return NULL;
}

// System task to periodicaly execute interpreter routine
void console_task(run_mode_t run_mode) {
	
	// Normaln execution
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
					char *	argv[CONSOLE_MAX_ARGUMENTS];								// Tablica wskaźników do argumentów
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
			}
			
			// if(console_res == con_recv_cmd) {										// Jeżeli zakończono odbieranie polecenia
				
			// }

			// else if(console_res == con_buffer_full) {								
			// 	printf("%c", BEL);						
			// }
			
			// else if(console_res == con_input_cancelled) {							
			// 	prompt_show();
			// }

			// else if(console_res == con_recv_begin) {								
			// 	prompt_show();
			// }
		}
	}
	
	#if OS_USE_TASK_IDENTIFY
	else if(run_mode == os_id) {
		printf(__func__);
	}
	#endif
}

// ========================================
// Argument parsers
// ========================================

void debug(const parse_res res, const char * arg) {
	if(res == parse_ok) {
		print_ok();
		return;
	}
	
	if(arg != NULL) {
		printf(TEXT_RED "Error in agument " TEXT_RED_BRIGHT "%s" TEXT_RED ": ", (const char *)arg);
	}
	
	printf(TEXT_RED);

	switch(res) {
		case parse_unknown_command:						printf("Unknown command");				break;
		case parse_no_input:							printf("No input");						break;
		case parse_overflow:							printf("Overflow");						break;
		case parse_missing_argument:					printf("Missing arg");					break;
		case parse_underflow:							printf("Underflow");					break;
		case parse_error:								printf("Parse error");					break;
		case parse_expected_hex:						printf("Expected Hex");					break;
		case parse_expected_dec:						printf("Expected Dec");					break;
		default:										printf("Unknown");						break;
	}

	printf(FORMAT_RESET "\n");
}

// Funkcja przekształca znak ASCII HEX na wartość binarną
// - input_char	 - Wskaźnik do badanego znaku
// - output_char - Wskaźnik do zmiennej, w której ma być zapisany nibble
static parse_res parse_hex_char(const char * input_char, uint8_t * output_char) {
	if(*input_char >= '0' && *input_char <= '9') {
		*output_char = *input_char - '0';
		return parse_ok;
	}
	else if(*input_char >= 'A' && *input_char <= 'F') {
		*output_char = *input_char - 55;
		return parse_ok;
	}
	else if(*input_char >= 'a' && *input_char <= 'f') {
		*output_char = *input_char - 87;
		return parse_ok; 
	}
	else {
		return parse_expected_hex;
	}
}

// Parse single character 0..9 and A..F
static parse_res parse_hex_char(const char * input_char, uint8_t * output_char, bool high_nibble) {
	char temp = *input_char;
	
	// Interpretowanie zaku ASCII
	if(temp >= '0' && temp <= '9') {
		temp = temp - '0';
	}
	else if(temp >= 'A' && temp <= 'F') {
		temp = temp - 55;
	}
	else if(temp >= 'a' && temp <= 'f') {
		temp = temp - 87;
	}
	else {
		return parse_expected_hex;
	}
	
	// Przesuwanie jeżeli to jest starszy nibble
	if(high_nibble) {
		temp <<= 4;
		*output_char |= temp;
	}
	else {
	// Zapisywanie wyniku
		*output_char = temp;
	}
	
	return parse_ok; 
}

// Przetwarzanie stringu od końca, a output od początku
// Zmienna wskazywana przez *output musi być wyzerowana, inaczej będzie błąd losowo zainicjalizowanej pamięci
static parse_res parse_hex_num(const char * arg, void * output, uint8_t chars) {
	parse_res res;
	const char * arg_copy = arg;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	arg = arg + chars;										// Przesunięcie wskaźnika na koniec ciągu znaków argumentu
	
	if(*arg != 0) {											// Sprawdzanie czy ostatni bajt argumentu to 0, jeżeli nie to znaczy, że przesłano więcej znaków niż jest potrzebne dla konkretnego typu zmiennej
		res = parse_overflow;
		goto end;
	}
	
	do {													// Przetwarzanie wszystkich znaków ASCII od końca
		uint8_t high_nibble = chars & 0x01;
		res = parse_hex_char(--arg, (uint8_t*)output, high_nibble);
		if(res) {
			goto end;
		}
		if(high_nibble) {
			output = (uint8_t *)output + 1;
		}
	} while(--chars);
	
	end:
	if(res) {												// Wyświetlenie informacji o ewentualnym błędzie
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie liczby HEX 8-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_hex8(const char * arg, uint8_t * output) {
	*output = 0;
	return parse_hex_num(arg, output, 2);
}

// Parsowanie liczby HEX 16-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_hex16(const char * arg, uint16_t * output) {
	*output = 0;
	return parse_hex_num(arg, output, 4);
}

// Parsowanie liczby HEX 24-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_hex24(const char * arg, uint32_t * output) {
	*output = 0;
	return parse_hex_num(arg, output, 6);
}

// Parsowanie liczby HEX 32-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_hex32(const char * arg, uint32_t * output) {
	*output = 0;
	return parse_hex_num(arg, output, 8);
}

// Parsowanie liczby dziesiętnej 8-bitowej
// Funkcja przekształca znak ASCII HEX na wartość binarną
// - input_char	 - Wskaźnik do badanego znaku
// - output_char - Wskaźnik do zmiennej, w której ma być zapisany nibble
static parse_res parse_dec_char(const char * input_char, char * output_char) {
	if(*input_char >= '0' && *input_char <= '9') {
		*output_char = *input_char - '0';
		return parse_ok;
	}
	else {
		return parse_expected_dec;
	}
}

// Parsowanie liczby dziesiętnej 8-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_dec8(const char * arg, uint8_t * output, const uint8_t max) {
	const char * arg_copy = arg;
	char digit; 	
	uint8_t temp = 0;
	uint8_t temp2;
	parse_res res = parse_ok;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	while(*arg != 0) {										// Przetwarzamy wszystkie znaki po kolei
		res = parse_dec_char(arg++, &digit);
		if(res) {
			goto end;
		}
		
		temp2 = temp * 10 + digit;
		if(temp <= temp2) {
			temp = temp2;
		}
		else {
			res = parse_overflow;
			goto end;
		}
	}
	
	if(temp <= max) {										// Zwracanie wyniku
		*output = temp;
	}
	else {
		res = parse_overflow;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie liczby dziesiętnej 16-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output		- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_dec16(const char * arg, uint16_t * output, const uint16_t max) {
	const char * arg_copy = arg;
	char digit; 	
	uint16_t temp = 0;
	uint16_t temp2;
	parse_res res = parse_ok;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	while(*arg != 0) {										// Przetwarzamy wszystkie znaki po kolei
		res = parse_dec_char(arg++, &digit);
		if(res) {
			goto end;
		}
		
		temp2 = temp * 10 + digit;
		if(temp <= temp2) {
			temp = temp2;
		}
		else {
			res = parse_overflow;
			goto end;
		}
	}
	
	if(temp <= max) {										// Zwracanie wyniku
		*output = temp;
	}
	else {
		res = parse_overflow;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie liczby dziesiętnej 16-bitowej ze znakiem
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output	- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_dec16s(const char * arg, int16_t * output) {
	const char * arg_copy = arg;
	char digit; 	
	uint16_t temp = 0;
	parse_res res = parse_ok;
	bool negative = false;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	if(*arg == '-') {										// Czy znak minus na początku
		arg++;
		negative = true;
	}
	
	while(*arg != 0) {										// Przetwarzamy wszystkie znaki po kolei
		res = parse_dec_char(arg++, &digit);
		if(res) {
			goto end;
		}

		temp = temp * 10 + digit;

		if(negative == false && temp > INT16_MAX) {
			res = parse_overflow;
			goto end;
		}
		else if(negative == true && temp > INT16_MAX+1) {
			res = parse_underflow;
			goto end;
		}
	}
	
	if(negative) {											// Zwracanie wyniku
		*output = -int16_t(temp);
	}
	else {
		*output = temp;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie liczby dziesiętnej 32-bitowej
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output		- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_dec32(const char * arg, uint32_t * output, const uint32_t max) {
	const char * arg_copy = arg;
	char digit; 	
	uint32_t temp = 0;
	uint32_t temp2;
	parse_res res = parse_ok;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	while(*arg != 0) {										// Przetwarzamy wszystkie znaki po kolei
		res = parse_dec_char(arg++, &digit);
		if(res) {
			goto end;
		}
		
		temp2 = temp * 10 + digit;
		if(temp <= temp2) {
			temp = temp2;
		}
		else {
			res = parse_overflow;
			goto end;
		}
	}
	
	if(temp <= max) {										// Zwracanie wyniku
		*output = temp;
	}
	else {
		res = parse_overflow;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie liczby dziesiętnej 32-bitowej ze znakiem
// - arg	- Wskaźnik do argumentu, który ma być przetworzony
// - output		- Wskaźnik do zmiennej, w której będzie zwrócony wynik
parse_res parse_dec32s(const char * arg, int32_t * output) {
	const char * arg_copy = arg;
	char digit; 	
	uint32_t temp = 0;
	parse_res res = parse_ok;
	bool negative = false;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
		goto end;
	}
	
	if(*arg == '-') {										// Czy znak minus na początku
		arg++;
		negative = true;
	}
	
	while(*arg != 0) {										// Przetwarzamy wszystkie znaki po kolei
		res = parse_dec_char(arg++, &digit);
		if(res) {
			goto end;
		}

		temp = temp * 10 + digit;

		if(negative == false && temp > INT32_MAX) {
			res = parse_overflow;
			goto end;
		}
		else if(negative == true && temp > INT32_MAX+1ul) {
			res = parse_underflow;
			goto end;
		}
	}
	
	if(negative) {											// Zwracanie wyniku
		*output = -int32_t(temp);
	}
	else {
		*output = temp;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Konwertowanie stringu znaków ASCII HEX na dane zapisane binarnie. W rezultacie wynikowy string jest 2x krótszy od
// stringu wejściowego (jeśli były w nim spacje to dodatowo zostały wycięte). Wszystkie nadmiarowe znaki zostają zastąpione zerami,
// aby można było wykorzystać miejsce w pamięci, które jest dotychczas zajmowane przez string wejściowy.
// - String		- wejście i wyjście
// - max_len	- maksymalna dopuszczalna dłogość stringu po przetworzeniu, domyślnie 255 znaków
// - min_len	- minimalna dopuszczalna długość stringu po przetworzeniu, domyśłnie 0 znaków
parse_res parse_hex_string(const char * arg, uint8_t * output, uint8_t * out_len, const uint8_t max_len, const uint8_t min_len) {
	const char * arg_copy = arg;
	uint8_t nibble_h;
	uint8_t nibble_l;
	*out_len = 0;
	parse_res res = parse_ok;
	
	while(*arg != 0) {										// przetwarzanie aż do napotkania znaku 0
		if(*out_len == max_len) {							// Kontrola przepełnienia
			res = parse_overflow;
			goto end;
		}
		
		if(*arg == ' ') {									// Pomijanie spacji
			arg++;
			continue;
		}
		
		res = parse_hex_char(arg++, &nibble_h);				// Przetwarzanie starszego nibble
		if(res) {
			goto end;
		}
		
		res = parse_hex_char(arg++, &nibble_l);				// Przetwarzanie młodszego nibble
		if(res) {
			goto end;
		}
		
		*output++ = nibble_h << 4 | nibble_l;				// Sklejanie wyniku
		(*out_len)++;										// Licznie znaków w stringu wynikowym
	}
	
	if(*out_len < min_len) {								// Kontrola długości
		res = parse_underflow;
	}

	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parsowanie stringu ASCII
// Zwraca ciąg znaków zakończonu znakiem NUL
parse_res parse_ascii_string(const char * arg, uint8_t * output, uint8_t * out_len, const uint8_t max_len, const uint8_t min_len) {
	const char * arg_copy = arg;
	*out_len = 0;
	parse_res res = parse_ok;
	
	if(arg == NULL) {
		res = parse_missing_argument;
		goto end;
	}
	
	while(1) {												// przetwarzanie aż do napotkania znaku 0
		if(*out_len == max_len) {							// Kontrola przepełnienia
			res = parse_overflow;
			goto end;
		}
		
		(*out_len)++;										// Licznie znaków w stringu wynikowym
		*output = *arg;										// Kopiowanie znaku
		
		if(*arg == NUL) {									// Sprawdzenie czy już doszliśmy do znaku NUL
			break;
		}
		else {
			output++;
			arg++;
		}
	}
	
	if(*out_len < min_len) {								// Kontrola długości
		res = parse_underflow;
	}
	
	end:													// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
	if(res) {
		debug(res, arg_copy);
	}
	return res;
}

// Parse a single character
parse_res parse_ascii_char(const char * arg, uint8_t * output) {
	parse_res res = parse_ok;
	
	if(arg == NULL) {										// Kontrola czy podano argument
		res = parse_missing_argument;
	}
	else {
		res = parse_ok;
		*output = *arg;
	}
	
	if(res) {												// Wyświetlenie informacji o ewentualnym błędzie i zwrócenie wyniku
		debug(res, arg);
	}
	return res;
}

// Parse time given in format YYMMDDhhmmss
parse_res parse_time(const char * arg, time_t * output) {
	parse_res res = parse_ok;
	tm new_time = {};
	
	if(arg == NULL) {										// Sanity check
		res = parse_missing_argument;
		goto end;
	}
	
	if(strlen(arg) != 12) {
		res = parse_error;
		goto end;
	}
	
	uint8_t date_as_numbers[6];								// Parse characters into table
	for(uint8_t i=0; i<sizeof(date_as_numbers); i++) {
		date_as_numbers[i] = ((*arg++) - '0') * 10;
		date_as_numbers[i] += ((*arg++) - '0');
	}
	
	new_time.tm_year	= date_as_numbers[0] + 100;
	new_time.tm_mon		= date_as_numbers[1] - 1;
	new_time.tm_mday	= date_as_numbers[2];
	new_time.tm_hour	= date_as_numbers[3];
	new_time.tm_min		= date_as_numbers[4];
	new_time.tm_sec		= date_as_numbers[5];
	new_time.tm_isdst	= 0;
	new_time.tm_yday	= 0;
	
	time_t time_seconds;
	time_seconds = mktime(&new_time);
	*output = time_seconds;
	
	end:
	if(res) {
		debug(res, arg);
	}
	return res;
}

void print_ok(void) {
	printf(TEXT_GREEN "OK" FORMAT_RESET "\n");
}

// ========================================
// Commands
// ========================================

#if CONSOLE_USE_COMMAND_ALL
	void all_commands_cmd(int argc, char * argv[]) {
		printf(TEXT_WHITE_BRIGHT "Num\tPointer\t\tName\n" FORMAT_RESET);
		
		for(uint16_t i=0; i<(sizeof(command_list)/sizeof(command_struct)); i++) {
			void (*cmd_ptr)(int argc, char * argv[]);						// Pusty wskaźnik do polecenia
			cmd_ptr = command_list[i].ptr;									// Odczytanie wskaźnika do polecenia z pamięci flash i rzutowanie go na właściwy typ
			const char * cmd_name = (const char *)(command_list[i].name);	// Nazwa polecenia	
			printf("%u:\t%p\t%s\n", i, cmd_ptr, cmd_name);
		}
	}
#endif

}
#endif
