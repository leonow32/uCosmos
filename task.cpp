#include "../_config/_config.h"
#include "log.h"
#include "task.h"
#include "uCosmos.h"
#include <string.h>

#if ESP_PLATFORM
	#include "esp_timer.h"
	#include "freertos/task.h"
	static esp_timer_handle_t timer_handle;
	static portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
	#define IRQ_DISABLE() portENTER_CRITICAL(&mutex);
	#define IRQ_ENABLE()  portEXIT_CRITICAL(&mutex);
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
	#include "hardware/sync.h"
	static struct repeating_timer timer = {};
	#define IRQ_DISABLE() uint32_t irq_status = save_and_disable_interrupts();
	#define IRQ_ENABLE()  restore_interrupts_from_disabled(irq_status);
#endif

uint64_t get_time_us(void) {
	#if ESP_PLATFORM
		return uint64_t(esp_timer_get_time());
	#elif PICO_RP2040 || PICO_RP2350
		return time_us_64();
	#else
		#warning "Platform not supported"
		return 0;
	#endif
}

const char * task_name = nullptr;
volatile bool exit_request = false;
volatile task_control_t task_table[TASK_MAXCOUNT];

// ========================================
// Task disgnostics
// ========================================

#if DIAG_USE
struct diag_t {
	void (*task_ptr)(run_mode_t);
	uint32_t run_count;
	uint32_t overrun_count;
	uint64_t time_total;
	uint64_t time_max;
};

static diag_t   diag_table[DIAG_COUNT] = {};
static uint64_t diag_begin = 0;

static void diag_add(void (*task_ptr)(run_mode_t)) {
	for(int i=0; i<DIAG_COUNT; i++) {
		if(diag_table[i].task_ptr == task_ptr) {
			return;
		}

		if(diag_table[i].task_ptr == nullptr) {
			diag_table[i].task_ptr = task_ptr;
			return;
		}
	}

	LOGE("%s(%s) - no free space", __func__, task_get_name(task_ptr));
}

static void diag_update(void (*task_ptr)(run_mode_t), uint64_t time, uint16_t period) {
	uint64_t period_us = uint64_t(period) * uint64_t(TICK_PERIOD_MS) * 1000;

	for(int i=0; i<DIAG_COUNT; i++) {
		if(diag_table[i].task_ptr == task_ptr) {
			if(diag_table[i].run_count == UINT32_MAX)			return;
			if(diag_table[i].overrun_count == UINT32_MAX)		return;
			if(diag_table[i].time_total > (UINT64_MAX - time))	return;
			if(time > diag_table[i].time_max) 					diag_table[i].time_max = time;
			if(time > period_us) 								diag_table[i].overrun_count++;
			diag_table[i].run_count++;
			diag_table[i].time_total += time;
			return;
		}
	}
}

void diag_clear_cmd(int argc, char * argv[]) {
	for(int i=0; i<DIAG_COUNT; i++) {
		diag_table[i].run_count     = 0;
		diag_table[i].overrun_count = 0;
		diag_table[i].time_max      = 0;
		diag_table[i].time_total    = 0;
	}
	diag_begin = get_time_us();
	print_ok();
}

#endif

