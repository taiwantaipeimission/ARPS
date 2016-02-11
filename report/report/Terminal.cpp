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
	: cmd_source(COMMAND_SOURCE_LOGIC), date(date_in), english_date(english_date_in), modem(modem_in), report_sheet(report_sheet_in), english_report_sheet(english_report_sheet_in), comp_list(comp_list_in), output_file(output_file_in), reminders()
{
	std::time(&cur_time);
}


Terminal::~Terminal()
{
}

/*void Terminal::set_mode(TerminalMode new_mode)
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
}*/

void Terminal::init_auto()
{
	cmd_source = COMMAND_SOURCE_LOGIC;
	while (!command_stream.empty())
		command_stream.pop();
	push_command(L"ATE0AT + CMGF = 0\nAT + CMGL = 4\nAT + CMGF = 1\n");
	got_modem = true;
	ms_to_wait = 0;
	cur_messages.clear();
}

void Terminal::init_user()
{
	cmd_source = COMMAND_SOURCE_USER;
	while (!command_stream.empty())
		command_stream.pop();
	push_command(L"ATE0");
	got_modem = true;
	ms_to_wait = 0;
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
		if (it->tm_wday == cur_time_st.tm_wday && (it->tm_hour * 60 + it->tm_min) < (cur_time_st.tm_hour * 60 + cur_time_st.tm_min))
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
						push_command(L"AT+CMGS=\"" + ci->first + L"\"" + L"\nPlease remember to send in your key indicators." + COMMAND_ESCAPE_CHAR);
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

void Terminal::parse_messages(std::wstring raw_str)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find(L"+CMGL:");
	int end = raw_str.find(L"+CMGL:", start + 1) - 1;
	std::map<int, std::vector<int>> concatenated_messages;		//map containing all concatenated msg indeces, keyed by concat_index
	while (start != std::wstring::npos)
	{
		Message msg;
		std::wstring msg_str = raw_str.substr(start, end - start + 1);	//String containing message info as well as CMGL response of the modem
		decode_msg(&msg, msg_str, comp_list);
		cur_messages.push_back(msg);
		if (msg.concatenated)
		{
			concatenated_messages[msg.concat_refnum].push_back(cur_messages.size() - 1);
			std::wcout << msg.contents << L"<text missing>" << std::endl;
		}
		else
		{
			std::wcout << msg.contents << std::endl;
		}
		start = end + 1;
		end = raw_str.find(L"+CMGL:", start + 1) - 1;
	}

	//Attempt to piece together concatenated messages
	for (std::map<int, std::vector<int>>::iterator it = concatenated_messages.begin(); it != concatenated_messages.end(); ++it)
	{
		std::vector<int> correct_order;
		int num_msgs = cur_messages[*it->second.begin()].concat_num_msgs;
		correct_order.assign(num_msgs, -1);		//Fill with -1s so we can see if any msgs are missing
		for (std::vector<int>::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			int this_concat_index = cur_messages[*jt].concat_index;
			correct_order[this_concat_index - 1] = *jt;					//Concat indeces inside msg start from 1, so subtract 1
		}
		if (std::count(correct_order.begin(), correct_order.end(), -1) == 0)	//All sub-msgs found that make up this concat msg
		{
			for (unsigned int i = 1; i < correct_order.size(); i++)
			{
				cur_messages[correct_order[0]].contents += cur_messages[correct_order[i]].contents;
			}
			cur_messages[correct_order[0]].concatenated = false;
			std::wcout << cur_messages[correct_order[0]].contents << std::endl;
		}
	}

	return;
}

void Terminal::process_messages()
{
	for (std::vector<Message>::iterator it = cur_messages.begin(); it != cur_messages.end(); )
	{
		bool remove_this_msg = false;
		if (!it->concatenated)
		{
			if (it->type == Message::TYPE_REPORT)
			{
				Report* report = new Report();
				report->read_message(*it, date);
				report_sheet->add_report(report);

				for (int i = 0; i < it->cmgl_ids.size(); i++)
					push_command(L"AT+CMGD=" + it->cmgl_ids[i] + COMMAND_NEWLINE_CHAR);
				remove_this_msg = true;
			}
			else if (it->type == Message::TYPE_REPORT_ENGLISH)
			{
				ReportEnglish* report = new ReportEnglish();
				report->read_message(*it, english_date);
				english_report_sheet->add_report(report);

				for (int i = 0; i < it->cmgl_ids.size(); i++)
					push_command(L"AT+CMGD=" + it->cmgl_ids[i] + COMMAND_NEWLINE_CHAR);
				remove_this_msg = true;
			}
			else if (it->type == Message::TYPE_REFERRAL)
			{
				Referral referral;
				referral.read_message(*it);
				referral.locate(comp_list);
				if (referral.found_dest())
				{
					it->dest_number = referral.dest_number;
					std::vector<std::wstring> encoded_msgs = encode_msg(&(*it));
					
					push_command(L"AT+CMGF=0" + COMMAND_NEWLINE_CHAR);
					for (int i = 0; i < encoded_msgs.size(); i++)
					{
						std::wstringstream length(L"");
						length << std::dec << (encoded_msgs[i].length() / 2 - 1);
						push_command(L"AT+CMGS=" + length.str() + L"\n" + encoded_msgs[i] + COMMAND_ESCAPE_CHAR);
					}

					for (int i = 0; i < it->cmgl_ids.size(); i++)
						push_command(L"AT+CMGD=" + it->cmgl_ids[i] + COMMAND_NEWLINE_CHAR); 
					remove_this_msg = true;
					push_command(L"AT+CMGF=1" + COMMAND_NEWLINE_CHAR);
					//referral_list.add_sent(referral);
				}
				else
				{
					//referral_list.add_unsent(referral);
				}
			}
		}
		if (remove_this_msg)
		{
			it = cur_messages.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Terminal::push_command(std::wstring cmd)
{
	for (unsigned int i = 0; i < cmd.length(); i++)
	{
		command_stream.push(cmd[i]);
	}
}

bool Terminal::update(double millis)
{
	bool ret_value = true;
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
		if (got_modem && ms_to_wait <= 0)
		{
			if (command_stream.size() > 0)
			{
				command_ch = command_stream.front();
				command_stream.pop();
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
				}
				std::wcout << command_ch;
			}
			else	//eof
			{
				bool reset = false;
				if (modem_str.find(L"+CMGL:") != std::wstring::npos)
				{
					parse_messages(modem_str);
					reset = true;
				}
				if (cur_messages.size() > 0)
				{
					process_messages();
					reset = true;
				}
				if (modem_str.find(L"+CMTI") != std::wstring::npos)
				{
					push_command(L"AT+CMGF=0\nAT+CMGL=0\nAT+CMGF=1\n" + COMMAND_NEWLINE_CHAR);
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
			ret_value = false;
		}
		
	}
	else if (cmd_source == COMMAND_SOURCE_USER)
	{
		//std::wcout << command_stream.str() << std::endl;
		if (got_user)
		{
			if (user_ch != 27)
			{
				command_stream.push(user_ch);
			}
			else
				ret_value = false;
		}
		if (got_modem && ms_to_wait <= 0)
		{
			if (command_stream.size() > 0)
			{
				command_ch = command_stream.front();
				command_stream.pop();
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
				}
				std::wcout << command_ch;
			}
		}
	}
	if (ms_to_wait > 0)
		ms_to_wait = max(0, ms_to_wait - millis);
	return ret_value;
}