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
#include "Reminder.h"
#include "Referral.h"

Terminal::Terminal(std::wstring date_in, std::wstring english_date_in, Modem* modem_in, ReportSheet* report_sheet_in, ReportSheet* english_report_sheet_in, CompList* comp_list_in, File* output_file_in)
	: mode(MODE_AUTOMATIC), date(date_in), english_date(english_date_in), modem(modem_in), report_sheet(report_sheet_in), english_report_sheet(english_report_sheet_in), comp_list(comp_list_in), output_file(output_file_in), reminders()
{
	std::time(&cur_time);

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
		command_stream.str(L"AT+CMGF=0\nAT+CMGL=4\nAT+CMGF=1\n");
		
		got_modem = true;
		ms_to_wait = 0;
	}

	mode = new_mode;
}

void Terminal::parse_messages(std::wstring raw_str)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find(L"+CMGL:");
	int end = raw_str.find(L"+CMGL:", start + 1) - 1;
	while (start != std::wstring::npos)
	{
		Message msg;
		msg.parse(raw_str.substr(start, end - start + 1), comp_list);
		cur_messages.push_back(msg);

		start = end + 1;
		end = raw_str.find(L"+CMGL:", start + 1) - 1;
	}
}

void Terminal::add_reminder(Reminder reminder)
{
	reminders.push_back(reminder);
}

bool Terminal::send_reminders()
{
	bool sent = false;
	struct tm cur_time_st;
	localtime_s(&cur_time_st, &cur_time);
	for (std::vector<Reminder>::iterator it = reminders.begin(); it != reminders.end(); ++it)
	{
		if (it->tm_wday == cur_time_st.tm_wday)
		{
			if (!it->sent)
			{
				for (std::map<std::wstring, Area>::iterator ci = comp_list->areas.begin(); ci != comp_list->areas.end(); ++ci)
				{
					bool send_it = false;
					if (it->english)
					{
						if (ci->first != L"" && english_report_sheet->reports.count(english_date + L":" + ci->second.area_name) <= 0 && ci->second.english_unit_name != L"NONE")
							send_it = true;
					}
					else
					{
						if (ci->first != L"" && report_sheet->reports.count(date + L":" + ci->second.area_name) <= 0)
							send_it = true;
					}
					if (send_it)
					{
						command_stream.str(command_stream.str() + L"AT+CMGS=\"" + ci->first + L"\"" + L"\nPlease remember to send in your key indicators." + COMMAND_ESCAPE_CHAR + COMMAND_NEWLINE_CHAR);
						it->sent = true;
						sent = true;
					}
				}
			}
		}
		else
		{
			it->sent = false;
		}
	}
	return sent;
}

void Terminal::update(double millis)
{
		ReadFile(modem->file, &modem_ch, 1, &read, NULL);

		time(&cur_time);

		//get char from modem
		if (read)
		{
			std::wstring modem_ch_null = L"";
			modem_ch_null += modem_ch;
			std::wcout << modem_ch_null;
			output_file->file << modem_ch_null;
			modem_str += modem_ch_null;
			got_modem = true;

			if (ms_to_wait > 0)
				ms_to_wait = TIMEOUT_MS;	//reset wait timer: wait for another TIMOUT_MS ms before writing
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
			if (got_modem && ms_to_wait <= 0)
			{
				command_stream.get(command_ch);
				if (!command_stream.eof())
				{
					if (command_ch == COMMAND_NEWLINE_CHAR)
					{
						WriteFile(modem->file, &command_ch, 1, &written, NULL);
						ms_to_wait = TIMEOUT_MS;
						got_modem = false;
					}
					else if (command_ch == COMMAND_ESCAPE_CHAR)
					{
						WriteFile(modem->file, "\u001A", 1, &written, NULL);
						ms_to_wait = MSG_TIMEOUT_MS;
						got_modem = false;
					}
					else
					{
						WriteFile(modem->file, &command_ch, 1, &written, NULL);
						got_modem = false;
					}
				}
				else	//eof
				{
					bool reset = false;
					command_stream.str(L"");
					command_stream.clear();
					command_stream.seekg(0, std::ios::beg);

					if (modem_str.find(L"+CMGL:") != std::wstring::npos)
					{
						parse_messages(modem_str);

						for (std::vector<Message>::iterator it = cur_messages.begin(); it != cur_messages.end(); ++it)
						{
							if (it->type == Message::TYPE_REPORT)
							{
								Report* report = new Report();
								report->read_message(*it, date);
								report_sheet->add_report(report);

								command_stream.str(command_stream.str() + L"AT+CMGD=" + it->cmgl_id + COMMAND_NEWLINE_CHAR);
							}
							else if (it->type == Message::TYPE_REPORT_ENGLISH)
							{
								ReportEnglish* report = new ReportEnglish();
								report->read_message(*it, english_date);
								english_report_sheet->add_report(report);

								command_stream.str(command_stream.str() + L"AT+CMGD=" + it->cmgl_id + COMMAND_NEWLINE_CHAR);
							}
							else if (it->type == Message::TYPE_REFERRAL)
							{
								Referral referral;
								referral.read_message(*it);
								referral.locate(comp_list);
								if (referral.found_dest())
								{
									command_stream.str(command_stream.str() + L"AT+CMGS=" + referral.dest_number +
																			  L"\nName:" + referral.name +
																			  L"\nNumber:" + referral.number +
																			  L"\nInfo:" + referral.info +
																			  COMMAND_ESCAPE_CHAR + COMMAND_NEWLINE_CHAR + L"AT+CMGD=" + it->cmgl_id + COMMAND_NEWLINE_CHAR);
									//referral_list.add_sent(referral);
								}
								else
								{
									//referral_list.add_unsent(referral);
								}
							}
						}
						cur_messages.clear();
						reset = true;
					}

					if (modem_str.find(L"+CMTI") != std::wstring::npos)
					{
						command_stream.str(command_stream.str() + L"AT+CMGF=0\nAT+CMGL=4\nAT+CMGF=1\n" + COMMAND_NEWLINE_CHAR);
						reset = true;
					}

					if (send_reminders())
					{
						reset = true;
					}

					if (reset)
					{
						got_modem = true;
						modem_str = L"";
					}
				}
			}
			if (got_user && user_ch == 27)
			{
				set_mode(MODE_INACTIVE);
			}
			if (ms_to_wait > 0)
				ms_to_wait = max(0, ms_to_wait - millis);
		}

		else 
		{
			if (mode == MODE_USER_INPUT)
			{
				if (got_user)
				{
					if (user_ch != 27)
						WriteFile(modem->file, &user_ch, 1, &written, NULL);
					else
						set_mode(MODE_INACTIVE);
				}
			}
		}
};