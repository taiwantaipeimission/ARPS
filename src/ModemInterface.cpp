#include "ModemInterface.h"



ModemInterface::ModemInterface()
{
}


ModemInterface::~ModemInterface()
{
}

void ModemInterface::push_command(wstring cmd)
{
	commands.push(cmd);
}

wstring ModemInterface::pop_command()
{
	wstring ret_val(L"");
	if (commands.size() > 0)
	{
		ret_val = commands.front();
		commands.pop();
	}
	return ret_val;
}

void ModemInterface::push_result(wstring cmd, wstring result)
{
	results.push(pair<wstring, wstring>(cmd, result));
}

pair<wstring, wstring> ModemInterface::pop_result()
{
	pair<wstring, wstring> ret_val(L"", L"");
	if (results.size() > 0)
	{
		ret_val = results.front();
		results.pop();
	}
	return ret_val;
}