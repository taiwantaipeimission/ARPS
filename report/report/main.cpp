#include <string.h>
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

#include "Modem.h"
#include "FileManager.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Terminal.h"
#include "Area.h"
#include "Reminder.h"

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

std::string to_utf8(const wchar_t* buffer, int len)
{
	int nChars = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		NULL,
		0,
		NULL,
		NULL);
	if (nChars == 0) return "";

	std::string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		const_cast< char* >(newbuffer.c_str()),
		nChars,
		NULL,
		NULL);

	return newbuffer;
}

std::string to_utf8(const std::wstring& str)
{
	return to_utf8(str.c_str(), (int)str.size());
}

int main(int argc, char **argv)
{





	FileManager file_manager(L"paths.txt");
	file_manager.open_file(OUTPUT, File::FILE_TYPE_OUTPUT, true);
	file_manager.files[OUTPUT]->file << L"Test陳la";

	file_manager.open_file(PH_LIST, File::FILE_TYPE_INPUT);
	file_manager.open_file(REPORT_DATA, File::FILE_TYPE_INPUT);
	file_manager.open_file(ENGLISH_DATA, File::FILE_TYPE_INPUT);
	file_manager.open_file(REFERRAL_ROUTING_TABLE, File::FILE_TYPE_OUTPUT);

	file_manager.open_file(REPORT_DATA_OLD, File::FILE_TYPE_OUTPUT, true);
	file_manager.files[REPORT_DATA_OLD]->file << "=========================";
	file_manager.files[REPORT_DATA_OLD]->file << file_manager.files[REPORT_DATA]->file.rdbuf();
	file_manager.close_file(REPORT_DATA_OLD);

	std::wstring date = file_manager.config_file.values[L"REPORT_DATE"];
	std::wstring english_date = file_manager.config_file.values[L"ENGLISH_DATE"];
	Reminder report_reminder(file_manager.config_file.values[L"REPORT_REMINDER"]);
	Reminder english_reminder(file_manager.config_file.values[L"ENGLISH_REMINDER"]);
	english_reminder.english = true;
	
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
	Terminal terminal(date, english_date, &modem, &report_collection.report_by_comp, &report_collection_english.report_by_comp, &comp_list, file_manager.files[OUTPUT]);
	terminal.set_mode(Terminal::MODE_INACTIVE);
	terminal.add_reminder(report_reminder);
	terminal.add_reminder(english_reminder);

	//close input files
	file_manager.close_file(REPORT_DATA);
	file_manager.close_file(ENGLISH_DATA);
	file_manager.close_file(REFERRAL_ROUTING_TABLE);

	// process string

	

	bool quit = false;
	
	while (!quit)
	{

		
		std::cout << "1. Start\n2. Report status\n3. English report status\n4. Terminal\n5. Save\n6. Quit" << std::endl;
		char input_choice;
		std::cin >> input_choice;
		if (input_choice == '1')
			terminal.set_mode(Terminal::MODE_AUTOMATIC);
		else if (input_choice == '2')
			show_report_status(&report_collection, &comp_list, date, false);
		else if (input_choice == '3')
			show_report_status(&report_collection_english, &comp_list, english_date, true);
		else if (input_choice == '4')
			terminal.set_mode(Terminal::MODE_USER_INPUT);
		else if (input_choice == '5')
			save(&file_manager, &report_collection, &report_collection_english, &comp_list, date, english_date);
		else if (input_choice == '6')
			quit = true;

		// basic terminal loop:

		clock_t start = clock();
		clock_t end = start;
		while (!quit && terminal.get_mode() != Terminal::MODE_INACTIVE)
		{
			terminal.update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f);
			start = end;
			end = clock();
		}
	}

	//save

	save(&file_manager, &report_collection, &report_collection_english, &comp_list, date, english_date);
	
	//close output files
	file_manager.close_file(OUTPUT);
	
	
	return 0;
}