#if USE_CMD_TASK_MONITOR
void task_monitor_cmd(int argc, char * argv[]) {
	printf(TEXT_WHITE_BRIGHT " No  Pointer   Per [ms] Name\n" FORMAT_RESET);
	
	for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
		if(task_table[i].task_ptr) {
			printf("%3u %08lX %10lu %s\n", 
				i, 
				uint32_t(task_table[i].task_ptr), 
				uint32_t(task_table[i].period) * TICK_PERIOD_MS, 
				task_get_name(task_table[i].task_ptr));
		}
	}
	
	printf("TickTime: %u ms\n", TICK_PERIOD_MS);

	#if DIAG_USE
		uint64_t period = get_time_us() - diag_begin;
		double   percent_total = 0.0;

		// Bubble sort
		#if DIAG_SORT_BEFORE_PRINT
			ExecutionTimer sort_timer = ExecutionTimer();

			for(int i=0; i<DIAG_COUNT-1; i++) {
				for(int j=0; j<DIAG_COUNT-1-i; j++) {
					if(diag_table[j].time_total < diag_table[j+1].time_total) {
						diag_t temp     = diag_table[j];
						diag_table[j]   = diag_table[j+1];
						diag_table[j+1] = temp;
					}
				}
			}

			sort_timer.print("Sort time");
		#endif

		printf("\n" TEXT_WHITE_BRIGHT " No  Run count   Overruns            Total [ms]              Max [ms]              Avg [ms]  Usage [%%]  Name\n" FORMAT_RESET);
		for(uint8_t i=0; i<DIAG_COUNT; i++) {
			if(diag_table[i].task_ptr) {
				uint32_t count   = diag_table[i].run_count;
				uint32_t over    = diag_table[i].overrun_count;
				uint64_t total   = diag_table[i].time_total;
				uint64_t max     = diag_table[i].time_max;
				uint64_t avg     = count ? total / count : 0;
				double   percent = period ? double(total) * 100.0 / double(period) : 0;
				percent_total   += percent;

				printf("%3u %10lu %10lu %17llu.%03u %17llu.%03u %17llu.%03u %10.6f  %s\n", 
					i, 
					count,
					over,
					total / 1000,    uint16_t(total % 1000),
					max / 1000,      uint16_t(max % 1000),
					avg / 1000,      uint16_t(avg % 1000),
					percent,
					task_get_name(diag_table[i].task_ptr)
				);
			}
		}

		period = period / 1000000ul;
		uint8_t seconds = period % 60;
		period = period / 60;
		uint8_t minutes = period % 60;
		period = period / 60;
		uint8_t hours = period % 24;
		period = period / 24;
		printf("Measurement time: %llu days %02u:%02u:%02u\n", period, hours, minutes, seconds);
		printf("Total CPU usage:  %.6f %%\n", percent_total);
	#endif
}
#endif

// ========================================
// Debug
// ========================================

static const char * debug_res(os_t result) {
	switch(result) {
		case os_ok:							return("ok");
		case os_nullptr:					return("nullptr");
		case os_no_free_slot:				return("no free slot");
		case os_not_found:					return("not found");
		case os_task_already_created:		return("already created");
		case os_task_period_under_range:	return("period under range");
	}

	return "?";
}

static os_t debug_print(os_t res, const char * function_name, void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	#if ESP_PLATFORM
		printf("%s%c (%lu) %s: %s(%s, %u): %s" FORMAT_RESET "\n", res ? TEXT_RED : TEXT_GREEN, res ? 'E' : 'D', esp_log_timestamp(), __FILE_NAME__, function_name, task_get_name(task_ptr), period_ms, debug_res(res));
	#elif PICO_RP2040 || PICO_RP2350
		printf("%s%c (%llu) %s: %s(%s, %u): %s" FORMAT_RESET "\n", res ? TEXT_RED : TEXT_GREEN, res ? 'E' : 'D', time_us_64() / 1000, __FILE_NAME__, function_name, task_get_name(task_ptr), period_ms, debug_res(res));
	#else
		#warning "Platform not supported"
	#endif
	return res;
}

// ========================================
// Static functions
// ========================================

// The function finds the first available slot and returns its number via a pointer. If no slots are available, it returns os_no_free_slot.
static os_t task_find_free_slot(uint8_t * slot_number) {
	for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
		if(task_table[i].task_ptr == nullptr) {
			*slot_number = i;
			return os_ok;
		}
	}
	
	return os_no_free_slot;
}

// Check whether the task provided as an argument is present in the task_table array.
static bool task_check_running(void (*task_ptr)(run_mode_t)) {
	for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
		if(task_table[i].task_ptr == task_ptr) {
			return true;
		}
	}

	return false;
}

