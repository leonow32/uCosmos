#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

// Konfiguracja systemu
#define OS_TASK_MAXCOUNT				10			// Od tego zalezy rozmiar tablicy tasków w RAM
#define OS_TICK_PERIOD_MS				10			// Okres timera systemowego w ms, zakres 1-1000
#define OS_SHOW_RESET_SOURCE_AT_START	1			// Czy pokazywać źródło resetu na starcie
#define OS_SHOW_SPLASH_SCREEN_AT_START	1			// Czy pokazywać logo systemu na starcie

// Konfiguracja konsoli
#define CONSOLE_COMMAND_LENGTH			250
#define CONSOLE_MAX_ARGUMENTS			11
#define CONSOLE_USE_HELP				1
#define CONSOLE_USE_CTRL_Z				1
#define CONSOLE_USE_COMMAND_ALL			1
#define CONSOLE_USE_DEMO_COMMANDS		1

// Czas
#define OS_USE_TIME						1			// Zegar czasu rzeczywistego działający w tle (147B)
#define OS_USE_TIME_COMMAND				1			// Dodaje polecenie "time" do pokazywania czasu (963B)

// Debugowanie
#define OS_DEBUG_MESSAGES_SHOW			1			// Komunikaty u tworzeniu i zamykaniu tasków
#define OS_DEBUG_MESSAGES_TIMESTAMP		1			// W komunikatach o błędach będzie podana data i godzina
#define OS_USE_TASK_IDENTIFY			1			// Identyfikacja tasków poprzez wysołanie z argumentem identify 
#define OS_USE_TASK_MONITOR				1			// Wyświetlanie na UART tablicy tasków
#define OS_USE_TASK_COMMANDS			1			// Polecenia do zarządzania taskami
#define OS_USE_DEMO_TASKS				1			// Demonstracyjne taski

#define OS_CONFIG_DONE

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
