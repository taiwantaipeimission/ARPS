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
		: cmd(cmd_str), result(L""), ran(false), success(false), timeout_ms(g_no_response_timeout_ms)
	{
		if (cmd.find(g_command_escape_char) != wstring::npos)
			timeout_ms = g_msg_no_response_timeout_ms;
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
	double timeout_ms;
};

class Command
{
public:

	enum Type
	{
		TYPE_CHECK_MSG,
		TYPE_SEND_MSG,
		TYPE_OTHER
	};

	Command(Type type_in = TYPE_OTHER)
		: type(type_in), n_times_to_try(1), n_times_tried(0), ran_all(false), success_all(false), msg(NULL), sub_cmds()
	{
	}

	~Command()
	{}

	Type type;
	int n_times_to_try;
	int n_times_tried;
	bool ran_all;
	bool success_all;
	Message* msg;	//If the command is of TYPE_SEND_MSG, this contains a pointer to its associated message.
	vector<SubCommand> sub_cmds;
};