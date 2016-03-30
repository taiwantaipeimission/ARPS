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