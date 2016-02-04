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

void show_report_status(ReportCollection* report_collection, CompList* comp_list, std::string date, bool english = false)
{
	std::cout << "Received\t\tNot received\n=============================================" << std::endl;
	for (std::map<std::string, Area>::iterator it = comp_list->areas.begin(); it != comp_list->areas.end(); ++it)
	{
		std::string id_str = date + ":" + it->second.area_name;
		if (!english || it->second.english_unit_name != "")
		{
			if (report_collection->report_by_comp.reports.count(id_str) > 0)
			{
				std::cout << it->second.area_name << std::endl;
			}
			else
			{
				std::cout << "\t\t\t" << it->second.area_name << std::endl;
			}
		}
	}
}

int main(int argc, char **argv)
{
	FileManager file_manager("paths.txt");
	file_manager.open_file("OUTPUT", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("PH_LIST", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_INPUT);
	file_manager.open_file("ENGLISH_DATA", File::FILE_TYPE_INPUT);

	file_manager.open_file("REPORT_DATA_OLD", File::FILE_TYPE_OUTPUT);
	file_manager.files["REPORT_DATA_OLD"]->file << file_manager.files["REPORT_DATA"]->file.rdbuf();
	file_manager.close_file("REPORT_DATA_OLD");

	std::string date = file_manager.config_file.values["REPORT_DATE"];
	std::string english_date = file_manager.config_file.values["ENGLISH_DATE"];
	Reminder report_reminder(file_manager.config_file.values["REPORT_REMINDER"]);
	Reminder english_reminder(file_manager.config_file.values["ENGLISH_REMINDER"]);
	english_reminder.english = true;
	
	ReportCollection report_collection;
	ReportCollection report_collection_english;
	report_collection_english.report_by_comp.report_type = Report::TYPE_ENGLISH;
	report_collection_english.report_by_zone.report_type = Report::TYPE_ENGLISH;
	CompList comp_list;

	report_collection.read_report_by_comp(file_manager.files["REPORT_DATA"]);
	report_collection_english.read_report_by_comp(file_manager.files["ENGLISH_DATA"]);
	comp_list.load(file_manager.files["PH_LIST"]->file);

	Modem modem;
	std::stringstream command;
	Terminal terminal(date, english_date, &modem, &report_collection.report_by_comp, &report_collection_english.report_by_comp, &comp_list, file_manager.files["OUTPUT"]);
	terminal.set_mode(Terminal::MODE_INACTIVE);
	terminal.add_reminder(report_reminder);
	terminal.add_reminder(english_reminder);

	//close input files
	file_manager.close_file("REPORT_DATA");
	file_manager.close_file("ENGLISH_DATA");

	// process string

	

	bool quit = false;
	
	while (!quit)
	{

		
		std::cout << "\n1. Start\n2. Report status\n3. English report status\t4. Terminal\t5. Quit" << std::endl;
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

	//copy output file for safety


	//open output files
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_OUTPUT);

	file_manager.open_file("ENGLISH_DATA", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("ENGLISH_DATA_BY_UNIT", File::FILE_TYPE_OUTPUT);
	
	
	report_collection.write_report_by_comp(file_manager.files["REPORT_DATA"]);
	report_collection.calculate_report_by_zone(&comp_list, date);
	report_collection.write_report_by_zone(file_manager.files["REPORT_DATA_BY_ZONE"]);

	report_collection_english.write_report_by_comp(file_manager.files["ENGLISH_DATA"]);
	report_collection_english.calculate_report_by_zone(&comp_list, english_date, true);
	report_collection_english.write_report_by_zone(file_manager.files["ENGLISH_DATA_BY_UNIT"]);
	
	
	//close output files
	file_manager.close_file("OUTPUT");
	file_manager.close_file("REPORT_DATA");
	file_manager.close_file("REPORT_DATA_BY_ZONE");
	file_manager.close_file("ENGLISH_DATA");
	file_manager.close_file("ENGLISH_DATA_BY_UNIT");
	
	return 0;
}