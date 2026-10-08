#include "../config.h"
#if COMPONENT_UCOSMOS

#include "console.h"

#include "uCosmos.h"
#include "commands.h"

// ========================================
// Basic commands
// ========================================

// System reboot
void reset_cmd(int argc, char * argv[]) {
	#if ESP_PLATFORM
		esp_restart();
	#elif PICO_RP2040 || PICO_RP2350
		// TODO
	#endif
}

// Memory status
void memory_status_cmd(int argc, char * argv[]) {
	#if ESP_PLATFORM
		printf("Heap size: %ld\n", esp_get_free_heap_size());
		printf("Hear min:  %ld\n", esp_get_minimum_free_heap_size());
	#elif PICO_RP2040 || PICO_RP2350
		
	#endif
}

// ========================================
// Task Monitor
// ========================================

// Print taks table
#if USE_CMD_TASK_MONITOR
void task_monitor_cmd(int argc, char * argv[]) {
	printf(TEXT_WHITE_BRIGHT "No\tPtr\t\tPer[ms]\tName\n" FORMAT_RESET);
	
	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
		printf("%u\t%08lX\t%lu\t%s\n", i, uint32_t(task_table[i].task_ptr), uint32_t(task_table[i].period) * OS_TICK_PERIOD_MS, task_get_name(task_table[i].task_ptr));
	}
	
	#if ESP_PLATFORM
		uint32_t ClockFreq;
		esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &ClockFreq);
		printf("F_CPU:\t\t%lu MHz\n", ClockFreq / 1000000);
	#elif PICO_RP2040 || PICO_RP2350
		// TODO
	#endif
	
	printf("TickTime:\t%u ms\n", OS_TICK_PERIOD_MS);
}
#endif

// ========================================
// Task control commands (add, close, kill)
// ========================================

