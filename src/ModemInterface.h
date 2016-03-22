#pragma once

#include "Command.h"

#include <mutex>
#include <queue>

using namespace std;


class ModemInterface
{
private:
	mutex mu;
	queue <Command> cmd_unrun;
	queue <Command> cmd_run;
public:
	ModemInterface();
	virtual ~ModemInterface();

	void push_command(Command cmd);
	void push_command(wstring str);
	Command pop_command();
	size_t num_commands();

	void push_result(Command cmd);
	Command pop_result();
	size_t num_results();
};

