#include "../config.h"
#if COMPONENT_UCOSMOS

#include "uCosmos.h"
#include "uCosmos_commands.h"
#include "../uCosmos/console.h"

using namespace console;

// ========================================
// Basic commands
// ========================================

// System reboot
void reset_cmd(int argc, char * argv[]) {
	#if ESP_PLATFORM
		esp_restart();
	#elif PICO_RP2040 || PICO_RP2350
		
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
#if OS_USE_TASK_MONITOR
void task_monitor_cmd(int argc, char * argv[]) {
	
	// Print header
	printf(TEXT_WHITE_BRIGHT "No\tPtr\t\tFlag\tPer[ms]\tId\n" FORMAT_RESET);
	
	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
		printf("%u\t%08lX\t%02X\t%lu\t", i, uint32_t(task_table[i].task_ptr), task_table[i].flag, uint32_t(task_table[i].period) * OS_TICK_PERIOD_MS);
		
		#if OS_USE_TASK_IDENTIFY
			if(task_table[i].task_ptr != NULL) task_table[i].task_ptr(os_id);
		#endif

		printf("\n");
	}
	
	#if ESP_PLATFORM
		uint32_t ClockFreq;
		esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &ClockFreq);
		printf(TEXT_WHITE_BRIGHT "F_CPU:\t\t%lu MHz\n" FORMAT_RESET, ClockFreq / 1000000);
	#elif PICO_RP2040 || PICO_RP2350
		
	#endif
	
	printf(TEXT_WHITE_BRIGHT "TickTime:\t" FORMAT_RESET "%u ms\n", OS_TICK_PERIOD_MS);
}
#endif

// ========================================
// Task control commands (add, close, kill)
// ========================================

#if OS_USE_TASK_COMMANDS
	
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
		if(parse_hex32(argv[1], &ptr)) return;
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
		if(parse_hex32(argv[1], &ptr)) return;
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
		if(parse_hex32(argv[1], &ptr)) return;
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
		if(parse_hex32(argv[1], &ptr)) return;
		void (*task_ptr)(run_mode_t) = (void (*)(run_mode_t))(ptr);
		
		// Execute command
		task_ptr(os_run);
	}
#endif

// ========================================
// Time
// ========================================

// Print system time in YYYY-MM-DD hh:mm:ss format
#if OS_USE_TIME_COMMAND
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

#if OS_USE_DEMO_TASKS
	void demo1_task(run_mode_t run_mode) {
		static int counter = 0;

		if(run_mode == os_run) {
			printf("%s %d\n", __func__, counter++);
		}
		
		else if(run_mode == os_constructor) {
			printf("%s constructor\n", __func__);
		}
		
		else if(run_mode == os_destructor) {
			printf("%s destructor\n", __func__);
		}
		
		#if OS_USE_TASK_IDENTIFY
		else if(run_mode == os_id) {
			printf(__func__);
		}
		#endif
	}

	void demo2_task(run_mode_t run_mode) {
		static int counter = 0;

		if(run_mode == os_run) {
			printf("%s %d\n", __func__, counter++);
		}
		
		else if(run_mode == os_constructor) {
			printf("%s constructor\n", __func__);
		}
		
		else if(run_mode == os_destructor) {
			printf("%s destructor\n", __func__);
		}
		
		#if OS_USE_TASK_IDENTIFY
		else if(run_mode == os_id) {
			printf(__func__);
		}
		#endif
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


#endif