#include "ModemInterface.h"



ModemInterface::ModemInterface()
{
}


ModemInterface::~ModemInterface()
{
}

void ModemInterface::push_command(wstring cmd)
{
	std::lock_guard<std::mutex> guard(mu);
	commands.push(cmd);
}

wstring ModemInterface::pop_command()
{
	std::lock_guard<std::mutex> guard(mu);
	wstring ret_val(L"");
	if (commands.size() > 0)
	{
		ret_val = commands.front();
		commands.pop();
	}
	return ret_val;
}

size_t ModemInterface::num_commands()
{
	std::lock_guard<std::mutex> guard(mu);
	return commands.size();
}

void ModemInterface::push_result(wstring cmd, wstring result)
{
	std::lock_guard<std::mutex> guard(mu);
	results.push(pair<wstring, wstring>(cmd, result));
}

pair<wstring, wstring> ModemInterface::pop_result()
{
	std::lock_guard<std::mutex> guard(mu);
	pair<wstring, wstring> ret_val(L"", L"");
	if (results.size() > 0)
	{
		ret_val = results.front();
		results.pop();
	}
	return ret_val;
}

size_t ModemInterface::num_results()
{
	std::lock_guard<std::mutex> guard(mu);
	return results.size();
}