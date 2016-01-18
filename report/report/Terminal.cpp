#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>

#include <string>
#include <iostream>

#include "Terminal.h"
#include "Modem.h"
#include "ReportSheet.h"
#include "CompList.h"
#include "Message.h"

Terminal::Terminal(std::string date_in, Modem* modem_in, ReportSheet* report_sheet_in, CompList* comp_list_in, std::fstream* output_file_in)
	: mode(MODE_AUTOMATIC), date(date_in), modem(modem_in), report_sheet(report_sheet_in), comp_list(comp_list_in), output_file(output_file_in)
{
	set_mode(MODE_AUTOMATIC);
}


Terminal::~Terminal()
{
}

Terminal::TerminalMode Terminal::get_mode()
{
	return mode;
}

void Terminal::set_mode(TerminalMode new_mode)
{
	if (mode == MODE_AUTOMATIC)
	{
		//write the raw msg output to file
		//output_file->clear();
		//output_file->seekg(0, std::ios::beg);
		//report_sheet->read_unprocessed(modem_str, date, comp_list);

		
	}

	if (new_mode == MODE_AUTOMATIC)
	{
		command_stream.clear();
		command_stream.seekg(0, std::ios::beg);
		command_stream.str("AT+CMGL=\"ALL\"\n~");
		
		got_modem = true;
		wait_ms = 0;
	}

	mode = new_mode;
}

void Terminal::parse_messages(std::string raw_str)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find("+CMGL:");
	int end = raw_str.find("+CMGL:", start + 1) - 1;
	while (start != std::string::npos)
	{
		Message msg;
		msg.parse(raw_str.substr(start, end - start + 1), comp_list);
		cur_messages.push_back(msg);

		start = end + 1;
		end = raw_str.find("+CMGL:", start + 1) - 1;
	}
}

void Terminal::update(int millis)
{
		ReadFile(modem->file, &modem_ch, 1, &read, NULL);

		//get char from modem
		if (read)
		{
			std::string modem_ch_null = "";
			modem_ch_null += modem_ch;
			std::cout << modem_ch_null;
			//(*output_file) << modem_ch_null;
			modem_str += modem_ch_null;
			got_modem = true;

			if (wait_ms > 0)
				wait_ms = TIMEOUT_MS;	//reset wait timer: wait for another TIMOUT_MS ms before writing
		}

		//get char from user input
		if (_kbhit())
		{
			user_ch = _getch();
			got_user = true;
		}
		else
		{
			got_user = false;
		}

		if (mode == MODE_AUTOMATIC)
		{
			if (got_modem && wait_ms <= 0)
			{
				got_modem = false;
				
				command_stream.get(command_ch);
				if (command_ch == COMMAND_NEWLINE_CHAR)
				{
					WriteFile(modem->file, "\r\n", 3, &written, NULL);
					wait_ms = TIMEOUT_MS;
				}
				else if (command_ch == COMMAND_ESCAPE_CHAR)
				{
					WriteFile(modem->file, "\u001A", 1, &written, NULL);
					wait_ms = 0;
				}
				else if (command_ch != COMMAND_END_CHAR)
				{
					WriteFile(modem->file, &command_ch, 1, &written, NULL);
					wait_ms = 0;
				}
				else if (command_ch == COMMAND_END_CHAR)
				{
					if (modem_str.find("+CMTI") != std::string::npos)
					{
						command_stream.clear();
						command_stream.seekg(0, std::ios::beg);
						command_stream.str("AT+CMGL=\"ALL\"\n~");
					}

					if (modem_str != "" && modem_str.find("+CMGL: ") != std::string::npos)
					{
						parse_messages(modem_str);
					}

					modem_str = "";

					for (std::vector<Message>::iterator it = cur_messages.begin(); it != cur_messages.end(); ++it)
					{
						if (it->type == Message::TYPE_REPORT)
						{
							ReportRegular report(*it, date);
							report_sheet->add_report(report);
						}
					}
					cur_messages.clear();
				}
			}
			if (got_user && user_ch == 27)
			{
				set_mode(MODE_INACTIVE);
			}
			wait_ms-=millis;
		}

		else if (mode == MODE_USER_INPUT)
		{
			if (got_user)
			{
				if (user_ch != 27)
					WriteFile(modem->file, &user_ch, 1, &written, NULL);
				else
					set_mode(MODE_INACTIVE);
			}
		}
};