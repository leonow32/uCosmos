#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

#if PRODUCT_DEMO

	// Konfiguracja systemu
	#define OS_TASK_MAXCOUNT				uint8_t(10)		// Od tego zalezy rozmiar tablicy tasków w RAM
	#define OS_TICK_PERIOD_MS				uint16_t(10)	// Okres timera systemowego w ms, zakres 1-1000
	#define OS_SHOW_SPLASH_SCREEN_AT_START	true			// Czy pokazywać logo systemu na starcie

	// Konfiguracja konsoli
	#define CONSOLE_USE						true
	#define CONSOLE_TASK_PERIOD_MS			uint16_t(20)
	#define CONSOLE_COMMAND_LENGTH			size_t(250)
	#define CONSOLE_MAX_ARGC				11				// Max count of arguments, including command name
	#define CONSOLE_USE_HELP				true
	#define CONSOLE_USE_CTRL_Z				true

	// Dostępne polecenia
	#define USE_CMD_ALL						true			// Polecenie "?" do wyświetlania na konsoli wszystkich dostępnych komend
	#define USE_CMD_TIME					true			// Polecenie "time" do pokazywania czasu
	#define USE_CMD_TASK_MONITOR			true			// Polecenie "`" do wyświetlania tablicy tasków
	#define USE_CMD_TASK_COMMANDS			true			// Polecenia do ręcznego zarządzania taskami
	#define USE_CMD_PARSE_DEMO				true			// Demonstracje parserów argumentów
	#define USE_CMD_TASK_DEMO				true			// Demonstracyjne taski

	#define OS_CONFIG_DONE
#endif

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
