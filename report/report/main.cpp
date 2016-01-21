#include <string.h>
#include <iostream>



#include <fstream>
#include <string>
#include <sstream>

#include "Modem.h"
#include "FileManager.h"
#include "ReportRegular.h"
#include "ReportSheet.h"
#include "CompList.h"
#include "Terminal.h"

int main(int argc, char **argv)
{
	

	Modem modem;

	FileManager file_manager("paths.txt");

	file_manager.open_file("COMMANDS", File::FILE_TYPE_INPUT);
	file_manager.open_file("RAW_OUTPUT", File::FILE_TYPE_OUTPUT);
	file_manager.open_file("PH_LIST", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_ZONE", File::FILE_TYPE_INPUT);
	file_manager.open_file("REPORT_DATA_BY_MISS", File::FILE_TYPE_INPUT);

	std::string date = "2016:1:2:7";

	ReportSheet report_sheet;
	CompList comp_list;

	report_sheet.read_stored_all(file_manager.files["REPORT_DATA"]->file);
	comp_list.load(file_manager.files["PH_LIST"]->file);
	// process string

	std::stringstream command;

	Terminal terminal(date, &modem, &report_sheet, &comp_list);

	bool quit = false;
	
	while (!quit)
	{

		std::cout << "RECEIVED\t\tNOT RECEIVED\n=============================================" << std::endl;
		for (std::map<std::string, std::string>::iterator it = comp_list.phone_name.begin(); it != comp_list.phone_name.end(); ++it)
		{
			std::string id_str = date + ":" + it->second;
			if (report_sheet.reports.count(id_str) > 0)
			{
				std::cout << it->second << std::endl;
			}
			else
			{
				std::cout << "\t\t\t" << it->second << std::endl;
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
	
	file_manager.close_file("REPORT_DATA");
	file_manager.open_file("REPORT_DATA", File::FILE_TYPE_OUTPUT);
	report_sheet.print(file_manager.files["REPORT_DATA"]->file);

	return 0;
}