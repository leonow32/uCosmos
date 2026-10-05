#include "../config.h"
#if COMPONENT_UCOSMOS
static const char *TAG = "uCosmos";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include <stdio.h>
#include <time.h>
#include "log.h"
#include "uCosmos.h"

volatile task_control_t task_table[OS_TASK_MAXCOUNT];

static void os_splash_screen(void) {
	#if ESP_PLATFORM
		printf(TEXT_YELLOW "\n");
		printf("     ___________ ____            __________  _____ __  _______  _____ \n");
		printf("    / ____/ ___// __ \\    __  __/ ____/ __ \\/ ___//  |/  / __ \\/ ___/ \n");
		printf("   / ___/ \\__ \\/ /_/ /   / / / / /   / / / /\\__ \\/ /|_/ / / / /\\__ \\  \n");
		printf("  / /___ ___/ / ____/   / /_/ / /___/ /_/ /___/ / /  / / /_/ /___/ /  \n");
		printf(" /_____/_____/_/       / ____/\\____/\\____//____/_/  /_/\\____//____/   \n");
		printf(TEXT_CYAN);
		for(uint8_t i=0; i<23; i++) {
			printf("-");
		}
		printf(TEXT_YELLOW "\\/" TEXT_CYAN);
		for(uint8_t i=0; i<42; i++) {
			printf("-");
		}
		printf("\n");
	#elif PICO_RP2040 || PICO_RP2350
		printf(TEXT_YELLOW "\n");
		printf("      ____  ____________            __________  _____ __  _______  _____\n");
		printf("     / __ \\/ / ____/ __ \\    __  __/ ____/ __ \\/ ___//  |/  / __ \\/ ___/\n");
		printf("    / /_/ / / /   / / / /   / / / / /   / / / /\\__ \\/ /|_/ / / / /\\__ \\\n");
		printf("   / ____/ / /___/ /_/ /   / /_/ / /___/ /_/ /___/ / /  / / /_/ /___/ /\n");
		printf("  /_/   /_/\\____/\\____/   / ____/\\____/\\____//____/_/  /_/\\____//____/\n");
		printf(TEXT_CYAN);
		for(uint8_t i=0; i<26; i++) {
			printf("-");
		}
		printf(TEXT_YELLOW "\\/" TEXT_CYAN);
		for(uint8_t i=0; i<42; i++) {
			printf("-");
		}
		printf("\n");
	#endif
}

static const char * os_print_reset_source() {
	#if ESP_PLATFORM
		switch(esp_reset_reason()) {
			case ESP_RST_UNKNOWN:   		return "UNKNOWN";
			case ESP_RST_POWERON:   		return "POWERON";
			case ESP_RST_EXT:       		return "EXT";
			case ESP_RST_SW:        		return "SW";
			case ESP_RST_PANIC:     		return "PANIC";
			case ESP_RST_INT_WDT:   		return "INT_WDT";
			case ESP_RST_TASK_WDT:  		return "TASK_WDT";
			case ESP_RST_WDT:       		return "WDT";
			case ESP_RST_DEEPSLEEP: 		return "DEEPSLEEP";
			case ESP_RST_BROWNOUT:  		return "BROWNOUT";
			case ESP_RST_SDIO:      		return "SDIO";
			case ESP_RST_USB:       		return "USB";
			case ESP_RST_JTAG:      		return "JTAG";
			case ESP_RST_EFUSE:     		return "EFUSE";
			case ESP_RST_PWR_GLITCH:		return "PWR_GLITCH";
			case ESP_RST_CPU_LOCKUP:		return "CPU_LOCKUP";
		}
	#elif PICO_RP2040 || PICO_RP2350
		return "UNKNOWN";
	#endif
}

static const char * debug_res(os_t result) {
	switch(result) {
		case os_ok:							return("ok");
		case os_no_free_slot:				return("no free slot");
		case os_not_found:					return("not found");
		case os_task_already_created:		return("already created");
		case os_slot_number_over_range:		return("slot over range");
		case os_task_period_under_range:	return("period under range");
	}

	return "?";
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
					task_table[i].flag |= OS_PENDING_FLAG;
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

void os_init(void) {
	LOGI("init");

	for(uint8_t i=0; i<OS_TASK_MAXCOUNT; i++) {
		task_table[i].task_ptr	=	nullptr;
		task_table[i].counter	=	0;
		task_table[i].period	=	0;
		task_table[i].flag		=	false;
		task_table[i].name		=	nullptr;
	}

	#if OS_SHOW_SPLASH_SCREEN_AT_START
		os_splash_screen();
	#endif

	#if OS_SHOW_RESET_SOURCE_AT_START
		printf(TEXT_CYAN_BRIGHT "reset source: " TEXT_CYAN "%s" FORMAT_RESET "\n\n", os_print_reset_source());
	#endif

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
		add_repeating_timer_ms(-10, os_system_tick, nullptr, &timer);
	#endif
}

// This funtion must be executed directly from main()
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
		#endif
	}
}