// Look for a slot that contains a function with the specified `task_ptr` value.
// - task_ptr - task pointer to be found
// - slot_number - a pointer to a variable in which the result will be returned, or nullptr if not used
static os_t task_find(void (*task_ptr)(run_mode_t), uint8_t * slot_number = nullptr) {
	if(task_ptr == nullptr)					return os_nullptr;
	
	for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
		if(task_table[i].task_ptr == task_ptr) {
			if(slot_number != nullptr) {
				*slot_number = i;
			}
			return os_ok;
		}
	}
	
	return os_not_found;
}

// ========================================
// System tick
// ========================================

// RTC interrupt used for the system timer
#if ESP_PLATFORM
	void task_system_tick(void *pvParameter) {
		IRQ_DISABLE();
		for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {						// Check all the items in task_table array
			if(task_table[i].task_ptr != nullptr) {
				if(task_table[i].counter == 0) {						// If the counter is zero then set the flag to mark this task to be executed in the next scheduler's loop
					task_table[i].flag = true;
					task_table[i].counter = task_table[i].period - 1;	// Re-entering the time to be counted down, decremented by 1 (for this reason, period_ms cannot be initialized to 0)
				}
				else {
					task_table[i].counter--;							// Nothing is happening – we are counting down the time
				}
			}
		}
		IRQ_ENABLE();
	}
#elif PICO_RP2040 || PICO_RP2350
	bool task_system_tick(struct repeating_timer *t) {
	for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {							// Check all the items in task_table array
		if(task_table[i].task_ptr != nullptr) {
			if(task_table[i].counter == 0) {							// If the counter is zero then set the flag to mark this task to be executed in the next scheduler's loop
				task_table[i].flag = true;
				task_table[i].counter = task_table[i].period - 1;		// Re-entering the time to be counted down, decremented by 1 (for this reason, period_ms cannot be initialized to 0)
			}
			else {
				task_table[i].counter--;								// Nothing is happening – we are counting down the time
			}
		}
	}
    return true;
}
#endif

// ========================================
// Task management
// ========================================

void task_init(void) {
	LOGD("init");
	exit_request = false;

	for(uint8_t slot_number=0; slot_number<TASK_MAXCOUNT; slot_number++) {
		task_table[slot_number].task_ptr = nullptr;
		task_table[slot_number].counter  = 0;
		task_table[slot_number].period   = 0;
		task_table[slot_number].flag     = false;
	}

	#if ESP_PLATFORM
		const esp_timer_create_args_t timer_config = {
			.callback 				= &task_system_tick,
			.arg 					= nullptr,
			.dispatch_method 		= ESP_TIMER_TASK,
			.name 					= "uCosmosTick",
			.skip_unhandled_events	= false,
		};

		ESP_ERROR_CHECK(esp_timer_create(&timer_config, &timer_handle));
		ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, TICK_PERIOD_MS * 1000));  // here time is in micro seconds
	#elif PICO_RP2040 || PICO_RP2350
		add_repeating_timer_ms(-TICK_PERIOD_MS, task_system_tick, nullptr, &timer);
	#else
		#error "Platform not supported"
	#endif
}

void task_deinit(void) {
	exit_request = true;
}

void task_scheduler(void) {
	while(1) {
		for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
			IRQ_DISABLE();
			void (*task_ptr)(run_mode_t) = task_table[i].task_ptr;
			bool flag = task_table[i].flag;
			#if DIAG_USE
				uint16_t period = task_table[i].period;
			#endif
			task_table[i].flag = false;
			IRQ_ENABLE();

			if(flag && task_ptr) {
				#if DIAG_USE
					ExecutionTimer execution_timer = ExecutionTimer();
				#endif
				
				task_ptr(os_run);													// Task execution

				#if DIAG_USE
					diag_update(task_ptr, execution_timer.get(), period);
				#endif
			}
		}

		if(exit_request) {
			LOGI("exit request");

			for(uint8_t i=0; i<TASK_MAXCOUNT; i++) {
				IRQ_DISABLE();
				void (*task_ptr)(run_mode_t) = task_table[i].task_ptr;
				IRQ_ENABLE();

				if(task_ptr) {
					task_close(task_ptr);
				}
			}

			#if ESP_PLATFORM
				esp_timer_stop(timer_handle);
				esp_timer_delete(timer_handle);
			#elif PICO_RP2040 || PICO_RP2350
				cancel_repeating_timer(&timer);
			#else
				#error "Platform not supported"
			#endif

			return;
		}

		#if ESP_PLATFORM
			vTaskDelay(1);
		#elif PICO_RP2040 || PICO_RP2350
			tight_loop_contents();
			__asm("  wfi");
		#endif
	}
}

