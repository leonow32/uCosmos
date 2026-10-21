#include "../config.h"
#include "uCosmos.h"

void uCosmos_init(void) {
	LOGI("init");
	task_init();

	#if CONSOLE_USE
		console_init();
	#endif
}

void uCosmos_scheduler(void) {
	task_scheduler();
}
