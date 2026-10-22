#include "../_config/_config.h"
#include "uCosmos.h"

void uCosmos_init(void) {
	LOGI("init");

	#if OS_SHOW_SPLASH_SCREEN_AT_START
		printf(TEXT_YELLOW);
		printf("              __________  _____ __  _______  _____ \n");
		printf("       __  __/ ____/ __ \\/ ___//  |/  / __ \\/ ___/ \n");
		printf("      / / / / /   / / / /\\__ \\/ /|_/ / / / /\\__ \\  \n");
		printf("     / /_/ / /___/ /_/ /___/ / /  / / /_/ /___/ /  \n");
		printf("    / ____/\\____/\\____//____/_/  /_/\\____//____/   \n");
		printf(TEXT_CYAN "----" TEXT_YELLOW "\\/" TEXT_CYAN);
		for(uint8_t i=0; i<48; i++) printf("-");
		printf("\n" TEXT_CYAN_BRIGHT "System version: " TEXT_CYAN "%u.%u.%u" FORMAT_RESET "\n\n", UCOSMOS_VERSION_MAJOR, UCOSMOS_VERSION_MINOR, UCOSMOS_VERSION_PATCH);
	#endif
	
	task_init();

	#if CONSOLE_USE
		console_init();
	#endif
}

void uCosmos_scheduler(void) {
	task_scheduler();
}
