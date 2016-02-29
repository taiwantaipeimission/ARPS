#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <ctime>

#include <string>
#include <iostream>

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

Terminal::Terminal()
{
}


Terminal::~Terminal()
{
}

void Terminal::init(std::wstring report_date_in, std::wstring english_date_in, Modem* modem_in, ReportCollection* report_collection_in, CompList* comp_list_in, File* output_file_in)
{
	date = report_date_in;
	english_date = english_date_in;
	modem = modem_in;
	report_collection = report_collection_in;
	comp_list = comp_list_in;
	output_file = output_file_in;
}

void Terminal::init_auto()
{
	cmd_source = COMMAND_SOURCE_LOGIC;
	while (!command_stream.empty())
		command_stream.pop();
	push_command(L"AT");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"ATE0");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGF=0");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGL=4");
	push_command(COMMAND_NEWLINE_CHAR);

	got_modem = true;
	ms_until_timeout = 0;
}

void Terminal::init_user()
{
	cmd_source = COMMAND_SOURCE_USER;
	while (!command_stream.empty())
		command_stream.pop();
	push_command(L"AT");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"ATE0");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGF=0");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGL=4");
	push_command(COMMAND_NEWLINE_CHAR);
	
	got_modem = true;
	ms_until_timeout = 0;
}

void Terminal::send_reminders(bool english)
{
	for (std::map<std::wstring, Area>::iterator ci = comp_list->areas.begin(); ci != comp_list->areas.end(); ++ci)
	{
		bool send_it = false;
		if (english)
		{
			if (ci->first != L"" && report_collection->reports[Report::TYPE_ENGLISH][ReportCollection::COMP].reports.count(english_date + L":" + ci->second.area_name) <= 0 && ci->second.english_required)
				send_it = true;
		}
		else
		{
			if (ci->first != L"" && report_collection->reports[Report::TYPE_REGULAR][ReportCollection::COMP].reports.count(date + L":" + ci->second.area_name) <= 0 && ci->second.report_required)
				send_it = true;
		}
		if (send_it)
		{
			send_message(ci->first, L"Please remember to send in your key indicators.");
		}
	}
}

void Terminal::send_message(std::wstring dest_ph_number, std::wstring msg_contents)
{
	Message msg;
	msg.contents = msg_contents;
	msg.dest_number = dest_ph_number;
	std::vector<std::wstring> strings = encode_msg(&msg);
	for (int i = 0; i < strings.size(); i++)
	{
		std::wstringstream cmd;
		cmd << L"AT+CMGS=";
		cmd << std::dec << (int)(strings[i].length() / 2 - 1);
		cmd << COMMAND_NEWLINE_CHAR;
		cmd << strings[i];
		cmd << COMMAND_ESCAPE_CHAR;
		std::wstring cmd_str = cmd.str();
		push_command(cmd.str());
	}
}

void Terminal::delete_message_from_sim(int msg_cmg_id)
{
	std::wstringstream cmgl_id(L"");
	cmgl_id << std::dec << msg_cmg_id;
	push_command(L"AT+CMGD=");
	push_command(cmgl_id.str());
	push_command(COMMAND_NEWLINE_CHAR);
}

void Terminal::push_command(std::wstring cmd)
{
	for (unsigned int i = 0; i < cmd.length(); i++)
	{
		command_stream.push(cmd[i]);
	}
}

bool Terminal::update(double millis, MessageHandler* msg_handler)
{
	bool ret_value = true;
	ReadFile(modem->file, &modem_ch, 1, &read, NULL);


	//get char from modem
	if (read)
	{
		std::wstring modem_ch_null = L"";
		modem_ch_null += modem_ch;
		std::wcout << modem_ch_null;
		output_file->file << modem_ch_null;
		modem_str += modem_ch_null;
		
		if (modem_str.find(L"OK\r\n") != std::wstring::npos || modem_str.find(L"> ") != std::wstring::npos)
			got_modem = true;

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

	if (cmd_source == COMMAND_SOURCE_LOGIC)
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
		if (got_user)
		{
			if (user_ch != 127)
			{
				command_stream.push(user_ch);
			}
			else
			{
				if (got_modem)
					ret_value = false;
				else
					got_modem = true;
			}
		}
		
	}
	if (got_modem)
	{
		if (command_stream.size() > 0)
		{
			command_ch = command_stream.front();
			std::wstring command_ch_str = L"";
			command_ch_str += command_ch;
			command_stream.pop();
			//if (cmd_source != COMMAND_SOURCE_USER)
				std::wcout << command_ch_str;
			if (command_ch_str == COMMAND_NEWLINE_CHAR)
			{
				WriteFile(modem->file, COMMAND_NEWLINE_CHAR, 1, &written, NULL);
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}
			else if (command_ch_str == COMMAND_ESCAPE_CHAR)
			{
				WriteFile(modem->file, "\u001A", 1, &written, NULL);
				ms_until_timeout = NO_RESPONSE_TIMEOUT_MS;
				got_modem = false;
			}
			else
			{
				WriteFile(modem->file, &command_ch, 1, &written, NULL);
			}	
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