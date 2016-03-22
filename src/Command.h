#pragma once
#include "Message.h"
#include <string>
#include <vector>

using namespace std;

class SubCommand
{
public:
	SubCommand(wstring cmd_str)
		: cmd(cmd_str), result(L""), ran(false), success(false), timeout(false)
	{}
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
};

class Command
{
public:

	Command()
		: n_times_to_try(1), n_times_tried(0), sub_cmds()
	{
	}

	~Command()
	{}

	int n_times_to_try;
	int n_times_tried;
	vector<SubCommand> sub_cmds;
};