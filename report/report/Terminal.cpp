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

void Terminal::init_auto()
{
	cmd_source = COMMAND_SOURCE_LOGIC;
	while (!command_stream.empty())
		command_stream.pop();
	push_command(L"AT");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGF=0");
	push_command(COMMAND_NEWLINE_CHAR);
	push_command(L"AT+CMGL=4");
	push_command(COMMAND_NEWLINE_CHAR);

	//send_message(L"+886910358944", L"TYPE:REFERRAL\nZHONGSHAN\n歐巴馬\nThis guy is actually really cool. We met him on the street. He's black, and I think he's from America. He speaks really good English and is super 帥. This text message is also really long.");

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
	push_command(COMMAND_NEWLINE_CHAR);
	
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
						send_message(ci->first, L"\nPlease remember to send in your key indicators.");
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
				cur_messages[correct_order[0]].cmgl_ids.push_back(cur_messages[correct_order[i]].cmgl_ids[0]);
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
				{
					delete_message(it->cmgl_ids[i]);
				}
				remove_this_msg = true;
			}
			else if (it->type == Message::TYPE_REPORT_ENGLISH)
			{
				ReportEnglish* report = new ReportEnglish();
				report->read_message(*it, english_date);
				english_report_sheet->add_report(report);

				for (int i = 0; i < it->cmgl_ids.size(); i++)
				{
					delete_message(it->cmgl_ids[i]);
				}
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
					send_message(it->dest_number, it->contents);

					for (int i = 0; i < it->cmgl_ids.size(); i++)
					{
						delete_message(it->cmgl_ids[i]);
					}
					remove_this_msg = true;
					//referral_list.add_sent(referral);
				}
				else
				{
					//referral_list.add_unsent(referral);
				}
			}
			else if (it->type == Message::TYPE_UNKNOWN)
			{
				output_file->file << it->sender_name << L":" << it->sender_number << L":" << it->contents << std::endl;
				for (int i = 0; i < it->cmgl_ids.size(); i++)
				{
					delete_message(it->cmgl_ids[i]);
				}
				remove_this_msg = true;
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

void Terminal::delete_message(int msg_cmg_id)
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

			if(command_stream.size() <= 0)	//eof
			{
				if (modem_str.find(L"+CMGL:") != std::wstring::npos)
				{
					parse_messages(modem_str);
					modem_str = L"";
				}
				if (cur_messages.size() > 0)
				{
					process_messages();
				}
				if (modem_str.find(L"+CMTI") != std::wstring::npos)
				{
					push_command(L"AT+CMGL=0");
					push_command(COMMAND_NEWLINE_CHAR);
				}
				send_reminders();
			}
		}
		if (got_user && user_ch == 27)
		{
			ret_value = false;
		}
		
	}
	else if (cmd_source == COMMAND_SOURCE_USER)
	{
		if (got_user)
		{
			if (user_ch == ',')
			{
				send_message(L"+886972576566", L"Hello there!");
			}
			else if (user_ch != 27)
			{
				if (user_ch == '\r')
					std::wcout << std::endl;
				std::wcout << user_ch;
				command_stream.push(user_ch);
			}
			else
				ret_value = false;
		}
		
	}
	if (got_modem && ms_to_wait <= 0)
	{
		if (command_stream.size() > 0)
		{
			command_ch = command_stream.front();
			std::wstring command_ch_str = L"";
			command_ch_str += command_ch;
			command_stream.pop();
			if (cmd_source != COMMAND_SOURCE_USER)
				std::wcout << command_ch_str;
			if (command_ch_str == COMMAND_NEWLINE_CHAR)
			{
				WriteFile(modem->file, COMMAND_NEWLINE_CHAR, 1, &written, NULL);
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
		}
	}
	if (ms_to_wait > 0)
		ms_to_wait = max(0, ms_to_wait - millis);
	return ret_value;
}