#if USE_CMD_TASK_COMMANDS
	
	// Add new task with requested period and phase shift
	void task_add_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("pointer[HEX] period[DEC]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint32_t ptr;
		if(new_parse_hex(argv[1], &ptr)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Argument 2 - period
		uint16_t period;
		if(parse_dec16(argv[2], &period)) return;
		
		// Execute command
		task_add(task_ptr, period);
	}
	
	// close task with its destructor
	void task_close_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("pointer[HEX]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint32_t ptr;
		if(new_parse_hex(argv[1], &ptr)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Execute command
		task_close(task_ptr);
	}
	
	// Change task period
	void task_period_change_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("pointer[HEX] period[DEC]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint32_t ptr;
		if(new_parse_hex(argv[1], &ptr)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Argument 2 - period
		uint16_t period;
		if(parse_dec16(argv[2], &period)) return;
		
		// Execute command
		task_period_change(task_ptr, period);
	}

	// Execute task manually
	void task_exe_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("pointer[HEX]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint32_t ptr;
		if(new_parse_hex(argv[1], &ptr)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Execute command
		task_ptr(os_run);
	}
#endif

// ========================================
// Time
// ========================================

// Print system time in YYYY-MM-DD hh:mm:ss format
#if USE_CMD_TIME
	void time_print_cmd(int argc, char * argv[]) {
		time_t time_now;
		time(&time_now);

		tm time_struct;
		if(time_now == 0xFFFFFFFF) time_now = 0;
		gmtime_r(&time_now, &time_struct);

		if(time_struct.tm_year == 70) {
			time_struct.tm_year = 100;
		}
		
		printf("20%02u-%02u-%02u %02u:%02u:%02u\n",
			time_struct.tm_year - 100,
			time_struct.tm_mon + 1,
			time_struct.tm_mday,
			time_struct.tm_hour,
			time_struct.tm_min,
			time_struct.tm_sec
		);
	}
#endif

// ========================================
// Demo tasks
// ========================================

#if USE_CMD_TASK_DEMO
	void demo1_task(run_mode_t run_mode) {
		static int counter = 0;

		if(run_mode == os_run) {
			LOGI("%s %d", __func__, counter++);
		}
		
		else if(run_mode == os_constructor) {
			LOGI("%s constructor", __func__);
		}
		
		else if(run_mode == os_destructor) {
			LOGI("%s destructor", __func__);
		}

		else if(run_mode == os_id) {
			task_name = __func__;
		}
	}

	void demo2_task(run_mode_t run_mode) {
		static int counter = 0;

		if(run_mode == os_run) {
			LOGD("%s %d", __func__, counter++);
		}
		
		else if(run_mode == os_constructor) {
			LOGD("%s constructor", __func__);
		}
		
		else if(run_mode == os_destructor) {
			LOGD("%s destructor", __func__);
		}

		else if(run_mode == os_id) {
			task_name = __func__;
		}
	}

	void demo1_add_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("period[DEC]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint16_t period;
		if(parse_dec16(argv[1], &period)) return;
		
		// Execute command
		task_add(demo1_task, period);
	}

	void demo2_add_cmd(int argc, char * argv[]) {
		if(argc == 1) {
			#if CONSOLE_USE_HELP
				printf("period[DEC]\n");
			#endif
			return;
		}
		
		// Argument 1 - task pointer
		uint16_t period;
		if(parse_dec16(argv[1], &period)) return;
		
		// Execute command
		task_add(demo2_task, period);
	}

	void demo1_cls_cmd(int argc, char * argv[]) {
		task_close(demo1_task);
	}

	void demo2_cls_cmd(int argc, char * argv[]) {
		task_close(demo2_task);
	}
#endif

#if USE_CMD_PARSE_DEMO

// Print all provided arguments
void args_cmd(int argc, char * argv[]) {
	printf("argc = %u\n", argc);
	
	for(uint8_t i=0; i<argc; i++) {
		printf("%u:\t%08lX\t%s\n", i, (uint32_t)argv[i], (const char *)argv[i]);
	}
}

// Print first argument
void echo_cmd(int argc, char * argv[]) {
	if(argc != 1) {
		printf("%s", argv[1]);
	}
	printf("\n");
}

// void hex8_cmd(int argc, char * argv[]) {
// 	uint8_t value;
// 	if(new_parse_hex(argv[1], &value)) return;
// 	printf("%u\n", value);
// }

// void hex16_cmd(int argc, char * argv[]) {
// 	uint16_t value;
// 	if(new_parse_hex(argv[1], &value)) return;
// 	printf("%u\n", value);
// }

// void hex32_cmd(int argc, char * argv[]) {
// 	uint32_t value;
// 	if(new_parse_hex(argv[1], &value)) return;
// 	printf("%lu\n", value);
// }

void new_hex8_cmd(int argc, char * argv[]) {
	uint8_t value;
	if(new_parse_hex(argv[1], &value)) return;
	printf("%u\n", value);
}

void new_hex16_cmd(int argc, char * argv[]) {
	uint16_t value;
	if(new_parse_hex(argv[1], &value)) return;
	printf("%u\n", value);
}

void new_hex32_cmd(int argc, char * argv[]) {
	uint32_t value;
	if(new_parse_hex(argv[1], &value)) return;
	printf("%lu\n", value);
}

void new_hex64_cmd(int argc, char * argv[]) {
	uint64_t value;
	if(new_parse_hex(argv[1], &value)) return;
	printf("%llu\n", value);
}

void dec8_cmd(int argc, char * argv[]) {
	uint8_t value = 0;
	if(parse_dec8(argv[1], &value, 100)) return;
	printf("%u\n", value);
}

void dec16_cmd(int argc, char * argv[]) {
	uint16_t value = 0;
	if(parse_dec16(argv[1], &value, 10000)) return;
	printf("%u\n", value);
}

void dec16s_cmd(int argc, char * argv[]) {
	int16_t value = 0;
	if(parse_dec16s(argv[1], &value)) return;
	printf("%d\n", value);
}

void dec32_cmd(int argc, char * argv[]) {
	uint32_t value = 0;
	if(parse_dec32(argv[1], &value, 1000000)) return;
	printf("%lu\n", value);
}

void dec32s_cmd(int argc, char * argv[]) {
	int32_t value = 0;
	if(parse_dec32s(argv[1], &value)) return;
	printf("%ld\n", value);
}

void hexstr_cmd(int argc, char * argv[]) {
	uint8_t buffer[64];
	uint8_t length;
	if(parse_hex_string(argv[1], buffer, &length)) return;
	printf("length: %u\n", length);
	printf("ASC: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%c", buffer[i]);
	}
	printf("\nHEX: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

void ascstr_cmd(int argc, char * argv[]) {
	uint8_t buffer[16];
	uint8_t length;
	if(parse_ascii_string(argv[1], buffer, &length, sizeof(buffer), 3)) return;
	printf("length: %u\n", length);
	printf("ASC: %s\n", buffer);
	printf("HEX: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

void ascchr_cmd(int argc, char * argv[]) {
	uint8_t chr;
	if(parse_ascii_char(argv[1], &chr)) return;
	printf("%c\n", chr);
}

#endif
#endif
