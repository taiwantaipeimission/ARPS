#include "ModemData.h"

ModemData::ModemData()
{
	int x = 0;
}


ModemData::~ModemData()
{
}

void ModemData::push_command(std::wstring cmd)
{
	std::lock_guard<std::mutex> guard(mu);
	for (unsigned int i = 0; i < cmd.length(); i++)
	{
		command_stream.push(cmd[i]);
	}
}

wchar_t ModemData::pop_command_ch()
{
	std::lock_guard<std::mutex> guard(mu);
	wchar_t command_ch = command_stream.front();
	command_stream.pop();
	return command_ch;
}

size_t ModemData::get_command_stream_size()
{
	std::lock_guard<std::mutex> guard(mu);
	return command_stream.size();
}

void ModemData::clear_command_stream()
{
	std::lock_guard<std::mutex> guard(mu);
	while (!command_stream.empty())
		command_stream.pop();
}

void ModemData::push_modem_str(std::wstring modem_chars)
{
	std::lock_guard<std::mutex> guard(mu);
	modem_str += modem_chars;
}

std::wstring ModemData::get_modem_str()
{
	std::lock_guard<std::mutex> guard(mu);
	return modem_str;
}

void ModemData::clear_modem_str()
{
	std::lock_guard<std::mutex> guard(mu);
	modem_str = L"";
}