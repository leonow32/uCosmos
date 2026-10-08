#include "config.h"
#if COMPONENT_CONSOLE
#ifndef INTERPRETER_COMMANDS_H_
#define INTERPRETER_COMMANDS_H_

// ========================================
// Includes
// ========================================

#include "uCosmos/console.h"

#if COMPONENT_CIPHER_AES128
	#include "cipher_aes128/aes128_demo.h"
#endif

#if COMPONENT_CIPHER_S
	#include "cipher_s/s_demo.h"
#endif

#if COMPONENT_CIPHER_SBOX
	#include "cipher_sbox/sbox_demo.h"
#endif

#if COMPONENT_CRC
	#include "crc/crc_demo.h"
#endif

#if COMPONENT_DISPLAY_HAL
	#include "display_hal/display_hal_demo.h"
#endif

#if COMPONENT_DISPLAY_ILI9341
	#include "display_ili9341/display_ili9341_demo.h"
#endif

#if COMPONENT_DISPLAY_ST7796
	#include "display_st7796/display_st7796_demo.h"
#endif

#if COMPONENT_DISPLAY_ST7796_DIV2
	#include "display_st7796_div2/display_st7796_div2_demo.h"
#endif

#if COMPONENT_DISPLAY_RGB
	#include "display_rgb/display_rgb_demo.h"
#endif

#if COMPONENT_DISPLAY_BACKLIGHT
	#include "display_backlight/display_backlight_demo.h"
#endif

#if COMPONENT_DISPLAY_WS2812
	#include "display_ws2812/display_ws2812_demo.h"
#endif

#if COMPONENT_FILE_SYSTEM
	#include "file_system/file_system_demo.h"
#endif

#if COMPONENT_I2C_MASTER
	#include "i2c_master/i2c_master_demo.h"
#endif

#if COMPONENT_DISPLAY_LVGL8
	#include "display_lvgl8/display_lvgl_demo.h"
#endif

#if COMPONENT_DISPLAY_LVGL9
	#include "display_lvgl9/display_lvgl_demo.h"
#endif

#if COMPONENT_FILTER_IIR
	#include "filter_iir/iir_demo.h"
#endif

#if COMPONENT_MYM
	#include "mystify_your_mind/mym.h"
#endif

#if COMPONENT_NVS
	#include "nvs/nvs_demo.h"
#endif

#if COMPONENT_RTC_HAL
	#include "rtc_hal/rtc_hal_demo.h"
#endif

#if COMPONENT_RTC_DS1307
	#include "rtc_ds1307/rtc_ds1307_demo.h"
#endif

#if COMPONENT_RTC_DS3231
	#include "rtc_ds3231/rtc_ds3231_demo.h"
#endif

#if COMPONENT_RTC_FAKE
	#include "rtc_fake/rtc_fake_demo.h"
#endif

#if COMPONENT_RTC_MCP7940
	#include "rtc_mcp7940/rtc_mcp7940_demo.h"
#endif

#if COMPONENT_RTC_PCF8563
	#include "rtc_pcf8563/rtc_pcf8563_demo.h"
#endif

#if COMPONENT_SENSOR_TEMP_ESP32S3
	#include "sensor_temperature_esp32s3/sensor_temperature_esp32s3_demo.h"
#endif

#if COMPONENT_SPEAKER
	#include "speaker/speaker_demo.h"
#endif

#if COMPONENT_SPI_MASTER
	#include "spi_master/spi_master_demo.h"
#endif

#if COMPONENT_TITAN
	#include "titan/titan_commands.h"
#endif

#if COMPONENT_TEXT_TIMER
	#include "text_timer/text_timer_commands.h"
	#include "text_timer/sensor.h"
	#include "text_timer/titan_link/console.h"

	#if PRODUCT_TEXT_TIMER_V34
		#include "text_timer/greenpak.h"
	#endif
#endif

#if COMPONENT_TOUCH_HAL
	#include "touch_hal/touch_hal_demo.h"
#endif

#if COMPONENT_TOUCH_FT5206
	#include "touch_ft5206/touch_ft5206_demo.h"
#endif

#if COMPONENT_TOUCH_FT5446
	#include "touch_ft5446/touch_ft5446_demo.h"
#endif

#if COMPONENT_TOUCH_FT6336
	#include "touch_ft6336/touch_ft6336_demo.h"
#endif

#if COMPONENT_TOUCH_GT911
	#include "touch_gt911/touch_gt911_demo.h"
#endif

