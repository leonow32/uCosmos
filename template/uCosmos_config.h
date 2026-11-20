#ifndef UCOSMOS_CONFIG_H_
#define UCOSMOS_CONFIG_H_

#define UCOSMOS_CONFIG_VERSION				261120

#if PRODUCT_DEMO

	// Task manager config
	#define TASK_MAXCOUNT					10				// Maximum number of tasks running simultaneously
	#define TICK_PERIOD_MS					10				// Period of the system timer in ms, range 1-1000
	#define SHOW_SPLASH_SCREEN_AT_START		true			// Should the system logo be displayed at startup?
	
	// Console config
	#define CONSOLE_USE						true			// Should a command-line task be launched automatically upon system startup
	#define CONSOLE_TASK_PERIOD_MS			20				// Time between console task execution
	#define CONSOLE_COMMAND_LENGTH			250				// Size of the command buffer in RAM
	#define CONSOLE_MAX_ARGC				11				// Max count of arguments, including command name
	#define CONSOLE_USE_HELP				true			// Enable CMD_HELP macro in all commands whick support it
	#define CONSOLE_USE_CTRL_Z				true			// CTRL-Z recalls the last used command

	// Available console commands
	#define USE_CMD_ALL						true			// Command "?" to display all the available commands
	#define USE_CMD_EXIT					true			// Command "exit" to close the system
	#define USE_CMD_TIME					true			// Command "time" to print the time
	#define USE_CMD_COLORS					true			// Command "colors" to demonstrate how to use text color and formatting features 
	#define USE_CMD_TASK_MONITOR			true			// Command "`" to print all running tasks
	#define USE_CMD_TASK_COMMANDS			true			// Commands for manual task management
	#define USE_CMD_PARSE_DEMO				true			// Demo of argument parsers
	#define USE_CMD_TASK_DEMO				true			// Demo of tasks

	// Diagnostic tools for measuring task times
	#define DIAG_USE						true			// Include diagnostic functions to track the time spent by all the executed tasks
	#define DIAG_COUNT						20				// Maximum number of tasks to be tracked
	#define DIAG_SORT_BEFORE_PRINT			true			// Display tasks in order from most time-consuming

	#define OS_CONFIG_DONE
#endif

// ========================================
// Error handling
// ========================================

#ifndef OS_CONFIG_DONE
	#error "Missing config"
#endif

#endif /* UCOSMOS_CONFIG_H_ */
