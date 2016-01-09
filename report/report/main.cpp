#include <string.h>
#include <iostream>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <string>

#include "Modem.h"
#include "ReportRegular.h"
#include "ReportSheet.h"

enum TerminalMode
{
	MODE_COMMAND_ECHO = 0,
	MODE_USER_INPUT
};

int main(int argc, char **argv)
{
	TerminalMode mode = MODE_COMMAND_ECHO;

	Modem modem;

	std::fstream output_file;
	std::fstream processed_output_file;
	std::ifstream input_file;
	std::ifstream config_file;
	std::ifstream phone_list_file;

	output_file.open("output.txt");
	processed_output_file.open("processed_output.txt", std::ios_base::out | std::ios_base::in | std::ios_base::app);
	input_file.open("input.txt");
	config_file.open("config.txt");
	phone_list_file.open("phone_list.txt");
	
	char config_data[1000];
	char commands[1000];

	std::string text_data;

	std::string year = "2016";
	std::string month = "1";
	std::string week = "1";
	std::string day = "7";

	ReportSheet report_sheet("2016", "1", "2", "7");

	if(config_file.is_open())
	{
		config_file.getline(config_data, 1000, '\0');
	}
	if (input_file.is_open())
	{
		input_file.getline(commands, 1000, '\0');
	}

	report_sheet.read_processed(processed_output_file);

	// process string
	std::map<std::string, std::string> comp_list;
	char all_text[1000] = "";
	char ph_number[64] = "";
	char comp_name[64] = "";

	phone_list_file.getline(ph_number, 64, '\t');
	phone_list_file.getline(comp_name, 64, '\n');

	while (phone_list_file.good() )
	{
	comp_list[ph_number] = comp_name;

	phone_list_file.getline(ph_number, 64, '\t');
	phone_list_file.getline(comp_name, 64, '\n');
	}


	std::cout << "RECEIVED\t\tNOT RECEIVED" << std::endl;
	for (std::map<std::string, std::string>::iterator it = comp_list.begin(); it != comp_list.end(); ++it)
	{
		if (report_sheet.reports.count(it->second) > 0)
		{
			std::cout << it->second << std::endl;
		}
		else
		{
			std::cout << "\t\t\t" << it->second << std::endl;
		}
	}
	std::cout << "\n1. READ MSGS\t2. RUN AT TERMINAL\t3. QUIT" << std::endl;
	char input_choice;
	std::cin >> input_choice;

	// basic terminal loop:
	if (input_choice == '1' || input_choice == '2')
	{
		mode = input_choice == '1' ? MODE_COMMAND_ECHO : MODE_USER_INPUT;
		char ch;
		char buffer[2] = " ";
		int command_index = 0;
		DWORD read, written;
		bool received_response = true;
		int idle_ms = 0;
		do
		{
			ReadFile(modem.file, buffer, 1, &read, NULL);
			if (read)
			{
				std::cout << buffer;
				output_file << buffer;
				text_data += buffer;
				received_response = true;
				idle_ms = 0;
			}
			else
			{
				idle_ms += 1;
			}

			if (mode == MODE_COMMAND_ECHO)
			{
				if (received_response && idle_ms > 10 && ch != '\0')
				{
					ch = commands[command_index];
					if (ch == '\n')
						WriteFile(modem.file, "\r\n", 3, &written, NULL);
					else if (ch == ';')
						WriteFile(modem.file, "\u001A", 1, &written, NULL);
					else if (ch != '~')
						WriteFile(modem.file, &ch, 1, &written, NULL);
					command_index++;
					received_response = false;
				}
			}
			else if (mode == MODE_USER_INPUT)
			{
				if (_kbhit())
				{
					ch = _getch();
					WriteFile(modem.file, &ch, 1, &written, NULL);
				}
			}
			Sleep(1);
		} while (mode == MODE_USER_INPUT || ch != '~');

		if (mode == MODE_COMMAND_ECHO)
		{
			

			std::ifstream raw_text_contents;
			std::ofstream processed_output;

			raw_text_contents.open("output.txt");
			processed_output.open("processed_output.txt");

			

			report_sheet.read_unprocessed(raw_text_contents);

			report_sheet.print(processed_output);

			raw_text_contents.close();
			processed_output.close();
		}
	}

	// close up and go home.
	output_file.close();
	processed_output_file.close();
	input_file.close();
	config_file.close();
	phone_list_file.close();

	return 0;
}