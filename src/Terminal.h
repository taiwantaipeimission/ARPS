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

#include "Modem.h"
#include "ModemInterface.h"
#include "ModemData.h"
#include "File.h"

#include <sstream>
#include <vector>
#include <queue>
#include <Windows.h>

class Terminal
{
public:

	std::mutex mu;

	enum CommandResult
	{
		RESULT_SUCCESS,
		RESULT_ERROR
	};

	Modem modem;
	File* output_file;

	wstring cur_command;
	wstring cur_reply;
	bool got_modem = true;
	bool timeout = false;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_until_timeout;			//time to wait before sending data if no response

	Terminal();
	virtual ~Terminal();

	void init(File* output_file_in);
	wstring run_command_str(wstring command);
	void run_command(Command* command);
	bool update(double millis);
	bool isbusy();
};

