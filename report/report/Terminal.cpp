#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <ctime>

#include <string>
#include <iostream>

#include "Terminal.h"
#include "Modem.h"
#include "ReportSheet.h"
#include "ReportEnglish.h"
#include "CompList.h"
#include "Message.h"
#include "Area.h"
#include "File.h"

Terminal::Terminal(std::string date_in, std::string english_date_in, Modem* modem_in, ReportSheet* report_sheet_in, ReportSheet* english_report_sheet_in, CompList* comp_list_in, File* output_file_in)
	: mode(MODE_AUTOMATIC), date(date_in), english_date(english_date_in), modem(modem_in), report_sheet(report_sheet_in), english_report_sheet(english_report_sheet_in), comp_list(comp_list_in), output_file(output_file_in), reminders()
{
	time_t cur_time;
	std::time(&cur_time);
	struct tm reminder_time;
	localtime_s(&reminder_time, &cur_time);

	//reminders.push_back(mktime(&reminder_time));
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

void Terminal::add_reminder(tm* time)
{
	reminders.push_back(mktime(time));
}

void Terminal::send_reminders()
{
	for (std::map<std::string, Area>::iterator it = comp_list->areas.begin(); it != comp_list->areas.end(); ++it)
	{
		if (it->first != "" && report_sheet->reports.count(date + ":" + it->second.area_name) <= 0)
		{
			command_stream.str(command_stream.str() + "AT+CMGS=\"" + it->first + "\"" + "\nPlease remember to send in your key indicators." + COMMAND_ESCAPE_CHAR + COMMAND_NEWLINE_CHAR);
		}
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
			output_file->file << modem_ch_null;
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
				command_stream.get(command_ch);
				if (command_ch == COMMAND_NEWLINE_CHAR)
				{
					WriteFile(modem->file, "\n", 3, &written, NULL);
					wait_ms = TIMEOUT_MS;
					got_modem = false;
				}
				else if (command_ch == COMMAND_ESCAPE_CHAR)
				{
					WriteFile(modem->file, "\u001A", 1, &written, NULL);
					wait_ms = 0;
					got_modem = false;
				}
				else if (command_ch != COMMAND_END_CHAR)
				{
					WriteFile(modem->file, &command_ch, 1, &written, NULL);
					wait_ms = 0;
					got_modem = false;
				}
				else if (command_ch == COMMAND_END_CHAR)
				{
					command_stream.str("");
					command_stream.clear();
					command_stream.seekg(0, std::ios::beg);

					bool reset = false;

					if (modem_str.find("+CMGL:") != std::string::npos)
					{
						parse_messages(modem_str);

						for (std::vector<Message>::iterator it = cur_messages.begin(); it != cur_messages.end(); ++it)
						{
							if (it->type == Message::TYPE_REPORT)
							{
								Report* report = new Report();
								report->read_message(*it, date);
								report_sheet->add_report(report);

								command_stream.str(command_stream.str() + "AT+CMGD=" + it->cmgl_id + "\n");
							}
							else if (it->type == Message::TYPE_REPORT_ENGLISH)
							{
								ReportEnglish* report = new ReportEnglish();
								report->read_message(*it, english_date);
								english_report_sheet->add_report(report);

								command_stream.str(command_stream.str() + "AT+CMGD=" + it->cmgl_id + "\n");
							}
						}
						command_stream.str(command_stream.str() + COMMAND_END_CHAR);
						cur_messages.clear();
						reset = true;
					}

					if (modem_str.find("+CMTI") != std::string::npos)
					{
						command_stream.str(command_stream.str() + "AT+CMGL=\"ALL\"\n");
						reset = true;
					}

					time_t cur_time;
					std::time(&cur_time);
					for (std::vector<time_t>::iterator it = reminders.begin(); it != reminders.end(); )
					{
						double time_diff = std::difftime(*it, cur_time);
						if (time_diff <= 0.0f)
						{
							send_reminders();
							it = reminders.erase(it);
							reset = true;
						}
						else ++it;
					}

					if (reset)
					{
						got_modem = true;
						modem_str = "";
					}
					command_stream.str(command_stream.str() + COMMAND_END_CHAR);
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