#if COMPONENT_TOUCH_ILI2130
	#include "touch_ili2130/touch_ili2130_demo.h"
#endif

#if COMPONENT_TOUCH_HY4613
	#include "touch_hy4613/touch_hy4613_demo.h"
#endif

#if COMPONENT_TOUCH_ST1XXX
	#include "touch_st1xxx/touch_st1xxx_demo.h"
#endif

#if COMPONENT_TOUCH_XPT2046
	#include "touch_xpt2046/touch_xpt2046_demo.h"
#endif

#if COMPONENT_UCOSMOS
	#include "uCosmos/commands.h"
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

	{"reset",					reset_cmd},
	{"mem",						memory_status_cmd},

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
	{"dec8",					dec8_cmd},
	{"dec16",					dec16_cmd},
	{"dec16s",					dec16s_cmd},
	{"dec32",					dec32_cmd},
	{"dec32s",					dec32s_cmd},
	{"hexstr",					hexstr_cmd},
	{"ascstr",					ascstr_cmd},
	{"ascchr",					ascchr_cmd},

	{"nhex8",					new_hex8_cmd},
	{"nhex16",					new_hex16_cmd},
	{"nhex32",					new_hex32_cmd},
	{"nhex64",					new_hex64_cmd},
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
// Cipher S
// ========================================

#if TL_USE_DEMO_COMMANDS
	{"s_e",				s::encrypt_cmd},
	{"s_d",				s::decrypt_cmd},
	{"s_test",			s::test_cmd},
	{"s_test2",			s::test2_cmd},
	{"s_test3",			s::test3_cmd},
	// {"s_test4",			s::test4_cmd},
#endif

// ========================================
// Cipher SBox
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
// Display LVGL
// ========================================

#if DISPLAY_LVGL_USE_DEMO_COMMANDS
	{"lv_sleep",		display_lvgl::sleep_cmd},
	{"lv_wake",			display_lvgl::wake_cmd},
	{"lv_screenshot",	display_lvgl::screenshot_cmd},
	{"lv_wid",			display_lvgl::widgets_demo_cmd},
	{"lv_ben",			display_lvgl::benchmark_demo_cmd},
	{"lv_str",			display_lvgl::stress_demo_cmd},
	{"lv_wid",			display_lvgl::widgets_demo_cmd},
	{"lv_ben",			display_lvgl::benchmark_demo_cmd},
	{"lv_str",			display_lvgl::stress_demo_cmd},
#endif

// ========================================
// Display HAL
// ========================================

#if DISPLAY_USE_DEMO_COMMANDS
	{"r",				DisplayHAL::CmdDisplayRefresh},
	{"cont",			DisplayHAL::CmdContrast},
	{"color",			DisplayHAL::CmdColor},
	{"cur",				DisplayHAL::CmdCursor},
 	{"clear",			DisplayHAL::CmdClear},
	
	{"chess",			DisplayHAL::CmdDrawChessboard},	
	{"pix",				DisplayHAL::CmdDrawPixel},
	{"clrpix",			DisplayHAL::CmdClearPixel},
	{"byte",			DisplayHAL::CmdDrawByte},
	{"line",			DisplayHAL::CmdDrawLine},
	{"rect",			DisplayHAL::CmdDrawRectangle},
	{"fill",			DisplayHAL::CmdDrawRectangleFill},
	{"cir",				DisplayHAL::CmdDrawCircle},
	
	{"bitmap",			DisplayHAL::CmdDrawBitmap},
	
	{"font",			DisplayHAL::CmdFontSet},
	{"txt",				DisplayHAL::CmdText},
	{"fonts",			DisplayHAL::CmdFontDemo},
	{"lorem",			DisplayHAL::CmdDemoLoremIpsum},

	#if DISPLAY_USE_UNICODE
		{"uni",			DisplayHAL::CmdDemoUnicode},
	#endif

	// Font demos
	#if DISPLAY_FONT_DOS8x8
		{"dos8",		DisplayHAL::CmdDemoFontDos8},
	#endif

	#if DISPLAY_FONT_DOS16x8
		{"dos16",		DisplayHAL::CmdDemoFontDos16},
	#endif
		
 	{"pixels",			DisplayHAL::CmdPixels},
 	{"snake",			DisplayHAL::CmdSnake},
 	{"movcir",			DisplayHAL::CmdMovingCircle},
#endif

// ========================================
// Display Backlight
// ========================================

#if DISPLAY_BACKLIGHT_USE_DEMO_COMMANDS
	{"b",				backlight::backlight_cmd},
