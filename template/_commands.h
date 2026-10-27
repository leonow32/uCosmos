#include "_config/_config.h"
#ifndef INTERPRETER_COMMANDS_H_
#define INTERPRETER_COMMANDS_H_

// ========================================
// Includes
// ========================================

#include "uCosmos/console.h"
#include "uCosmos/commands.h"

#if COMPONENT_CIPHER_AES128
	#include "cipher_aes128/aes128_demo.h"
#endif

#if COMPONENT_CIPHER_SBOX
	#include "cipher_sbox/sbox_demo.h"
#endif

#if COMPONENT_CRC
	#include "crc/crc_demo.h"
#endif

#if COMPONENT_TOOLS_RP
	#include "tools_rp/tools_rp.h"
#endif

// ========================================
// Command names and function pointers
// ========================================

const command_struct command_list[] = {

// ========================================
// System uCosmos
// ========================================

#if USE_CMD_TASK_MONITOR
	{"`",						task_monitor_cmd},
	{"~`~",						task_monitor_cmd},
#endif

#if USE_CMD_TIME
	{"time",					time_print_cmd},
#endif	

#if USE_CMD_TASK_COMMANDS
	{"add",						task_add_cmd},
	{"close",					task_close_cmd},
	{"per",						task_period_change_cmd},
	{"exe",						task_exe_cmd},
#endif

#if USE_CMD_TASK_DEMO
	{"demo1_add",				demo1_add_cmd},
	{"demo2_add",				demo2_add_cmd},
	{"demo1_cls",				demo1_cls_cmd},
	{"demo2_cls",				demo2_cls_cmd},
#endif

#if USE_CMD_ALL
	{"?",						all_commands_cmd},
#endif

#if USE_CMD_PARSE_DEMO
	{"args",					args_cmd},
	{"echo",					echo_cmd},

	{"hex8",					hex8_cmd},
	{"hex16",					hex16_cmd},
	{"hex32",					hex32_cmd},
	{"hex64",					hex64_cmd},

	{"uint8",					uint8_cmd},
	{"uint16",					uint16_cmd},
	{"uint32",					uint32_cmd},
	{"uint64",					uint64_cmd},
	{"anybase",					numeric_cmd},

	{"int8",					int8_cmd},
	{"int16",					int16_cmd},
	{"int32",					int32_cmd},
	{"int64",					int64_cmd},

	{"ascchr",					ascchr_cmd},
	{"ascstr",					ascstr_cmd},
	{"hexstr",					hexstr_cmd},
	{"ptime",					time_demo_cmd},
#endif

// ========================================
// Hardware related
// ========================================

#if COMPONENT_TOOLS_RP
	{"reset",					tools_rp::reset_cmd},
	{"mem",						tools_rp::mem_cmd},
	{"fcpu",					tools_rp::fcpu_cmd},
#endif

#if COMPONENT_TOOLS_ESP32
	{"reset",					tools_esp32::reset_cmd},
	{"mem",						tools_esp32::mem_cmd},
	{"fcpu",					tools_esp32::fcpu_cmd},
#endif

// ========================================
// Cipher AES-128
// ========================================

#if AES128_USE_DEMO_COMMANDS
	#if AES128_USE_ECB
		{"aes128_ecb_e",		aes128_demo::aes128_ecb_encrypt_cmd},
		{"aes128_ecb_d",		aes128_demo::aes128_ecb_decrypt_cmd},
	#endif

	#if AES128_USE_ECB_SOFT
		{"aes128_ecb_soft_e",	aes128_demo::aes128_ecb_soft_encrypt_cmd},
		{"aes128_ecb_soft_d",	aes128_demo::aes128_ecb_soft_decrypt_cmd},
	#endif

	#if AES128_USE_CBC
		{"aes128_cbc_e",		aes128_demo::aes128_cbc_encrypt_cmd},
		{"aes128_cbc_d",		aes128_demo::aes128_cbc_decrypt_cmd},
	#endif

	#if AES128_USE_CBC_SOFT
		{"aes128_cbc_soft_e",	aes128_demo::aes128_cbc_soft_encrypt_cmd},
		{"aes128_cbc_soft_d",	aes128_demo::aes128_cbc_soft_decrypt_cmd},
	#endif
#endif

// ========================================
// Cipher S-Box
// ========================================

#if SBOX_USE_DEMO_COMMANDS
	{"sbox_e",			sbox::encrypt_cmd},
	{"sbox_d",			sbox::decrypt_cmd},
#endif

// ========================================
// CRC
// ========================================

#if CRC_USE_DEMO_COMMANDS
	{"crc",				crc::crc16_cmd},
#endif

// ========================================
// Tools for Raspberry Pico
// ========================================

#if COMPONENT_TOOLS_RP
	{"test",			tools_rp::mem_cmd},
#endif

};

#endif
