#include <string.h>
#include <iostream>



#include <fstream>
#include <string>
#include <sstream>

#include "Modem.h"
#include "FileManager.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Terminal.h"

int main(int argc, char **argv)
{
	

	Modem modem;

	FileManager file_manager("paths.txt");

	file_manager.open_file("COMMANDS", File::FILE_TYPE_INPUT);
	file_manager.open_file("PH_LIST", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_MISS", File::FILE_TYPE_INPUT);

	std::string date = "2016:1:2:7";

	ReportCollection report_collection;
	CompList comp_list;

	report_collection.read_report_by_comp(file_manager.files["REPORT_DATA"]);
	comp_list.load(file_manager.files["PH_LIST"]->file);
	// process string

	std::stringstream command;

	Terminal terminal(date, &modem, &report_collection.report_by_comp, &comp_list);

	bool quit = false;
	
	while (!quit)
	{

		std::cout << "RECEIVED\t\tNOT RECEIVED\n=============================================" << std::endl;
		for (std::map<std::string, std::pair<std::string, std::string>>::iterator it = comp_list.phone_name.begin(); it != comp_list.phone_name.end(); ++it)
		{
			std::string id_str = date + ":" + it->second.first;
			if (report_collection.report_by_comp.reports.count(id_str) > 0)
			{
				std::cout << it->second.first << std::endl;
			}
			else
			{
				std::cout << "\t\t\t" << it->second.first << std::endl;
			}
		}
		std::cout << "\n1. START\t2. RUN AT TERMINAL\t3. QUIT" << std::endl;
		char input_choice;
		std::cin >> input_choice;
		if (input_choice == '1')
			terminal.set_mode(Terminal::MODE_AUTOMATIC);
		else if (input_choice == '2')
			terminal.set_mode(Terminal::MODE_USER_INPUT);
		else if (input_choice == '3')
			quit = true;

		// basic terminal loop:

		while (!quit && terminal.get_mode() != Terminal::MODE_INACTIVE)
		{
			terminal.update(1);
			Sleep(1);
		}
	}

	//close files for input before re-opening for output
	file_manager.close_file("REPORT_DATA");
	file_manager.close_file("REPORT_DATA_BY_ZONE");
	file_manager.close_file("REPORT_DATA_BY_MISS");

	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("REPORT_DATA_BY_MISS", File::FILE_TYPE_OUTPUT);

	report_collection.calculate_report_by_zone(&comp_list, date);
	
	report_collection.write_report_by_comp(file_manager.files["REPORT_DATA"]);
	report_collection.write_report_by_zone(file_manager.files["REPORT_DATA_BY_ZONE"]);
	report_collection.write_report_by_indiv(file_manager.files["REPORT_DATA_BY_MISS"]);

	return 0;
}