#endif

// ========================================
// Display ILI9341
// ========================================

#if ILI9341_USE_DEMO_COMMANDS
	{"init",					ILI9341::CmdInit},
	{"c",						ILI9341::CmdCommand},
	{"d",						ILI9341::CmdData},
	{"cd",						ILI9341::CmdCommandData},
	{"dc",						ILI9341::CmdDC},
	{"r",						ILI9341::CmdDisplayRefresh},
	{"area",					ILI9341::CmdActiveAreaSet},
	{"clr",						ILI9341::CmdClear},
	{"pix",						ILI9341::pixel_cmd},
	{"byte",					ILI9341::byte_cmd},
	{"chess",					ILI9341::CmdChess},
#endif

// ========================================
// Display ST7796S
// ========================================

#if ST7796_USE_DEMO_COMMANDS
	{"init",					ST7796::CmdInit},
	{"cmd",						ST7796::CmdCommand},
	{"dat",						ST7796::CmdData},
	{"cd",						ST7796::CmdCommandData},
	{"dc",						ST7796::CmdDC},
	{"r",						ST7796::CmdDisplayRefresh},
	{"area",					ST7796::CmdActiveAreaSet},
	{"clr",						ST7796::CmdClear},
	{"pix",						ST7796::pixel_cmd},
	{"byte",					ST7796::byte_cmd},
	{"chess",					ST7796::CmdChess},
#endif

// ========================================
// Display RGB
// ========================================

#if RGB_USE_DEMO_COMMANDS
	{"rgb_enable",				display_rgb::enable_cmd},
	{"rgb_sleep",				display_rgb::sleep_cmd},
	{"rgb_clr",					display_rgb::clear_cmd},
	{"rgb_pix",					display_rgb::pixel_cmd},
	{"rgb_byte",				display_rgb::byte_cmd},
	{"rgb_fill",				display_rgb::fill_cmd},
	{"rgb_selbuf",				display_rgb::select_buffer_cmd},
	{"rgb_mirror",				display_rgb::mirror_cmd},
	{"rgb_freq",				display_rgb::freq_set_cmd},
	{"rgb_r",					display_rgb::refresh_cmd},
	{"rgb_buffers",				display_rgb::buffers_cmd},
#endif

// ========================================
// Display WS2812 - display_rgb LEDs
// ========================================

#if WS2812_USE_DEMO_COMMANDS
	{"wr",						ws2812::refresh_cmd},
	{"ws",						ws2812::color_set_cmd},
	{"wd",						ws2812::demo_cmd},
#endif

// ========================================
// File System
// ========================================

#if FILE_SYSTEM_USE_DEMO_COMMANDS
	{"f_info",					FileSystem::CmdInfo},
	{"f_read",					FileSystem::CmdRead},
	{"f_write",					FileSystem::CmdWrite},
	{"f_list",					FileSystem::CmdList},
#endif

// ========================================
// Filter IIR
// ========================================

#if IIR_USE_DEMO_COMMANDS
	{"iir",						iir::test},
#endif

// ========================================
// I2C Master
// ========================================

#if I2C_MASTER_USE_DEMO_COMMANDS
	{"i2c_a",					I2C::CmdAdd},
	{"i2c_w",					I2C::CmdWrite},
	{"i2c_r",					I2C::CmdRead},
	{"i2c_t",					I2C::CmdWriteRead},
	{"i2c_scan",				I2C::CmdScan},
#endif

// ========================================
// nvs
// ========================================

#if NVS_USE_DEMO_COMMANDS
	{"nvs_u8",					nvs::read_write_u8_cmd},
	{"nvs_u16",					nvs::read_write_u16_cmd},
	{"nvs_u32",					nvs::read_write_u32_cmd},
	{"nvs_e",					nvs::erase_var_cmd},
	{"nvs_eall",				nvs::erase_all_cmd},
	{"nvs_stat",				nvs::stat_cmd},
	{"nvs",						nvs::list_cmd},
#endif

// ========================================
// Mystify Your Mind
// ========================================

#if MYM_USE_DEMO_COMMANDS
	{"mym",						MYM::CmdMystifyYourMind},
	{"mym_debug",				MYM::CmdDebug},
	{"mym_close",				MYM::CmdClose},
#endif

// ========================================
// Speaker
// ========================================

#if SPEAKER_USE_DEMO_COMMANDS
	{"n",						speaker::note_play_cmd},
	{"m",						speaker::melody_play_cmd},
	{"vol",						speaker::volume_set_cmd},
