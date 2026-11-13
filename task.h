#ifndef UCOSMOS_TASK_H
#define UCOSMOS_TASK_H

#include "../_config/_config.h"
#include "../_config/uCosmos_config.h"
#include <cstdio>

#if ESP_PLATFORM
	#include "esp_timer.h"
	#include "freertos/FreeRTOS.h"
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
#endif

// Task exection mode
enum run_mode_t {
	os_run = 0,
	os_constructor,
	os_destructor,
	os_id,
};

// Return value for task management functions
enum os_t {
	os_ok = 0,
	os_nullptr,
	os_no_free_slot,
	os_not_found,
	os_task_already_created,
	os_slot_number_over_range,
	os_task_period_under_range,
};

// Task control structure
struct task_control_t {
	void (*task_ptr)(run_mode_t);
	uint16_t counter;
	uint16_t period;
	bool flag;
};

// Task management
extern volatile task_control_t task_table[OS_TASK_MAXCOUNT];
extern const char * task_name;

void task_init(void);
void task_deinit(void);
void task_scheduler(void);
os_t task_add(void (*task_ptr)(run_mode_t), uint16_t period_ms);
os_t task_close(void (*task_ptr)(run_mode_t));
os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms);
bool task_is_running(void (*task_ptr)(run_mode_t));
const char * task_get_name(void (*task_ptr)(run_mode_t));
uint64_t get_time_us(void);

// Task diagnostics
#if DIAG_USE
	void diag_cmd(int argc, char * argv[]);
#endif

class ExecutionTimer {
	private:
		uint64_t time_begin;
		uint64_t time_end;

	public:
		ExecutionTimer() {
			time_end   = 0;
			time_begin = get_time_us();
		}

		void begin(void) {
			time_end   = 0;
			time_begin = get_time_us();
		}

		void end(void) {
			time_end = get_time_us();
		}

		uint64_t get(void) {
			uint64_t time_elapsed = time_end ? (time_end - time_begin) : (get_time_us() - time_begin);
			return time_elapsed;
		}

		void print(const char * text = nullptr) {
			uint64_t time_elapsed = get();
			uint64_t milliseconds = time_elapsed / 1000;
			uint16_t microseconds = time_elapsed % 1000;

			printf("%s%s%llu.%03u ms\n", 
				text ? text : "",
				text ? ": " : "",
				milliseconds,
				microseconds
			);
		}
};

#endif
