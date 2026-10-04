#if COMPONENT_UCOSMOS
#ifndef UCOSMOS_COMMANDS_H_
#define UCOSMOS_COMMANDS_H_

#if ESP_PLATFORM
	#include "esp_clk_tree.h"
#elif PICO_RP2040 || PICO_RP2350
	#include <stdio.h>	
	#include "pico/stdlib.h"
#endif

#include "uCosmos.h"

// Basic commands
void		reset_cmd(int argc, char * argv[]);
void		memory_status_cmd(int argc, char * argv[]);

#if OS_USE_TASK_MONITOR
	void	task_monitor_cmd(int argc, char * argv[]);
#endif

// Task control commands
#if OS_USE_TASK_COMMANDS
	void	task_add_cmd(int argc, char * argv[]);
	void	task_close_cmd(int argc, char * argv[]);
	void	task_period_change_cmd(int argc, char * argv[]);
	void 	task_exe_cmd(int argc, char * argv[]);
#endif

// System time
#if OS_USE_TIME_COMMAND
	void	time_print_cmd(int argc, char * argv[]);
#endif

// Demonstration tasks
#if OS_USE_DEMO_TASKS
	void demo1_add_cmd(int argc, char * argv[]);
	void demo2_add_cmd(int argc, char * argv[]);
	void demo1_cls_cmd(int argc, char * argv[]);
	void demo2_cls_cmd(int argc, char * argv[]);
#endif

#endif /* UCOSMOS_COMMANDS_H_ */
#endif