// Dodawanie tasku do tablicy tasków
// - task_ptr  - wskaźnik do tasku
// - period_ms - czas z jaką częstotliwością task ma być wykonywany
os_t task_add_name(void (*task_ptr)(run_mode_t), uint16_t period_ms, const char * name) {
	os_t res = os_ok;

	if(period_ms < OS_TICK_PERIOD_MS) {
		res = os_task_period_under_range;
		goto end;
	}
	
	if(task_is_running(task_ptr)) {								// Szukanie czy task już istnieje

		res = os_task_already_created;
		goto end;
	}
	
	uint8_t slot_number;										// Szukanie pierwszego wolnego slotu
	if(task_find_free_slot(&slot_number) == os_no_free_slot) {
		res = os_no_free_slot;
		goto end;
	}
	
	task_table[slot_number].task_ptr	=	task_ptr;			// Wpisywanie nowego procesu
	task_table[slot_number].counter		=	(period_ms / OS_TICK_PERIOD_MS)-1;
	task_table[slot_number].period		=	period_ms / OS_TICK_PERIOD_MS;
	task_table[slot_number].name		=	name;
	
	task_ptr(os_constructor);									// Wywołanie inicjalizacyjne (konstruktor tasku)

	end:
	if(res == os_ok) 	LOGD("add(%s, %u)", name, period_ms);
	else				LOGE("add(%s, %u) -> %s", name, period_ms, debug_res(res));
	return res;
}

// Usuwanie tasku bez wywołania destruktora - uważać jeśli task wykorzystuje dynamiczną alokację pamięci
os_t task_clear(uint8_t slot_number) {
	os_t res = os_ok;
	
	if(slot_number >= OS_TASK_MAXCOUNT) {						// Kontrola poprawności danych
		res = os_slot_number_over_range;
		// goto end;
	}
	
	task_table[slot_number].task_ptr	=	nullptr;
	task_table[slot_number].counter		=	0;
	task_table[slot_number].period		=	0;
	task_table[slot_number].flag		=	false;
	task_table[slot_number].name		=	nullptr;

	// end:
	// if(res == os_ok) 	LOGD("add(%s, %u)", name, period_ms);
	// else				LOGE("add(%s, %u) -> %s", name, period_ms, debug_res(res));
	return res;
}

// Execute task destructor and then remove it from the array
os_t task_close(void (*task_ptr)(run_mode_t)) {
	#if OS_DEBUG_MESSAGES_SHOW
		printf(FORMAT_RESET "Cls(");
		
		#if OS_USE_TASK_IDENTIFY
			task_ptr(os_id);
		#endif
		
		printf(")  \t= ");
	#endif
	
	uint8_t slot_number;										// Szukanie tasku
	if(task_find(task_ptr, &slot_number)) {
			
		#if OS_DEBUG_MESSAGES_SHOW
			printf("NotFound\n");
		#endif
			
		return os_not_found;
	}
	
	void (*ptr)(run_mode_t) = task_table[slot_number].task_ptr;				// Backup pointer to the task
	task_clear(slot_number);												// clear task from the array
	ptr(os_destructor);														// Execute task destructor
	
	#if OS_DEBUG_MESSAGES_SHOW
		printf("OK\n");
	#endif
	
	return os_ok;
}

// Zmiana czasów
// - task_ptr  - wskaźnik do procesu, który ma być zmieniony
// - period_ms - nowy okres
os_t task_period_change(void (*task_ptr)(run_mode_t), uint16_t period_ms) {
	uint8_t slot_number;
	if(task_find(task_ptr, &slot_number) == os_ok) {

		if(period_ms < OS_TICK_PERIOD_MS) {
			#if OS_DEBUG_MESSAGES_SHOW
				printf("period under range\n");
			#endif
		
			return os_task_period_under_range;
		}
		
		// Zmiana timingu - wszystkie operacje przy wyłączonych przerwaniach
		task_table[slot_number].counter	=	(period_ms / OS_TICK_PERIOD_MS)-1;
		task_table[slot_number].period	=	period_ms / OS_TICK_PERIOD_MS;
		task_table[slot_number].flag	=	0;
	
		#if OS_DEBUG_MESSAGES_SHOW
			printf(FORMAT_RESET "PCh(");
		
			#if OS_USE_TASK_IDENTIFY
				task_table[slot_number].task_ptr(os_id);
			#endif
		
			printf(",%u)\n", period_ms * OS_TICK_PERIOD_MS);
		#endif
	
		return os_ok;
	}
	else {
		return os_not_found;
	}
}

// Funkcja znajduje pierwszy wolny slot i zwraca jego numer przez wskaźnik
// Jeśli brak wolnych slotów to zwraca os_no_free_slot
os_t task_find_free_slot(uint8_t * slot_number) {
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
// Funkcja zwraca jedną z trzech wartości:
// - os_ok - znaleziono jedno wystąpienia tasku
// - os_error - nie znaleziono
os_t task_find(void (*task_ptr)(run_mode_t), uint8_t * slot_number) {
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

// Sprawdzanie czy task jest dodany do tablicy tasków (nie czy akcualnie jest w trakcie wykonywania)
bool task_is_running(void (*task_ptr)(run_mode_t)) {
	return task_find(task_ptr) == os_ok;
}

#endif
