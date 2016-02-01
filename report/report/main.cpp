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

std::string prompt_date()
{
	std::string date;
	std::string year;
	std::string month;
	std::string week;
	std::string day;
	std::cout << "Year:";
	std::cin >> year;
	std::cout << "Month:";
	std::cin >> month;
	std::cout << "Week:";
	std::cin >> week;
	std::cout << "Day:";
	std::cin >> day;
	date = year + ":" + month + ":" + week + ":" + day;
	return date;
}

int main(int argc, char **argv)
{
	

	Modem modem;

	FileManager file_manager("paths.txt");

	file_manager.open_file("COMMANDS", File::FILE_TYPE_INPUT);
	file_manager.open_file("PH_LIST", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_MISS", File::FILE_TYPE_INPUT);

	file_manager.open_file("REPORT_DATA_OLD", File::FILE_TYPE_OUTPUT);
	file_manager.files["REPORT_DATA_OLD"]->file << file_manager.files["REPORT_DATA"]->file.rdbuf();
	file_manager.close_file("REPORT_DATA_OLD");

	std::string date = prompt_date();

	ReportCollection report_collection;
	CompList comp_list;

	report_collection.read_report_by_comp(file_manager.files["REPORT_DATA"]);
	comp_list.load(file_manager.files["PH_LIST"]->file);

	//close input files
	file_manager.close_file("REPORT_DATA");
	file_manager.close_file("REPORT_DATA_BY_ZONE");
	file_manager.close_file("REPORT_DATA_BY_MISS");

	// process string

	std::stringstream command;

	Terminal terminal(date, &modem, &report_collection.report_by_comp, NULL, &comp_list);
	terminal.set_mode(Terminal::MODE_INACTIVE);

	bool quit = false;
	
	while (!quit)
	{

		std::cout << "Received\t\tNot received\n=============================================" << std::endl;
		for (std::map<std::string, Area>::iterator it = comp_list.areas.begin(); it != comp_list.areas.end(); ++it)
		{
			std::string id_str = date + ":" + it->second.area_name;
			if (report_collection.report_by_comp.reports.count(id_str) > 0)
			{
				std::cout << it->second.area_name << std::endl;
			}
			else
			{
				std::cout << "\t\t\t" << it->second.area_name << std::endl;
			}
		}
		std::cout << "\n1. Start\t2. Terminal\t3. Reporting period\t4. Reminders \t5. Quit" << std::endl;
		char input_choice;
		std::cin >> input_choice;
		if (input_choice == '1')
			terminal.set_mode(Terminal::MODE_AUTOMATIC);
		else if (input_choice == '2')
			terminal.set_mode(Terminal::MODE_USER_INPUT);
		else if (input_choice == '3')
		{
			date = prompt_date();
		}
		else if (input_choice == '4')
		{
			std::cout << "Current reminders:" << std::endl;
			for (int i = 0; i < terminal.reminders.size(); i++)
			{
				std::cout << i << "-" << terminal.reminders[i] << std::endl;
			}

			char input_choice_1;

			std::cout << "1. Add reminder\t2. Delete reminder\t3. Return" << std::endl;
			std::cin >> input_choice;

			if (input_choice == '1')
			{
				time_t cur_time;
				std::time(&cur_time);
				std::tm reminder;
				localtime_s(&reminder, &cur_time);

				std::cout << "Enter day of month:" << std::endl;
				std::cin >> reminder.tm_mday;
				std::cout << "Enter hours:" << std::endl;
				std::cin >> reminder.tm_hour;
				std::cout << "Enter minutes:" << std::endl;
				std::cin >> reminder.tm_min;

				terminal.add_reminder(&reminder);
			}
			else if (input_choice == '2')
			{
				int index;
				std::cout << "Enter index to delete, or -1 to quit" << std::endl;
				std::cin >> index;
				if (index >= 0 && index < terminal.reminders.size())
				{
					terminal.reminders.erase(terminal.reminders.begin() + index);
					std::cout << "Reminder " << index << " deleted" << std::endl;
				}
			}
		}
		else if (input_choice == '5')
			quit = true;

		// basic terminal loop:

		while (!quit && terminal.get_mode() != Terminal::MODE_INACTIVE)
		{
			terminal.update(1);
			Sleep(1);
		}
	}

	//copy output file for safety


	//open output files
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("REPORT_DATA_BY_MISS", File::FILE_TYPE_OUTPUT);

	
	
	report_collection.write_report_by_comp(file_manager.files["REPORT_DATA"]);
	report_collection.write_report_by_indiv(file_manager.files["REPORT_DATA_BY_MISS"]);


	report_collection.calculate_report_by_zone(&comp_list, date);
	report_collection.write_report_by_zone(file_manager.files["REPORT_DATA_BY_ZONE"]);
	
	
	//close output files
	file_manager.close_file("REPORT_DATA");
	file_manager.close_file("REPORT_DATA_BY_ZONE");
	file_manager.close_file("REPORT_DATA_BY_MISS");
	
	return 0;
}