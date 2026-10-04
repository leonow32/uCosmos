#ifndef CONSOLE_CONFIG_H_
#define CONSOLE_CONFIG_H_

#define CONSOLE_COMMAND_LENGTH				250
#define CONSOLE_MAX_ARGUMENTS				11
#define CONSOLE_USE_HELP					1
#define CONSOLE_USE_CTRL_Z					1
#define CONSOLE_USE_COMMAND_ALL				1
#define CONSOLE_USE_DEMO_COMMANDS			1

#define CONSOLE_CONFIG_DONE

// ========================================
// Error handling
// ========================================

#ifndef CONSOLE_CONFIG_DONE
	#error "Missing config"
#endif


#endif /* CONSOLE_CONFIG_H_ */