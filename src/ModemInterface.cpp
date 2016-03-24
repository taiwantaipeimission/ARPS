#include "ModemInterface.h"



ModemInterface::ModemInterface()
{
}


ModemInterface::~ModemInterface()
{
}

void ModemInterface::push_command(Command cmd)
{
	std::lock_guard<std::mutex> guard(mu);
	cmd_unrun.push(cmd);
}

void ModemInterface::push_command(wstring str)
{
	std::lock_guard<std::mutex> guard(mu);
	SubCommand sub_cmd(str);
	Command cmd;
	cmd.sub_cmds.push_back(sub_cmd);
	cmd_unrun.push(cmd);
}

Command ModemInterface::pop_command()
{
	std::lock_guard<std::mutex> guard(mu);
	Command ret_val;
	if (cmd_unrun.size() > 0)
	{
		ret_val = cmd_unrun.front();
		cmd_unrun.pop();
	}
	return ret_val;
}

size_t ModemInterface::num_commands()
{
	std::lock_guard<std::mutex> guard(mu);
	return cmd_unrun.size();
}

void ModemInterface::push_result(Command res)
{
	std::lock_guard<std::mutex> guard(mu);
	cmd_run.push(res);
}

Command ModemInterface::pop_result()
{
	std::lock_guard<std::mutex> guard(mu);
	Command ret_val;
	if (cmd_run.size() > 0)
	{
		ret_val = cmd_run.front();
		cmd_run.pop();
	}
	return ret_val;
}

size_t ModemInterface::num_results()
{
	std::lock_guard<std::mutex> guard(mu);
	return cmd_run.size();
}