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

static const bool is_final_line(std::wstring line)
{
	return line.find(L"OK\r\n") != std::wstring::npos || line.find(L"> ") != std::wstring::npos || line.find(L"ERROR\r\n") != std::wstring::npos;
}

static const bool is_error(std::wstring line)
{
	return line.find(L"ERROR\r\n") != std::wstring::npos;
}

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
	if (read)
	{
		std::wstring modem_ch_null = L"";
		modem_ch_null += modem_ch;
		modem_reply += modem_ch_null;

		if (is_final_line(modem_reply))
		{
			Fl::awake(check_msg_cb, (void*)gui);
			got_modem = true;
		}

		if (ms_until_timeout > 0)
		{
			ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
		}
	}

	//get char from user input
	wchar_t user_ch = '\0';
	if (_kbhit())
	{
		user_ch = _getch();
		if (user_ch != 127)
		{
			std::wstring user_str;
			user_str += user_ch;
		}
		else
		{
			if (got_modem)
				ret_value = false;
			else
				got_modem = true;
		}
	}
	if (got_modem)
	{
		ret_value = false;
	}
	else if (ms_until_timeout <= 0)
	{
		got_modem = true;
		timeout = true;
	}
	if (ms_until_timeout > 0)
		ms_until_timeout = max(0, ms_until_timeout - millis);

	return ret_value;
}

bool Terminal::run_command(std::wstring command)
{
	bool success = false;
	if (got_modem)
	{
		ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
		got_modem = false;
		timeout = false;
		cur_command = command;
		modem_reply = L"";

		std::string narrowed = tos(command);
		size_t len = narrowed.length();
		for (size_t i = 0; i < narrowed.length(); i++)
		{
			std::wstring str = L"";
			str += narrowed[i];
			written = 0;
			for (int i = 0; i < MAX_NUM_TRIES && written == 0; i++)
				WriteFile(modem.file, tos(str).c_str(), 1, &written, NULL);
		}

		clock_t start = clock();
		clock_t end = start;
		while (update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f))
		{
			start = end;
			end = clock();
		}

		std::wcout << modem_reply;
		if (!is_error(modem_reply))
		{
			success = true;
		}
	}
	return success;
}

void Terminal::run()
{
	/*clock_t start = clock();
	clock_t end = start;
	while (1)
	{
		update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f);

		if (modem_data->get_command_stream_size() > 0)
		{
			cur_command = modem_data->pop_command_str();
			if (cur_command.find(COMMAND_ESCAPE_CHAR) != std::wstring::npos)
			{
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}
			if (cur_command.find(COMMAND_NEWLINE_CHAR) != std::wstring::npos)
			{
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}

			std::string narrowed = tos(cur_command);

			size_t len = narrowed.length();
			//if (cmd_source != COMMAND_SOURCE_USER)
			std::wcout << cur_command;


			for (size_t i = 0; i < narrowed.length(); i++)
			{
				std::wstring str = L"";
				str += narrowed[i];
				written = 0;
				for (int i = 0; i < MAX_NUM_TRIES && written == 0; i++)
					WriteFile(modem.file, tos(str).c_str(), 1, &written, NULL);
			}

		}

		start = end;
		end = clock();
	}*/
}

bool Terminal::isbusy()
{
	std::lock_guard<std::mutex> guard(mu);
	return !got_modem;
}