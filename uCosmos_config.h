#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

// Konfiguracja systemu
#define OS_TASK_MAXCOUNT				10			// Od tego zalezy rozmiar tablicy tasków w RAM
#define OS_TICK_PERIOD_MS				10			// Okres timera systemowego w ms, zakres 1-1000
#define OS_SHOW_SPLASH_SCREEN_AT_START	1			// Czy pokazywać logo systemu na starcie
#define OS_SHOW_RESET_SOURCE_AT_START	1			// Czy pokazywać źródło resetu na starcie

// Konfiguracja konsoli
#define CONSOLE_COMMAND_LENGTH			250
#define CONSOLE_MAX_ARGUMENTS			11
#define CONSOLE_USE_HELP				1
#define CONSOLE_USE_CTRL_Z				1

// Dostępne polecenia
#define USE_CMD_ALL						1			// Polecenie "?" do wyświetlania na konsoli wszystkich dostępnych komend
#define USE_CMD_TIME					1			// Polecenie "time" do pokazywania czasu
#define USE_CMD_TASK_MONITOR			1			// Polecenie "`" do wyświetlania tablicy tasków
#define USE_CMD_TASK_COMMANDS			1			// Polecenia do ręcznego zarządzania taskami
#define USE_CMD_PARSE_DEMO				1			// Demonstracje parserów argumentów
#define USE_CMD_TASK_DEMO				1			// Demonstracyjne taski

#define OS_CONFIG_DONE

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