#endif

// ========================================
// SPI Master
// ========================================

#if SPI_MASTER_USE_DEMO_COMMANDS
	{"spi_t",					SPI::CmdTransmit},
	{"spi_r",					SPI::CmdRead},
	{"spi_w",					SPI::CmdWrite},
#endif

// ========================================
// RTC HAL
// ========================================

#if RTC_HAL_USE_DEMO_COMMANDS
	{"rtc_r",					RTC_HAL::read_cmd},
	{"rtc_w",					RTC_HAL::write_cmd},
	{"rtc_e",					RTC_HAL::erase_cmd},
	{"rtc_set",					RTC_HAL::is_set_cmd},
	{"rtc_con",					RTC_HAL::is_connected_cmd},
#endif

// ========================================
// RTC DS1307
// ========================================

#if DS1307_USE_DEMO_COMMANDS
	{"ds1307_r",				DS1307::read_cmd},
	{"ds1307_w",				DS1307::write_cmd},
	{"ds1307_e",				DS1307::erase_cmd},
	{"ds1307_set",				DS1307::is_set_cmd},
	{"ds1307_con",				DS1307::is_connected_cmd},
#endif

// ========================================
// RTC DS3231
// ========================================

#if DS3231_USE_DEMO_COMMANDS
	{"ds3231_r",				DS3231::CmdRead},
	{"ds3231_w",				DS3231::CmdWrite},
#endif

// ========================================
// RTC FAKE
// ========================================

#if RTC_FAKE_USE_DEMO_COMMANDS
	{"rtc_r",					RTC_FAKE::CmdRead},
	{"rtc_w",					RTC_FAKE::CmdWrite},
#endif

// ========================================
// RTC MCP7940
// ========================================

#if MCP7940_USE_DEMO_COMMANDS
	{"mcp7940_r",				MCP7940::read_cmd},
	{"mcp7940_w",				MCP7940::write_cmd},
	{"mcp7940_e",				MCP7940::erase_cmd},
	{"mcp7940_set",				MCP7940::is_set_cmd},
	{"mcp7940_con",				MCP7940::is_connected_cmd},
#endif

// ========================================
// RTC PCF8563
// ========================================

#if PCF8563_USE_DEMO_COMMANDS
	{"pcf8563_r",				PCF8563::CmdRead},
	{"pcf8563_w",				PCF8563::CmdWrite},
#endif

// ========================================
// Temperature Sensor in ESP32-S3
// ========================================

#if TEMPSENSOR_USE_DEMO_COMMANDS
	{"temp",					sensor_temp::read_cmd},
#endif
		
// ========================================
// Text Timer User Interface
// ========================================

