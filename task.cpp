#include "../_config/_config.h"
#include "log.h"
#include "task.h"
#include "uCosmos.h"

#if ESP_PLATFORM
	#include "esp_timer.h"
	#include "freertos/task.h"
	#define IRQ_DISABLE ;
	#define IRQ_ENABLE  ;
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
	#include "hardware/sync.h"
	#define IRQ_DISABLE() uint32_t irq_status = save_and_disable_interrupts();
	#define IRQ_ENABLE()  restore_interrupts(irq_status);
#endif

volatile task_control_t task_table[OS_TASK_MAXCOUNT];
const char * task_name = nullptr;

static const char * debug_res(os_t result) {
	switch(result) {
		case os_ok:							return("ok");
		case os_nullptr:					return("nullptr");
		case os_no_free_slot:				return("no free slot");
		case os_not_found:					return("not found");
		case os_task_already_created:		return("already created");
		case os_slot_number_over_range:		return("slot over range");
		case os_task_period_under_range:	return("period under range");
	}

	return "?";
}

static os_t debug_print(os_t res, const char * function_name, void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	#if ESP_PLATFORM
		printf("%s%c (%lu) %s: %s(%s, %u): %s" FORMAT_RESET "\n", res ? TEXT_RED : TEXT_GREEN, res ? 'E' : 'I', esp_log_timestamp(), __FILE_NAME__, function_name, task_get_name(task_ptr), period_ms, debug_res(res));
	#elif PICO_RP2040 || PICO_RP2350
		printf("%s%c (%llu) %s: %s(%s, %u): %s" FORMAT_RESET "\n", res ? TEXT_RED : TEXT_GREEN, res ? 'E' : 'I', time_us_64() / 1000, __FILE_NAME__, function_name, task_get_name(task_ptr), period_ms, debug_res(res));
	#endif
	return res;
}

static void slot_erase(uint8_t slot_number) {
	IRQ_DISABLE();
	task_table[slot_number].task_ptr = nullptr;
	task_table[slot_number].counter  = 0;
	task_table[slot_number].period   = 0;
	task_table[slot_number].flag     = false;
	IRQ_ENABLE();
}

// Funkcja znajduje pierwszy wolny slot i zwraca jego numer przez wskaźnik. Jeśli brak wolnych slotów to zwraca os_no_free_slot.
static os_t task_find_free_slot(uint8_t * slot_number) {
	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {			// Przesukiwanie tablicy slotów
		if(task_table[i].task_ptr == nullptr) {			// Szukanie pierwszego wolnego slotu, w którym wskaźnik do tasku jest nullptr
			*slot_number = i;
			return os_ok;
		}
	}
	
	return os_no_free_slot;								// Jeżeli brak wolnych slotów
}

