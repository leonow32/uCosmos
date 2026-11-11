#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#include <cstdio>
#include "task.h"

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
