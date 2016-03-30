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

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <ctime>

#include <string>
#include <iostream>

#include "utility.h"
#include "codes.h"
#include "Terminal.h"
#include "Modem.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Message.h"
#include "Area.h"
#include "File.h"
#include "Reminder.h"
#include "Referral.h"
#include "MessageHandler.h"
#include "Gui.h"

#include "FL/Fl.H"

Terminal::Terminal()
{
}


Terminal::~Terminal()
{
}

void Terminal::init(File* output_file_in, Gui* gui_in)
{
	output_file = output_file_in;
	gui = gui_in;

	modem.init();
}

bool Terminal::update(double millis)
{
	bool ret_value = true;
	wchar_t modem_ch = 0;
	ReadFile(modem.file, &modem_ch, 1, &read, NULL);

	//get char from modem
	while(read)
	{
		std::wstring modem_ch_null = L"";
		modem_ch_null += modem_ch;
		cur_reply += modem_ch_null;

		if (is_final_line(cur_reply) || cur_reply == cur_command)
		{
			got_modem = true;
		}

		if (ms_until_timeout > 0)
		{
			ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
		}

		ReadFile(modem.file, &modem_ch, 1, &read, NULL);
	}

	if (got_modem)
	{
		ret_value = false;
	}
	else if (ms_until_timeout <= 0)
	{
		set_color(CC_RED, CC_BLACK);
		std::wcout << L"\nTIMEOUT\n";
		set_color(CC_WHITE, CC_BLACK);
		got_modem = true;
		timeout = true;
	}
	if (ms_until_timeout > 0)
		ms_until_timeout = max(0, ms_until_timeout - millis);

	return ret_value;
}

wstring Terminal::run_command_str(wstring command)
{
	if (got_modem)
	{
		

		clock_t start = clock();
		clock_t end = start;
		while (update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f))
		{
			start = end;
			end = clock();
		}

		got_modem = false;
		timeout = false;
		cur_command = command;
		cur_reply = L"";

		set_color(CC_YELLOW, CC_BLACK);
		std::wcout << cur_command;
		set_color(CC_WHITE, CC_BLACK);
		std::string narrowed = tos(cur_command);
		size_t len = narrowed.length();
		for (size_t i = 0; i < narrowed.length(); i++)
		{
			std::wstring str = L"";
			str += narrowed[i];
			written = 0;
			for (int i = 0; i < MAX_NUM_TRIES && written == 0; i++)
				WriteFile(modem.file, tos(str).c_str(), 1, &written, NULL);
		}

		
		while (update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f))
		{
			start = end;
			end = clock();
		}

		set_color(CC_CYAN, CC_BLACK);
		std::wcout << cur_reply;
		set_color(CC_WHITE, CC_BLACK);

		return cur_reply;
	}
	else
	{
		return L"";
	}
}

void Terminal::run_command(Command* command)
{
	if (command)
	{
		bool had_failure = false;
		do
		{
			for (size_t i = 0; i < command->sub_cmds.size(); i++)
			{
				SubCommand* sub_cmd = &command->sub_cmds[i];
				ms_until_timeout = sub_cmd->timeout_ms;
				sub_cmd->result = run_command_str(sub_cmd->cmd);
				sub_cmd->ran = true;
				sub_cmd->success = !is_error(sub_cmd->result) && !timeout;
				sub_cmd->timeout = timeout;
				if (!sub_cmd->success)
					had_failure = true;
			}
			command->n_times_tried++;
		} while (had_failure && command->n_times_tried < command->n_times_to_try);
	}
}

bool Terminal::isbusy()
{
	std::lock_guard<std::mutex> guard(mu);
	return !got_modem;
}