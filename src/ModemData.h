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

#pragma once

#include "codes.h"

#include <queue>
#include <string>
#include <mutex>

class ModemData
{
	std::mutex mu;
	std::queue<std::wstring> command_stream;		//To send to the modem
	std::queue<std::wstring> modem_str;					//string of consecutive chars received from the modem

public:

	ModemData();
	virtual ~ModemData();

	void push_command(std::wstring cmd, std::wstring terminator = g_command_newline_char);
	std::wstring pop_command_str();
	size_t get_command_stream_size();
	void clear_command_stream();
	
	void push_modem_str(std::wstring modem_chars);
	std::queue<std::wstring> get_modem_strs();
	std::wstring pop_modem_str();
	void clear_modem_strs();
};