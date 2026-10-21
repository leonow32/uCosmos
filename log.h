
#if ESP_PLATFORM
	#include "esp_clk_tree.h"
	#include "esp_log.h"
	#include "esp_timer.h"
#elif PICO_RP2040 || PICO_RP2350
	#include <stdio.h>	
	#include "pico/stdlib.h"
#endif

// ASCII codes
#define NUL					uint8_t(0)
#define SOH					uint8_t(1)
#define STX					uint8_t(2)
#define ETX					uint8_t(3)
#define EOT					uint8_t(4)
#define ENQ					uint8_t(5)
#define ACK					uint8_t(6)
#define BEL					uint8_t(7)
#define BS					uint8_t(8)
#define HT					uint8_t(9)
#define LF					uint8_t(10)
#define VT					uint8_t(11)
#define FF					uint8_t(12)
#define CR					uint8_t(13)
#define SO					uint8_t(14)
#define SI					uint8_t(15)
#define DLE					uint8_t(16)
#define DC1					uint8_t(17)
#define DC2					uint8_t(18)
#define DC3					uint8_t(19)
#define DC4					uint8_t(20)
#define NAK					uint8_t(21)
#define SYN					uint8_t(22)
#define ETB					uint8_t(23)
#define CAN					uint8_t(24)
#define EM					uint8_t(25)
#define SUB					uint8_t(26)
#define ESC					uint8_t(27)
#define FS					uint8_t(28)
#define GS					uint8_t(29)
#define RS					uint8_t(30)
#define US					uint8_t(31)
#define DEL					uint8_t(127)

// Keycodes of some keys used by a terminal
#define LINEFEED			uint8_t(10)
#define ENTER				uint8_t(13)
#define ESCAPE				uint8_t(27)
#define TAB					uint8_t(9)
#define BACKSPACE1			uint8_t(8)
#define BACKSPACE2			uint8_t(127)
#define CTRL_Z				uint8_t(26)

// ANSI color codes
#define FORMAT_RESET		"\x1B[0m"
#define FORMAT_BOLD   		"\x1B[1m"
#define FORMAT_FAINT   		"\x1B[2m"
#define FORMAT_ITALIC   	"\x1B[3m"
#define FORMAT_UNDERLINE   	"\x1B[4m"
#define FORMAT_BLINK_SLOW   "\x1B[5m"
#define FORMAT_BLINK_FAST   "\x1B[6m"
#define TEXT_BLACK			"\x1B[30m"
#define TEXT_RED			"\x1B[31m"
#define TEXT_GREEN			"\x1B[32m"
#define TEXT_YELLOW			"\x1B[33m"
#define TEXT_BLUE			"\x1B[34m"
#define TEXT_MAGENTA		"\x1B[35m"
#define TEXT_CYAN			"\x1B[36m"
#define TEXT_WHITE			"\x1B[37m"
#define TEXT_BLACK_BRIGHT	"\x1B[90m"
#define TEXT_RED_BRIGHT		"\x1B[91m"
#define TEXT_GREEN_BRIGHT	"\x1B[92m"
#define TEXT_YELLOW_BRIGHT	"\x1B[93m"
#define TEXT_BLUE_BRIGHT	"\x1B[94m"
#define TEXT_MAGENTA_BRIGHT	"\x1B[95m"
#define TEXT_CYAN_BRIGHT	"\x1B[96m"
#define TEXT_WHITE_BRIGHT	"\x1B[97m"

#define BACK_BLACK			"\x1B[40m"
#define BACK_RED			"\x1B[41m"
#define BACK_GREEN			"\x1B[42m"
#define BACK_YELLOW			"\x1B[43m"
#define BACK_BLUE			"\x1B[44m"
#define BACK_MAGENTA		"\x1B[45m"
#define BACK_CYAN			"\x1B[46m"
#define BACK_WHITE			"\x1B[47m"
#define BACK_BLACK_BRIGHT	"\x1B[100m"
#define BACK_RED_BRIGHT		"\x1B[101m"
#define BACK_GREEN_BRIGHT	"\x1B[102m"
#define BACK_YELLOW_BRIGHT	"\x1B[103m"
#define BACK_BLUE_BRIGHT	"\x1B[104m"
#define BACK_MAGENTA_BRIGHT	"\x1B[105m"
#define BACK_CYAN_BRIGHT	"\x1B[106m"
#define BACK_WHITE_BRIGHT	"\x1B[107m"

// Logs
#if ESP_PLATFORM
	#include "esp_log.h"
	#define LOGE(text, ...) ESP_LOGE(__FILE_NAME__, text, ##__VA_ARGS__)
	#define LOGW(text, ...) ESP_LOGW(__FILE_NAME__, text, ##__VA_ARGS__)
	#define LOGD(text, ...) ESP_LOGD(__FILE_NAME__, text, ##__VA_ARGS__)
	#define LOGI(text, ...) ESP_LOGI(__FILE_NAME__, text, ##__VA_ARGS__)
#elif PICO_RP2040 || PICO_RP2350
	#include "pico/stdlib.h"
    #define LOGE(text, ...) printf(TEXT_RED    "E (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGW(text, ...) printf(TEXT_YELLOW "W (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGD(text, ...) printf(TEXT_GREEN  "D (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
    #define LOGI(text, ...) printf(TEXT_WHITE  "I (%lld) %s: " text FORMAT_RESET "\n", time_us_64() / 1000, __FILE_NAME__, ##__VA_ARGS__)
#endif

