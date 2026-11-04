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

	{"dec8",					dec8_cmd},
	{"dec16",					dec16_cmd},
	{"dec32",					dec32_cmd},
	{"dec64",					dec64_cmd},

	{"hex8",					hex8_cmd},
	{"hex16",					hex16_cmd},
	{"hex32",					hex32_cmd},
	{"hex64",					hex64_cmd},

	{"bin8",					bin8_cmd},
	{"bin16",					bin16_cmd},
	{"bin32",					bin32_cmd},
	{"bin64",					bin64_cmd},

	{"int8",					int8_cmd},
	{"int16",					int16_cmd},
	{"int32",					int32_cmd},
	{"int64",					int64_cmd},

	{"float",					float_cmd},
	{"double",					double_cmd},

	{"ascchr",					ascchr_cmd},
	{"ascstr",					ascstr_cmd},
	{"hexstr",					hexstr_cmd},
	{"timestr",					timestr_cmd},
#endif