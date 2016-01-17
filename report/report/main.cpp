#include <string.h>
#include <iostream>



#include <fstream>
#include <string>
#include <sstream>

#include "Modem.h"
#include "ReportRegular.h"
#include "ReportSheet.h"
#include "CompList.h"
#include "Terminal.h"

int main(int argc, char **argv)
{
	

	Modem modem;

	std::ifstream path_file;
	std::ifstream input_file;
	std::ifstream phone_list_file;

	std::fstream output_file;

	std::ifstream report_data_in;
	std::ofstream report_data_out;

	//Read file contents
	std::string commands_path = "";
	std::string raw_output_path = "";
	std::string ph_list_path = "";
	std::string report_data_path = "";

	path_file.open("paths.txt");
	if (path_file.good())
	{
		std::string path_file_contents(static_cast<std::stringstream const&>(std::stringstream() << path_file.rdbuf()).str());
		int commands_beg_pos = path_file_contents.find("\"", path_file_contents.find("COMMANDS"));
		int commands_end_pos = path_file_contents.find("\"", commands_beg_pos + 1);
		commands_path = path_file_contents.substr(commands_beg_pos + 1, commands_end_pos - commands_beg_pos - 1);

		int raw_output_beg_pos = path_file_contents.find("\"", path_file_contents.find("RAW_OUTPUT"));
		int raw_output_end_pos = path_file_contents.find("\"", raw_output_beg_pos + 1);
		raw_output_path = path_file_contents.substr(raw_output_beg_pos + 1, raw_output_end_pos - raw_output_beg_pos - 1);

		int ph_list_beg_pos = path_file_contents.find("\"", path_file_contents.find("PH_LIST"));
		int ph_list_end_pos = path_file_contents.find("\"", ph_list_beg_pos + 1);
		ph_list_path = path_file_contents.substr(ph_list_beg_pos + 1, ph_list_end_pos - ph_list_beg_pos - 1);

		int report_data_beg_pos = path_file_contents.find("\"", path_file_contents.find("REPORT_DATA"));
		int report_data_end_pos = path_file_contents.find("\"", report_data_beg_pos + 1);
		report_data_path = path_file_contents.substr(report_data_beg_pos + 1, report_data_end_pos - report_data_beg_pos - 1);
	}
	output_file.open(raw_output_path, std::fstream::trunc | std::ios_base::in | std::ios_base::out);
	report_data_in.open(report_data_path, std::ios_base::in);	// std::ios_base::app
	report_data_out.open(report_data_path + ".new", std::ios_base::out);

	input_file.open(commands_path);
	phone_list_file.open(ph_list_path);

	std::string date = "2016:1:2:7";

	ReportSheet report_sheet;
	CompList comp_list;

	report_sheet.read_stored_all(report_data_in);
	comp_list.load(phone_list_file);
	// process string

	std::stringstream command;

	Terminal terminal(date, &modem, &report_sheet, &comp_list, &output_file);

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
			terminal.set_mode(Terminal::MODE_READ_MSG);
		else if (input_choice == '2')
			terminal.set_mode(Terminal::MODE_USER_INPUT);
		else if (input_choice == '3')
			quit = true;

		// basic terminal loop:

		while (!quit && terminal.get_mode() != Terminal::MODE_MENU)
		{
			terminal.update(1);
			Sleep(1);
		}
	}
	
	report_data_out.clear();
	report_sheet.print(report_data_out);

	// close up and go home.
	output_file.close();
	report_data_in.close();
	report_data_out.close();
	input_file.close();
	phone_list_file.close();

	std::remove(report_data_path.c_str());
	std::rename((report_data_path + ".new").c_str(), report_data_path.c_str());

	return 0;
}