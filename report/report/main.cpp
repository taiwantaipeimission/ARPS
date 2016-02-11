﻿#include <string.h>
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include "include/utf8.h"

#include "Modem.h"
#include "FileManager.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Terminal.h"
#include "Area.h"
#include "Reminder.h"
#include "Message.h"

// File handles
#define OUTPUT L"OUTPUT"
#define PH_LIST L"PH_LIST"
#define REPORT_DATA L"REPORT_DATA"
#define REPORT_DATA_OLD L"REPORT_DATA_OLD"
#define REPORT_DATA_ZONE L"REPORT_DATA_ZONE"
#define ENGLISH_DATA L"ENGLISH_DATA"
#define ENGLISH_DATA_OLD L"ENGLISH_DATA_OLD"
#define ENGLISH_DATA_UNIT L"ENGLISH_DATA_UNIT"
#define REFERRAL_ROUTING_TABLE L"REFERRAL_ROUTING_TABLE"
#define REFERRAL_HISTORY L"REFERRAL_HISTORY"

//Characters
#define DATE_STAMP_SEPARATOR_CHAR L":"

using namespace std;

std::wstring tos(int x)
{
	wchar_t str[256] = L"";
	_itow_s(x, str, 256, 10);
	return str;
}

void show_report_status(ReportCollection* report_collection, CompList* comp_list, std::wstring date, bool english = false)
{
	std::cout << "Received\t\tNot received\n=============================================" << std::endl;
	for (std::map<std::wstring, Area>::iterator it = comp_list->areas.begin(); it != comp_list->areas.end(); ++it)
	{
		std::wstring id_str = date + L":" + it->second.area_name;
		if (!english || it->second.english_unit_name != L"")
		{
			if (report_collection->report_by_comp.reports.count(id_str) > 0)
			{
				std::wcout << it->second.area_name << std::endl;
			}
			else
			{
				std::wcout << L"\t\t\t" << it->second.area_name << std::endl;
			}
		}
	}
}

void save(FileManager* file_manager, ReportCollection* report_collection, ReportCollection* report_collection_english, CompList* comp_list, std::wstring date, std::wstring english_date)
{
	//open output files
	file_manager->open_file(REPORT_DATA, File::FILE_TYPE_OUTPUT);
	file_manager->open_file(REPORT_DATA_ZONE, File::FILE_TYPE_OUTPUT);

	file_manager->open_file(ENGLISH_DATA, File::FILE_TYPE_OUTPUT);
	file_manager->open_file(ENGLISH_DATA_UNIT, File::FILE_TYPE_OUTPUT);


	report_collection->write_report_by_comp(file_manager->files[REPORT_DATA]);
	report_collection->calculate_report_by_zone(comp_list, date);
	report_collection->write_report_by_zone(file_manager->files[REPORT_DATA_ZONE]);

	report_collection_english->write_report_by_comp(file_manager->files[ENGLISH_DATA]);
	report_collection_english->calculate_report_by_zone(comp_list, english_date, true);
	report_collection_english->write_report_by_zone(file_manager->files[ENGLISH_DATA_UNIT]);

	file_manager->close_file(REPORT_DATA);
	file_manager->close_file(REPORT_DATA_ZONE);
	file_manager->close_file(ENGLISH_DATA);
	file_manager->close_file(ENGLISH_DATA_UNIT);
}

/* Create the date stamp for a reporting period, based on the current time and the weekday of reporting.
 * All days up to the day of reporting return back to the previous reporting period (e.g. Thursday-Tuesday will be counted as reports
 * for English reporting session which began on the Wednesday previous).
 */
