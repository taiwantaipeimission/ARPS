/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ModemData.h"

ModemData::ModemData()
{
	int x = 0;
}


ModemData::~ModemData()
{
}

void ModemData::push_command(std::wstring cmd, std::wstring terminator)
{
	std::lock_guard<std::mutex> guard(mu);
	std::wstring str = cmd + terminator;
	command_stream.push(str);
}

std::wstring ModemData::pop_command_str()
{
	std::lock_guard<std::mutex> guard(mu);
	std::wstring command_str = L"";
	if (command_stream.size() > 0)
	{
		command_str = command_stream.front();
		command_stream.pop();
	}
	return command_str;
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

std::queue<std::wstring> ModemData::get_modem_strs()
{
	std::lock_guard<std::mutex> guard(mu);
	return modem_str;
}

void ModemData::push_modem_str(std::wstring modem_chars)
{
	std::lock_guard<std::mutex> guard(mu);
	modem_str.push(modem_chars);
}

std::wstring ModemData::pop_modem_str()
{
	std::lock_guard<std::mutex> guard(mu);
	std::wstring ret_val = L"";
	if (modem_str.size() > 0)
	{
		ret_val = modem_str.front();
		modem_str.pop();
	}
	return ret_val;
}

void ModemData::clear_modem_strs()
{
	std::lock_guard<std::mutex> guard(mu);
	while (modem_str.size() > 0)
		modem_str.pop();
}