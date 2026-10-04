#ifndef ASCII_H_
#define ASCII_H_

// ASCII codes
#define NUL					uint8_t(0)
#define SOH					uint8_t(1)	// Up arrow
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
#define FORMAT_RESET		"\033[0m"
#define FORMAT_BOLD   		"\033[1m"
#define FORMAT_FAINT   		"\033[2m"
#define FORMAT_ITALIC   	"\033[3m"
#define FORMAT_UNDERLINE   	"\033[4m"
#define FORMAT_BLINK_SLOW   "\033[5m"
#define FORMAT_BLINK_FAST   "\033[6m"
#define TEXT_BLACK			"\033[30m"
#define TEXT_RED			"\033[31m"
#define TEXT_GREEN			"\033[32m"
#define TEXT_YELLOW			"\033[33m"
#define TEXT_BLUE			"\033[34m"
#define TEXT_MAGENTA		"\033[35m"
#define TEXT_CYAN			"\033[36m"
#define TEXT_WHITE			"\033[37m"
#define TEXT_BLACK_BRIGHT	"\033[90m"
#define TEXT_RED_BRIGHT		"\033[91m"
#define TEXT_GREEN_BRIGHT	"\033[92m"
#define TEXT_YELLOW_BRIGHT	"\033[93m"
#define TEXT_BLUE_BRIGHT	"\033[94m"
#define TEXT_MAGENTA_BRIGHT	"\033[95m"
#define TEXT_CYAN_BRIGHT	"\033[96m"
#define TEXT_WHITE_BRIGHT	"\033[97m"

#define BACK_BLACK			"\033[40m"
#define BACK_RED			"\033[41m"
#define BACK_GREEN			"\033[42m"
#define BACK_YELLOW			"\033[43m"
#define BACK_BLUE			"\033[44m"
#define BACK_MAGENTA		"\033[45m"
#define BACK_CYAN			"\033[46m"
#define BACK_WHITE			"\033[47m"
#define BACK_BLACK_BRIGHT	"\033[100m"
#define BACK_RED_BRIGHT		"\033[101m"
#define BACK_GREEN_BRIGHT	"\033[102m"
#define BACK_YELLOW_BRIGHT	"\033[103m"
#define BACK_BLUE_BRIGHT	"\033[104m"
#define BACK_MAGENTA_BRIGHT	"\033[105m"
#define BACK_CYAN_BRIGHT	"\033[106m"
#define BACK_WHITE_BRIGHT	"\033[107m"

#endif /* ASCII_H_ */