std::wstring get_report_date_str(std::wstring report_wday)
{
	std::wstring report_date;
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);

	int cur_wday = (curtime_st.tm_wday == 0 ? 7 : curtime_st.tm_wday);
	if (cur_wday == _wtoi(report_wday.c_str()))
	{
		report_date = tos(1900 + curtime_st.tm_year)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(curtime_st.tm_mon + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(curtime_st.tm_mday / 7 + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + report_wday;
	}
	else
	{
		tm last_week_tm_st = curtime_st;
		last_week_tm_st.tm_mday -= 7;
		time_t last_week_tm = mktime(&last_week_tm_st);
		localtime_s(&last_week_tm_st, &last_week_tm);
		report_date = tos(1900 + last_week_tm_st.tm_year)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(last_week_tm_st.tm_mon + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(last_week_tm_st.tm_mday / 7 + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + report_wday;
	}
	return report_date;
}

class Console_streambuf
	: public std::basic_streambuf<wchar_t>
{
	HANDLE m_out;
public:
	Console_streambuf(HANDLE out) : m_out(out) {}

	virtual int_type overflow(int_type c = traits_type::eof())
	{
		wchar_t wc = c;
		DWORD numberOfCharsWritten;
		BOOL res = WriteConsoleW(m_out, &wc, 1, &numberOfCharsWritten, NULL);
		(void)res;
		return 1;
	}
};


int main(int argc, char **argv)
{
	Message msg;

	msg.contents = L"TYPE:REFERRAL\nZHONGSHAN\n歐巴馬\nThis guy is actually really cool.  We met him on the street.  He's black, and I think he's from America.  He speaks really good English and is super 帥. This text message is also really long.";
	msg.dest_number = L"+886972576566";

	std::vector<std::wstring> strings;

	strings = encode_msg(&msg);

	for (int i = 0; i < strings.size(); i++)
		std::wcout << strings[i] << endl;





		







	Console_streambuf out(GetStdHandle(STD_OUTPUT_HANDLE));
	auto old_buf = std::wcout.rdbuf(&out);
	std::wcout << "Loading..." << std::endl;
	













	FileManager file_manager(L"paths.txt");
	file_manager.open_file(OUTPUT, File::FILE_TYPE_OUTPUT, true);
	file_manager.open_file(PH_LIST, File::FILE_TYPE_INPUT);
	file_manager.open_file(REPORT_DATA, File::FILE_TYPE_INPUT);
	file_manager.open_file(ENGLISH_DATA, File::FILE_TYPE_INPUT);
	file_manager.open_file(REFERRAL_ROUTING_TABLE, File::FILE_TYPE_OUTPUT);

	file_manager.open_file(REPORT_DATA_OLD, File::FILE_TYPE_OUTPUT, true);
	file_manager.files[REPORT_DATA_OLD]->file << "=========================";
	file_manager.files[REPORT_DATA_OLD]->file << file_manager.files[REPORT_DATA]->file.rdbuf();
	file_manager.close_file(REPORT_DATA_OLD);

	std::wstring report_wday = file_manager.config_file.values[L"REPORT_WDAY"];
	std::wstring english_wday = file_manager.config_file.values[L"ENGLISH_WDAY"];
	Reminder report_reminder(file_manager.config_file.values[L"REPORT_REMINDER"]);
	Reminder english_reminder(file_manager.config_file.values[L"ENGLISH_REMINDER"]);
	english_reminder.english = true;
	std::wstring report_date = get_report_date_str(report_wday);
	std::wstring english_date = get_report_date_str(english_wday);
	
	
	ReportCollection report_collection;
	ReportCollection report_collection_english;
	report_collection_english.report_by_comp.report_type = Report::TYPE_ENGLISH;
	report_collection_english.report_by_zone.report_type = Report::TYPE_ENGLISH;
	CompList comp_list;

	report_collection.read_report_by_comp(file_manager.files[REPORT_DATA]);
	report_collection_english.read_report_by_comp(file_manager.files[ENGLISH_DATA]);
	comp_list.load(file_manager.files[PH_LIST]->file);

	Modem modem;
	std::wstringstream command;
	Terminal terminal(report_date, english_date, &modem, &report_collection.report_by_comp, &report_collection_english.report_by_comp, &comp_list, file_manager.files[OUTPUT]);
	terminal.add_reminder(report_reminder);
	terminal.add_reminder(english_reminder);

	//close input files
	file_manager.close_file(REPORT_DATA);
	file_manager.close_file(ENGLISH_DATA);
	file_manager.close_file(REFERRAL_ROUTING_TABLE);

	// process string

	

	bool quit = false;
	bool run_terminal = false;
	
	while (!quit)
	{
		std::cout << "1. Start\n2. Report status\n3. English report status\n4. Terminal\n5. Save\n6. Quit" << std::endl;
		char input_choice;
		std::cin >> input_choice;
		report_date = get_report_date_str(report_wday);
		english_date = get_report_date_str(english_wday);
		if (input_choice == '1')
		{
			terminal.init_auto();
			run_terminal = true;
		}
		else if (input_choice == '2')
		{
			show_report_status(&report_collection, &comp_list, report_date, false);
			run_terminal = false;
		}
		else if (input_choice == '3')
		{
			show_report_status(&report_collection_english, &comp_list, english_date, true);
			run_terminal = false;
		}
		else if (input_choice == '4')
		{
			terminal.init_user();
			run_terminal = true;
		}
		else if (input_choice == '5')
		{
			save(&file_manager, &report_collection, &report_collection_english, &comp_list, report_date, english_date);
			run_terminal = false;
		}
		else if (input_choice == '6')
		{
			quit = true;
			run_terminal = false;
		}

		// basic terminal loop:

		clock_t start = clock();
		clock_t end = start;
		while (run_terminal && terminal.update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f))
		{
			start = end;
			end = clock();
		}
	}

	//save

	save(&file_manager, &report_collection, &report_collection_english, &comp_list, report_date, english_date);
	
	//close output files
	file_manager.close_file(OUTPUT);
	
	std::wcout.rdbuf(old_buf);
	return 0;
}