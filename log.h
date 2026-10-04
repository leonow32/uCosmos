
#include "ascii.h"

#if ESP_PLATFORM
	#include "esp_log.h"
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
    #define LOGE(text, ...) printf(TEXT_RED    "E (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGW(text, ...) printf(TEXT_YELLOW "W (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGD(text, ...) printf(TEXT_GREEN  "D (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGI(text, ...) printf(TEXT_WHITE  "I (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
#endif