// Szukanie slotu, w któym znajduje się funkcja o podanym wskaźniku task_ptr
// - task_ptr - wskaźnik do szukaniego tasku
// - slot_number - wskaźnik do zmiennej w której będzie zwrócony wynik
static os_t task_find(void (*task_ptr)(run_mode_t), uint8_t * slot_number = nullptr) {
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, 0);
	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
		if(task_table[i].task_ptr == task_ptr) {
			if(slot_number != nullptr) {				// slot_number to argument opcjonalny, jeśli nie podano to nullptr
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

// Przerwanie RTC używane do Tickera systemowego
#if ESP_PLATFORM
	void os_system_tick(void *pvParameter) {
		for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {						// Sprawdzenie wszystkich procesów
			if(task_table[i].task_ptr != nullptr) {						// Jeżeli w badanym slocie jest wpisany jakiś task
				if(task_table[i].counter == 0) {						// Jeżeli aktualnie teraz licznik ma wartość zero (task może być zainicjalizowany z licznikiem 0)
					task_table[i].flag = true;
					task_table[i].counter = task_table[i].period - 1;	// Ponowne wpisanie czasu do odmierzenia pomniejszonego o 1 (z tego powodu period_ms nie może być zainicjalizowany jako 0)
				}
				else {
					task_table[i].counter--;							// Nic się nie dzieje - odliczamy czas
				}
			}
		}
	}
#elif PICO_RP2040 || PICO_RP2350
	bool os_system_tick(struct repeating_timer *t) {
	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {							// Sprawdzenie wszystkich procesów
		if(task_table[i].task_ptr != nullptr) {							// Jeżeli w badanym slocie jest wpisany jakiś task
			if(task_table[i].counter == 0) {							// Jeżeli aktualnie teraz licznik ma wartość zero (task może być zainicjalizowany z licznikiem 0)
				task_table[i].flag = true;
				task_table[i].counter = task_table[i].period - 1;		// Ponowne wpisanie czasu do odmierzenia pomniejszonego o 1 (z tego powodu period nie może być zainicjalizowany jako 0)				
			}
			else {
				task_table[i].counter--;								// Nic się nie dzieje - odliczamy czas
			}
		}
	}
    return true;         												// Ma się wykonać ponownie zgodnie z harmonogramem
}
#endif

// ========================================
// Task management
// ========================================

void task_init(void) {
	LOGI("init");

	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
		slot_erase(i);
	}

	#if ESP_PLATFORM
		const esp_timer_create_args_t timer_config = {
			.callback 				= &os_system_tick,
			.arg 					= nullptr,
			.dispatch_method 		= ESP_TIMER_TASK,
			.name 					= "uCosmosTick",
			.skip_unhandled_events	= false,
		};

		esp_timer_handle_t handle;
		ESP_ERROR_CHECK(esp_timer_create(&timer_config, &handle));
		ESP_ERROR_CHECK(esp_timer_start_periodic(handle, OS_TICK_PERIOD_MS * 1000));  // here time is in micro seconds
	#elif PICO_RP2040 || PICO_RP2350
		static struct repeating_timer timer = {};
		add_repeating_timer_ms(-OS_TICK_PERIOD_MS, os_system_tick, nullptr, &timer);
	#else
		#error "Platform not supported"
	#endif
}

void task_scheduler(void) {
	while(1) {
		for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
			if(task_table[i].flag) {
				task_table[i].flag = false;
				if(task_table[i].task_ptr) {
					task_table[i].task_ptr(os_run);				// Task execution
				}
			}
		}

		#if ESP_PLATFORM
			vTaskDelay(1);
		#elif PICO_RP2040 || PICO_RP2350
			tight_loop_contents();
			// __asm("  wfi");
		#endif
	}
}

// Dodawanie tasku do tablicy tasków
os_t task_add(void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, period_ms);
	if(period_ms < OS_TICK_PERIOD_MS)							return debug_print(os_task_period_under_range, __func__, task_ptr, period_ms);
	if(task_is_running(task_ptr))								return debug_print(os_task_already_created, __func__, task_ptr, period_ms);
	if(task_find_free_slot(&slot_number) == os_no_free_slot)	return debug_print(os_no_free_slot, __func__, task_ptr, period_ms);
	
	IRQ_DISABLE();
	task_table[slot_number].counter  = (period_ms / OS_TICK_PERIOD_MS)-1;
	task_table[slot_number].period   = period_ms / OS_TICK_PERIOD_MS;
	task_table[slot_number].flag     = false;
	task_table[slot_number].task_ptr = task_ptr;
	IRQ_ENABLE();
	task_ptr(os_constructor);
	return os_ok;
}

// Execute task destructor and then remove it from the array
os_t task_close(void (*task_ptr)(run_mode_t)) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, 0);
	if(task_find(task_ptr, &slot_number))						return debug_print(os_not_found, __func__, task_ptr, 0);
	
	task_ptr(os_destructor);													// Execute task destructor
	slot_erase(slot_number);													// Clear task from the array
	return os_ok;
}

// Zmiana czasów
// - task_ptr  - wskaźnik do procesu, który ma być zmieniony
// - period_ms - nowy okres
os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	uint8_t slot_number;
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, period_ms);
	if(task_find(task_ptr, &slot_number))						return debug_print(os_not_found, __func__, task_ptr, period_ms);
	if(period_ms < OS_TICK_PERIOD_MS)							return debug_print(os_task_period_under_range, __func__, task_ptr, period_ms);

	IRQ_DISABLE();
	task_table[slot_number].counter	= (period_ms / OS_TICK_PERIOD_MS)-1;
	task_table[slot_number].period	= period_ms / OS_TICK_PERIOD_MS;
	task_table[slot_number].flag	= 0;
	IRQ_ENABLE();
	return os_ok;
}

// Sprawdzanie czy task jest dodany do tablicy tasków (a nie czy akcualnie jest w trakcie wykonywania)
bool task_is_running(void (*task_ptr)(run_mode_t)) {
	if(task_ptr == nullptr)										return debug_print(os_nullptr, __func__, task_ptr, 0);
	return task_find(task_ptr) == os_ok;
}

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
