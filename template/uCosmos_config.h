#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

#if PRODUCT_DEMO

	// Task manager config
	#define OS_TASK_MAXCOUNT				uint8_t(10)		// Maximum number of tasks running simultaneously
	#define OS_TICK_PERIOD_MS				uint16_t(10)	// Period of the system timer in ms, range 1-1000
	#define OS_SHOW_SPLASH_SCREEN_AT_START	true			// Should the system logo be displayed at startup?

	// Diagnostic tools for measuring task times
	#define DIAG_USE						true
	#define DIAG_COUNT						20
	
	// Console config
	#define CONSOLE_USE						true
	#define CONSOLE_TASK_PERIOD_MS			uint16_t(20)
	#define CONSOLE_COMMAND_LENGTH			size_t(250)
	#define CONSOLE_MAX_ARGC				11				// Max count of arguments, including command name
	#define CONSOLE_USE_HELP				true
	#define CONSOLE_USE_CTRL_Z				true

	// Commands
	#define USE_CMD_ALL						true			// Command "?" to display all the available commands
	#define USE_CMD_EXIT					true			// Command "exit" to close the system
	#define USE_CMD_TIME					true			// Command "time" to print the time
	#define USE_CMD_TASK_MONITOR			true			// Command "`" to print all running tasks
	#define USE_CMD_TASK_COMMANDS			true			// Commands for manual task management
	#define USE_CMD_PARSE_DEMO				true			// Demo of argument parsers
	#define USE_CMD_TASK_DEMO				true			// Demo of tasks

	#define OS_CONFIG_DONE
#endif

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
