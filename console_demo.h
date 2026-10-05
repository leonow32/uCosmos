#ifndef CONSOLE_DEMO_H_
#define CONSOLE_DEMO_H_

// ========================================
// Console Commands
// ========================================

#if CONSOLE_USE_DEMO_COMMANDS
namespace console {
	void args_cmd(int argc, char * argv[]);
	void echo_cmd(int argc, char * argv[]);
	void hex8_cmd(int argc, char * argv[]);
	void hex16_cmd(int argc, char * argv[]);
	void hex32_cmd(int argc, char * argv[]);
	void dec8_cmd(int argc, char * argv[]);
	void dec16_cmd(int argc, char * argv[]);
	void dec16s_cmd(int argc, char * argv[]);
	void dec32_cmd(int argc, char * argv[]);
	void dec32s_cmd(int argc, char * argv[]);
	void hexstr_cmd(int argc, char * argv[]);
	void ascstr_cmd(int argc, char * argv[]);
	void ascchr_cmd(int argc, char * argv[]);
}
#endif

#endif /* CONSOLE_DEMO_H_ */