#if COMPONENT_TEXT_TIMER
	#if PRODUCT_TEXT_TIMER_V31
		{"tc",					text_timer::debug_data_clear_cmd},
		{"tp",					text_timer::debug_data_print_cmd},
	#endif

	#if PRODUCT_TEXT_TIMER_V34
		{"gp_en",				text_timer::gp_en_cmd},
		{"gp_val",				text_timer::gp_val_cmd},
	#endif

	// Ogólne
	{"ver",						text_timer::ver_cmd},
	{"state",					text_timer::state_cmd},
	{"pause",					text_timer::pause_cmd},
	{"run",						text_timer::run_cmd},
	{"off",						text_timer::off_cmd},
	{"on",						text_timer::on_cmd},
	{"relays",					text_timer::relays_cmd},
	{"hm",						text_timer::hm_cmd},
	{"frame",					text_timer::frame_cmd},

	{"t1",						text_timer::popup_test1_cmd},
	{"t2",						text_timer::popup_test2_cmd},
	{"t3",						text_timer::popup_test3_cmd},
	{"rtl",						text_timer::rtl_cmd},
	{"lang",					text_timer::lang_cmd},

	{"pop_ok",					text_timer::popup2_ok_cmd},
	{"pop_ok_del",				text_timer::popup2_ok_del_cmd},
	{"pop_info",				text_timer::popup2_info_cmd},
	{"pop_yes_no",				text_timer::popup2_yes_no_cmd},
	{"pop_yes_no_state",		text_timer::popup2_yes_no_state_cmd},
	{"pop_reset",				text_timer::popup2_reset_cmd},
	{"pop_timer",				text_timer::popup2_timer_cmd},
	
	{"test1",					text_timer::test1},
	{"prog1",					text_timer::program1_cmd},
	{"prog2",					text_timer::program2_cmd},
	{"prog3",					text_timer::program3_cmd},
	{"prog_ptr",				text_timer::prog_rom_ptr_find_cmd},

	{"clean_state",				text_timer::clean_state_cmd},

	{"cnt",						text_timer::debug_print_counters_cmd},
	{"cntclr",					text_timer::debug_clear_counters_cmd},

	{"tp",						text_timer::debug_data_print_cmd},
	{"tc",						text_timer::debug_data_clear_cmd},

	{"sr",						text_timer::sensor_read_cmd},
	{"sp",						text_timer::sensor_print_cmd},
	{"max_temp",				text_timer::sensor_max_temp_cmd},
	{"max_temp_reset",			text_timer::sensor_max_temp_reset_cmd},
	{"ss_dc",					text_timer::sensor_door_close_set_cmd},
	{"ss_dl",					text_timer::sensor_door_lock_set_cmd},
	{"ss_ho",					text_timer::sensor_heat_on_set_cmd},
	{"ss_at",					text_timer::sensor_at_set_cmd},
	{"ss_pt",					text_timer::sensor_pt_set_cmd},
	{"ss_pt2",					text_timer::sensor_pt2_set_cmd},
	{"ss_rhm",					text_timer::sensor_rhm_set_cmd},
	{"ss_pwr_temp",				text_timer::sensor_pwr_temp_set_cmd},

	{"layers",					text_timer::layers_cmd},
	{"layer_name",				text_timer::layer_name_cmd},
	{"layers_del",				text_timer::layers_delete_all_cmd},
	{"tasks_del",				text_timer::task_close_all_but_required_cmd},

	{"cook_time",				text_timer::cook_time_cmd},
	{"wait_time",				text_timer::wait_time_cmd},
	{"work_time",				text_timer::work_time_cmd},
	{"clean_time",				text_timer::clean_time_cmd},
	{"clean_timer",				text_timer::clean_timer_cmd},
	{"timer_time",				text_timer::timer_time_cmd},
	{"sab_time",				text_timer::sabbath_time_cmd},

	{"crash",					text_timer::crash_cmd},
	{"desync",					text_timer::desync},

	{"ai",						text_timer::ai_cmd},

	{"fav_dump",				text_timer::fav_dump_cmd},
	{"fav_check",				text_timer::fav_check_cmd},
	{"fav_add",					text_timer::fav_add_cmd},
	{"fav_del",					text_timer::fav_del_cmd},
	{"fav_del_all",				text_timer::fav_del_all_cmd},
	{"fav_swap",				text_timer::fav_swap_cmd},
	{"fav_find",				text_timer::fav_find_cmd},
	{"fav_count",				text_timer::fav_count_cmd},

	{"prog_dump",				text_timer::prog_dump_cmd},
	{"prog_print",				text_timer::prog_print_cmd},

	{"editor_dump",				text_timer::editor_dump_cmd},
	{"editor_check",			text_timer::editor_prog_check_cmd},
	{"editor_count",			text_timer::editor_prog_count_cmd},
	{"editor_del",				text_timer::editor_prog_del_cmd},
	{"editor_find",				text_timer::editor_prog_find_cmd},
	{"editor_swap",				text_timer::editor_prog_swap_cmd},
	{"editor_get",				text_timer::editor_prog_name_get_cmd},
	{"editor_set",				text_timer::editor_prog_name_set_cmd},

	{"editor_step_check",		text_timer::editor_step_check_cmd},
	{"editor_step_count",		text_timer::editor_step_count_cmd},
	{"editor_step_del",			text_timer::editor_step_del_cmd},
	{"editor_step_get",			text_timer::editor_step_data_get_cmd},
	{"editor_step_set",			text_timer::editor_step_data_set_cmd},

	{"keyboard",				text_timer::keyboard_cmd},

	// TitanLink
	{"ping",					text_timer::ping_cmd},
	{"get_system_info",			text_timer::get_system_info_cmd},
	{"set_module_cfg",			text_timer::set_module_cfg_cmd},
	{"get_module_cfg",			text_timer::get_module_cfg_cmd},
	{"get_status",				text_timer::get_status_cmd},
	{"set_relays",				text_timer::set_relays_cmd},
	{"set_cook_param",			text_timer::set_cooking_params_cmd},
	{"set_light",				text_timer::set_light_cmd},
	{"set_door_lock",			text_timer::set_door_lock_cmd},
	{"set_tune", 				text_timer::set_tune_cmd},
	{"get_tune", 				text_timer::get_tune_cmd},

	{"ado_get_ver",				text_timer::ado_get_ver_cmd},
	{"ado_set_mode",			text_timer::ado_set_mode_cmd},
	{"ado_set_color",			text_timer::ado_set_color_cmd},
	{"ado_set_color_adv",		text_timer::ado_set_color_advanced_cmd},
	{"ado_open",				text_timer::ado_open_door_cmd},


	{"unknown_command", 		text_timer::unknown_command_cmd},
	{"token",					text_timer::set_token_cmd},
