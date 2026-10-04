#include "../config.h"
#if COMPONENT_CONSOLE
#include "console_demo.h"

#if CONSOLE_USE_DEMO_COMMANDS

namespace console {

// Print all provided arguments
void args_cmd(int argc, char * argv[]) {
	printf("argc = %u\n", argc);
	
	for(uint8_t i=0; i<argc; i++) {
		printf("%u:\t%08lX\t%s\n", i, (uint32_t)argv[i], (const char *)argv[i]);
	}
}

// Print first argument
void echo_cmd(int argc, char * argv[]) {
	if(argc != 1) {
		printf("%s", argv[1]);
	}
	printf("\n");
}

void hex8_cmd(int argc, char * argv[]) {
	uint8_t value;
	if(parse_hex8(argv[1], &value)) return;
	printf("%u\n", value);
}

void hex16_cmd(int argc, char * argv[]) {
	uint16_t value;
	if(parse_hex16(argv[1], &value)) return;
	printf("%u\n", value);
}

void hex32_cmd(int argc, char * argv[]) {
	uint32_t value;
	if(parse_hex32(argv[1], &value)) return;
	printf("%lu\n", value);
}

void dec8_cmd(int argc, char * argv[]) {
	uint8_t value = 0;
	if(parse_dec8(argv[1], &value, 100)) return;
	printf("%u\n", value);
}

void dec16_cmd(int argc, char * argv[]) {
	uint16_t value = 0;
	if(parse_dec16(argv[1], &value, 10000)) return;
	printf("%u\n", value);
}

void dec16s_cmd(int argc, char * argv[]) {
	int16_t value = 0;
	if(parse_dec16s(argv[1], &value)) return;
	printf("%d\n", value);
}

void dec32_cmd(int argc, char * argv[]) {
	uint32_t value = 0;
	if(parse_dec32(argv[1], &value, 1000000)) return;
	printf("%lu\n", value);
}

void dec32s_cmd(int argc, char * argv[]) {
	int32_t value = 0;
	if(parse_dec32s(argv[1], &value)) return;
	printf("%ld\n", value);
}

void hexstr_cmd(int argc, char * argv[]) {
	uint8_t buffer[64];
	uint8_t length;
	if(parse_hex_string(argv[1], buffer, &length)) return;
	printf("length: %u\n", length);
	printf("ASC: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%c", buffer[i]);
	}
	printf("\nHEX: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

void ascstr_cmd(int argc, char * argv[]) {
	uint8_t buffer[16];
	uint8_t length;
	if(parse_ascii_string(argv[1], buffer, &length, sizeof(buffer), 3)) return;
	printf("length: %u\n", length);
	printf("ASC: %s\n", buffer);
	printf("HEX: ");
	for(uint8_t i=0; i<length; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

void ascchr_cmd(int argc, char * argv[]) {
	uint8_t chr;
	if(parse_ascii_char(argv[1], &chr)) return;
	printf("%c\n", chr);
}

}

#endif
#endif
