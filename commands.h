#if COMPONENT_UCOSMOS
#ifndef UCOSMOS_COMMANDS_H_
#define UCOSMOS_COMMANDS_H_

#include "log.h"

// Basic commands
void		reset_cmd(int argc, char * argv[]);
void		memory_status_cmd(int argc, char * argv[]);

#if USE_CMD_TASK_MONITOR
	void task_monitor_cmd(int argc, char * argv[]);
#endif

// Task control commands
#if USE_CMD_TASK_COMMANDS
	void task_add_cmd(int argc, char * argv[]);
	void task_close_cmd(int argc, char * argv[]);
	void task_period_change_cmd(int argc, char * argv[]);
	void task_exe_cmd(int argc, char * argv[]);
#endif

// System time
#if USE_CMD_TIME
	void time_print_cmd(int argc, char * argv[]);
#endif

// Demonstration tasks
#if USE_CMD_TASK_DEMO
	void demo1_task(run_mode_t run_mode);
	void demo2_task(run_mode_t run_mode);
	void demo1_add_cmd(int argc, char * argv[]);
	void demo2_add_cmd(int argc, char * argv[]);
	void demo1_cls_cmd(int argc, char * argv[]);
	void demo2_cls_cmd(int argc, char * argv[]);
#endif

#if USE_CMD_PARSE_DEMO
	void args_cmd(int argc, char * argv[]);
	void echo_cmd(int argc, char * argv[]);
	// void hex8_cmd(int argc, char * argv[]);
	// void hex16_cmd(int argc, char * argv[]);
	// void hex32_cmd(int argc, char * argv[]);
	// void dec8_cmd(int argc, char * argv[]);
	// void dec16_cmd(int argc, char * argv[]);
	void dec16s_cmd(int argc, char * argv[]);
	// void dec32_cmd(int argc, char * argv[]);
	void dec32s_cmd(int argc, char * argv[]);
	void hexstr_cmd(int argc, char * argv[]);
	void ascstr_cmd(int argc, char * argv[]);
	void ascchr_cmd(int argc, char * argv[]);

	void new_hex8_cmd(int argc, char * argv[]);
	void new_hex16_cmd(int argc, char * argv[]);
	void new_hex32_cmd(int argc, char * argv[]);
	void new_hex64_cmd(int argc, char * argv[]);
	void new_dec8_cmd(int argc, char * argv[]);
	void new_dec16_cmd(int argc, char * argv[]);
	void new_dec32_cmd(int argc, char * argv[]);
	void new_dec64_cmd(int argc, char * argv[]);
	void new_numeric_cmd(int argc, char * argv[]);
#endif

#endif /* UCOSMOS_COMMANDS_H_ */
#endif
