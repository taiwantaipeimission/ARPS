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
		//std::wcout << L"\nTIMEOUT\n";
		//got_modem = true;
		//timeout = true;
	}
	if (ms_until_timeout > 0)
		ms_until_timeout = max(0, ms_until_timeout - millis);

	return ret_value;
}

std::wstring Terminal::run_command(std::wstring command)
{
	if (got_modem)
	{
		ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
		got_modem = false;
		timeout = false;
		cur_command = command;
		modem_reply = L"";


		std::wcout << command;
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
		return modem_reply;
	}
	else
	{
		return L"";
	}
}

bool Terminal::isbusy()
{
	std::lock_guard<std::mutex> guard(mu);
	return !got_modem;
}