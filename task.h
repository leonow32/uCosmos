#ifndef UCOSMOS_TASK_H
#define UCOSMOS_TASK_H

#include "../_config/uCosmos_config.h"

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

// Return value for task managemen functions
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
	volatile uint16_t counter;
	volatile uint16_t period;
	volatile bool flag;
};

// Task management
extern volatile task_control_t task_table[OS_TASK_MAXCOUNT];
extern const char * task_name;

void task_init(void);
void task_scheduler(void);
os_t task_add(void (*task_ptr)(run_mode_t), uint16_t period_ms);
os_t task_close(void (*task_ptr)(run_mode_t));
os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms);
bool task_is_running(void (*task_ptr)(run_mode_t));
const char * task_get_name(void (*task_ptr)(run_mode_t));

#endif
