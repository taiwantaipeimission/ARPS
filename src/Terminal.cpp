#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <ctime>

#include <string>
#include <iostream>

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

void Terminal::init(ModemData* modem_data_in, File* output_file_in, Gui* gui_in)
{
	modem_data = modem_data_in;
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
		std::wcout << modem_ch_null;
		output_file->file << modem_ch_null;
		modem_reply += modem_ch_null;

		if (modem_reply.find(L"OK\r\n") != std::wstring::npos || modem_reply.find(L"> ") != std::wstring::npos)
		{
			modem_data->push_modem_str(modem_reply);
			Fl::awake(check_msg_cb, (void*)gui);
			got_modem = true;
			modem_reply = L"";
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
		got_user = true;
	}
	else
	{
		got_user = false;
	}

	/*if (cmd_source == COMMAND_SOURCE_LOGIC)
	{
		if (got_modem)
		{
			if(command_stream.size() <= 0)	//eof
			{
				if (modem_str.find(L"+CMGL:") != std::wstring::npos)
				{
					msg_handler->parse_messages(this, modem_str, comp_list);
				}
				if (command_stream.size() <= 0)
					ret_value = false;
			}
			modem_str = L"";
		}
	}
	else if (cmd_source == COMMAND_SOURCE_USER)
	{
		
		
	}*/
	if (got_user)
	{
		if (user_ch != 127)
		{
			std::wstring user_str(L"");
			user_str += user_ch;
			modem_data->push_command(user_str);
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
		if (modem_data->get_command_stream_size() > 0)
		{
			std::wstring command_ch_str = L"";
			command_ch_str += modem_data->pop_command_ch();
			//if (cmd_source != COMMAND_SOURCE_USER)
				std::wcout << command_ch_str;
			if (command_ch_str == COMMAND_NEWLINE_CHAR)
			{
				WriteFile(modem.file, COMMAND_NEWLINE_CHAR, 1, &written, NULL);
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}
			else if (command_ch_str == COMMAND_ESCAPE_CHAR)
			{
				WriteFile(modem.file, "\u001A", 1, &written, NULL);
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}
			else
			{
				WriteFile(modem.file, command_ch_str.c_str(), 1, &written, NULL);
			}	
		}
		else
		{
			ret_value = false;
		}
	}
	else if (ms_until_timeout <= 0)
	{
		std::wcout << L"TIMEOUT\n";
		got_modem = true;
	}
	if (ms_until_timeout > 0)
		ms_until_timeout = max(0, ms_until_timeout - millis);

	return ret_value;
}

void Terminal::run()
{
	clock_t start = clock();
	clock_t end = start;
	while (1)
	{
		update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f);
		start = end;
		end = clock();
	}
}