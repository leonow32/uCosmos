#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

#if PRODUCT_DEMO

	// Task manager config
	#define TASK_MAXCOUNT				uint8_t(10)		// Maximum number of tasks running simultaneously
	#define TICK_PERIOD_MS				uint16_t(10)	// Period of the system timer in ms, range 1-1000
	#define SHOW_SPLASH_SCREEN_AT_START	true			// Should the system logo be displayed at startup?
	
	// Console config
	#define CONSOLE_USE						true
	#define CONSOLE_TASK_PERIOD_MS			uint16_t(20)
	#define CONSOLE_COMMAND_LENGTH			size_t(250)
	#define CONSOLE_MAX_ARGC				uint8_t(11)		// Max count of arguments, including command name
	#define CONSOLE_USE_HELP				true
	#define CONSOLE_USE_CTRL_Z				true

	// Available console commands
	#define USE_CMD_ALL						true			// Command "?" to display all the available commands
	#define USE_CMD_EXIT					true			// Command "exit" to close the system
	#define USE_CMD_TIME					true			// Command "time" to print the time
	#define USE_CMD_TASK_MONITOR			true			// Command "`" to print all running tasks
	#define USE_CMD_TASK_COMMANDS			true			// Commands for manual task management
	#define USE_CMD_PARSE_DEMO				true			// Demo of argument parsers
	#define USE_CMD_TASK_DEMO				true			// Demo of tasks

	// Diagnostic tools for measuring task times
	#define DIAG_USE						true
	#define DIAG_COUNT						uint8_t(20)		// Maximum number of tasks to be tracked

	#define OS_CONFIG_DONE
#endif

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