//Adding a task to the task array and invoking its constructor
os_t task_add(void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, period_ms);
	if(period_ms < TICK_PERIOD_MS)								return debug_print(os_task_period_under_range, __func__, task_ptr, period_ms);
	IRQ_DISABLE();
	if(task_check_running(task_ptr)) 							{IRQ_ENABLE(); return debug_print(os_task_already_created, __func__, task_ptr, period_ms);}
	if(task_find_free_slot(&slot_number) == os_no_free_slot)	{IRQ_ENABLE(); return debug_print(os_no_free_slot, __func__, task_ptr, period_ms);}

	task_table[slot_number].counter  = (period_ms / TICK_PERIOD_MS)-1;
	task_table[slot_number].period   = period_ms / TICK_PERIOD_MS;
	task_table[slot_number].flag     = false;
	task_table[slot_number].task_ptr = task_ptr;
	IRQ_ENABLE();

	#if DIAG_USE
		diag_add(task_ptr);
	#endif

	task_ptr(os_constructor);
	return debug_print(os_ok, __func__, task_ptr, period_ms);
}

// Execute task destructor and then remove it from the array
os_t task_close(void (*task_ptr)(run_mode_t)) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, 0);
	IRQ_DISABLE();
	if(task_find(task_ptr, &slot_number)) 						{IRQ_ENABLE(); return debug_print(os_not_found, __func__, task_ptr, 0);}
	
	task_table[slot_number].counter  = 0;
	task_table[slot_number].period   = 0;
	task_table[slot_number].flag     = false;
	task_table[slot_number].task_ptr = nullptr;									// Clear task from the array
	IRQ_ENABLE();

	task_ptr(os_destructor);													// Execute task destructor
	return debug_print(os_ok, __func__, task_ptr, 0);
}

// Change the period of task execution
os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, period_ms);
	if(period_ms < TICK_PERIOD_MS)								return debug_print(os_task_period_under_range, __func__, task_ptr, period_ms);
	IRQ_DISABLE();
	if(task_find(task_ptr, &slot_number))						{IRQ_ENABLE(); return debug_print(os_not_found, __func__, task_ptr, period_ms);}
	
	task_table[slot_number].counter	= (period_ms / TICK_PERIOD_MS)-1;
	task_table[slot_number].period	= period_ms / TICK_PERIOD_MS;
	task_table[slot_number].flag	= 0;
	IRQ_ENABLE();
	return debug_print(os_ok, __func__, task_ptr, period_ms);
}

// Checking whether a task has been added to the array of tasks (rather than whether it is currently being executed)
bool task_is_running(void (*task_ptr)(run_mode_t)) {
	IRQ_DISABLE();
	bool result = task_check_running(task_ptr);
	IRQ_ENABLE();
	return result;
}

// Retrieving the name of a task provided as an argument. IMPORTANT – this function is not re-entrant. 
// It uses a global variable, `task_name`. Each task must store its name in flash memory; it cannot be an array that is dynamically created and destroyed.
const char * task_get_name(void (*task_ptr)(run_mode_t)) {
	if(task_ptr) {
		task_name = "unknown";
		task_ptr(os_id);
		return task_name;
	}
	else {
		return "nullptr";
	}
}