#endif

// ========================================
// Touch HAL
// ========================================

#if TOUCH_HAL_USE_DEMO_COMMANDS && COMPONENT_DISPLAY_HAL
	{"tr",						TouchHAL::CmdRead},
	{"td",						TouchHAL::CmdTaskDemo},
	{"tc",						TouchHAL::CmdClear},
#endif

// ========================================
// Touch FT5206
// ========================================

#if FT5206_USE_DEMO_COMMANDS
	{"ft5206_r",				FT5206::CmdRead},
	
	#if COMPONENT_DISPLAY_HAL
		{"ft5206_d",			FT5206::CmdTaskDemo},
	#endif
#endif

// ========================================
// Touch FT5446
// ========================================

#if FT5446_USE_DEMO_COMMANDS && COMPONENT_DISPLAY_HAL
	{"ft5446_r",				FT5446::CmdRead},	
	{"ft5446_d",				FT5446::CmdTaskDemo},
	{"ft5446_c",				FT5446::CmdClear},
#endif

// ========================================
// Touch FT6336
// ========================================

#if FT6336_USE_DEMO_COMMANDS
	{"ft6336_r",				FT6336::CmdRead},
	
	#if COMPONENT_DISPLAY_HAL
		{"ft6336_d",			FT6336::CmdTaskDemo},
	#endif
#endif

// ========================================
// Touch GT911 (touch)
// ========================================

#if GT911_USE_DEMO_COMMANDS
	{"gt_rr",					GT911::CmdReadRegister},
	{"gt_ra",					GT911::CmdReadArray},
	{"gt_c",					GT911::CmdClear},
	{"gt_r",					GT911::CmdRead},
	
	{"bdump",					GT911::CmdBufferDump},
	{"bset",					GT911::CmdBufferSet},
	{"bread",					GT911::CmdBufferRead},
	{"bWRITE",					GT911::CmdBufferWrite},
	{"bhop",					GT911::CmdBufferHopping},

	{"gt_dump",					GT911::CmdConfigDump},
	{"gt_whole",				GT911::CmdWholeMemoryDump},
	
	{"gt_cprint",				GT911::CmdConfigPrint},
	{"gt_ana",					GT911::CmdConfigAnalyze},
	{"gt_list",					GT911::CmdConfigList},
	{"gt_demo",					GT911::CmdTaskDemo},
#endif

// ========================================
// Touch HY4613
// ========================================

#if HY4613_USE_DEMO_COMMANDS
	{"hy4613_r",				HY4613::CmdRead},
	{"hy4613_d",				HY4613::CmdTaskDemo},
#endif

// ========================================
// Touch ILI2130
// ========================================

#if ILI2130_USE_DEMO_COMMANDS
	{"ili_r",					ILI2130::read_cmd},
	{"ili_rr",					ILI2130::read_request_cmd},
	{"ili_d",					ILI2130::demo_cmd},
	{"ili_reset",				ILI2130::reset_cmd},
	{"ili_sleep",				ILI2130::sleep_cmd},
	{"ili_wake",				ILI2130::wake_cmd},
	{"ili_panel_info",			ILI2130::get_panel_info_cmd},
#endif

// ========================================
// Touch ST1XXX
// ========================================

#if ST1XXX_USE_DEMO_COMMANDS
	{"st_r",					ST1XXX::CmdRead},
	{"st_d",					ST1XXX::CmdTaskDemo},
#endif

// ========================================
// Touch XPT2046
// ========================================

#if XPT2046_USE_DEMO_COMMANDS
	{"xpt_raw",					XPT2046::CmdReadRaw},
	{"xpt_real",				XPT2046::CmdReadReal},
	
	#if COMPONENT_DISPLAY_HAL
		{"xpt_task",			XPT2046::CmdTaskDemo},
	#endif
#endif

};

#endif /* INTERPRETER_COMMANDS_H_ */
#endif
