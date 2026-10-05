#ifndef UCOSMOS_H
#define UCOSMOS_H

#include <stdio.h>
#include <time.h>

#if ESP_PLATFORM
	#include "esp_log.h"
	#include "esp_timer.h"
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
#endif

#include "uCosmos_config.h"
#include "log.h"

// Task exection mode
enum run_mode_t {
	os_run = 0,
	os_constructor,
	os_destructor,
};

// Return value for task managemen functions
enum os_t {
	os_ok = 0,
	os_error,
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
	bool flag;								// If the task is set to be executed or has to be executed continuously
	const char * name;
};

// Task management
extern volatile task_control_t task_table[OS_TASK_MAXCOUNT];
void    os_init(void);
void    task_scheduler(void);
#define task_add(task_ptr, period_ms) task_add_name(task_ptr, period_ms, #task_ptr)
os_t    task_add_name(void (*task_ptr)(run_mode_t), uint16_t period_ms, const char * name);
os_t    task_clear(uint8_t slot_number);
os_t    task_close(void (*task_ptr)(run_mode_t));
os_t    task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms);
os_t    task_find_free_slot(uint8_t * slot_number);
os_t    task_find(void (*task_ptr)(run_mode_t), uint8_t * slot_number = nullptr);
bool    task_is_running(void (*task_ptr)(run_mode_t));


#endif
