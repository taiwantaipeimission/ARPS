#pragma once
#include "Message.h"
#include "codes.h"
#include <string>
#include <vector>

using namespace std;

class SubCommand
{
public:
	SubCommand(wstring cmd_str)
		: cmd(cmd_str), result(L""), ran(false), success(false), timeout(false), timeout_ms(NO_RESPONSE_TIMEOUT_MS)
	{
		if (cmd.find(COMMAND_ESCAPE_CHAR) != wstring::npos)
			timeout_ms = MSG_NO_RESPONSE_TIMEOUT_MS;
	}
	SubCommand()
		: SubCommand(L"")
	{}
	~SubCommand()
	{}

	wstring cmd;
	wstring result;
	bool ran;
	bool success;
	bool timeout;
	int timeout_ms;
};

class Command
{
public:

	Command()
		: n_times_to_try(1), n_times_tried(0), is_check_msg_command(false), sub_cmds()
	{
	}

	~Command()
	{}

	int n_times_to_try;
	int n_times_tried;
	bool is_check_msg_command;
	vector<SubCommand> sub_cmds;
};