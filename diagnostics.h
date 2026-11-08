#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#include <cstdio>

class ExecutionTimer {
	private:
		uint64_t time_begin;
		uint64_t time_end;

		static inline uint64_t get_time_us(void) {
			#if ESP_PLATFORM
				return uint64_t(esp_timer_get_time());
			#elif PICO_RP2040 || PICO_RP2350
				return time_us_64();
			#else
				#warning "Platform not supported"
				return 0;
			#endif
		}

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

		void print(const char * text = nullptr) {
			uint64_t time_elapsed = time_end ? (time_end - time_begin) : (get_time_us() - time_begin);
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
