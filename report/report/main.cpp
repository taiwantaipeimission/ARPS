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
#include "CompList.h"

enum TerminalMode
{
	MODE_COMMAND_ECHO = 0,
	MODE_USER_INPUT,
	MODE_MENU,
	MODE_QUIT
};

static int TIMEOUT_MS = 20;
static char COMMAND_END_CHAR = '~';
static char COMMAND_ESCAPE_CHAR = ';';
static char COMMAND_NEWLINE_CHAR = '\n';

int main(int argc, char **argv)
{
	TerminalMode mode = MODE_MENU;

	Modem modem;

	std::fstream output_file;
	std::fstream processed_output_file;
	std::ifstream input_file;
	std::ifstream config_file;
	std::ifstream phone_list_file;

	output_file.open("output.txt");
	processed_output_file.open("processed_output.txt", std::ios_base::in | std::ios_base::out);	// std::ios_base::app
	processed_output_file.clear();
	input_file.open("input.txt");
	phone_list_file.open("phone_list.txt");

	std::string text_data;

	std::string date = "2016:1:1:7";

	ReportSheet report_sheet;
	CompList comp_list;

	report_sheet.read_stored_all(processed_output_file);
	comp_list.load(phone_list_file);
	// process string
	
	while (mode != MODE_QUIT)
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
		std::cout << "\n1. READ MSGS\t2. RUN AT TERMINAL\t3. QUIT" << std::endl;
		char input_choice;
		std::cin >> input_choice;
		if (input_choice == '1')
			mode = MODE_COMMAND_ECHO;
		else if (input_choice == '2')
			mode = MODE_USER_INPUT;
		else if (input_choice == '3')
			mode = MODE_QUIT;

		// basic terminal loop:
		if (mode == MODE_COMMAND_ECHO || mode == MODE_USER_INPUT)
		{
			char ch;
			char buffer[2] = " ";
			DWORD read, written;
			bool received_response = true;
			bool entered_newline = false;
			int wait_ms = 0;
			int elapsed_ms = 0;
			input_file.clear();
			input_file.seekg(0, std::ios::beg);
			do
			{
				ReadFile(modem.file, buffer, 1, &read, NULL);
				if (read)
				{
					std::cout << buffer;
					output_file << buffer;
					text_data += buffer;
					received_response = true;
					if (wait_ms > 0)
						wait_ms = TIMEOUT_MS;
				}

				if (mode == MODE_COMMAND_ECHO)
				{
					if (received_response && wait_ms <= 0 && ch != COMMAND_END_CHAR)
					{
						//ch = commands[command_index];
						input_file.get(ch);
						if (ch == COMMAND_NEWLINE_CHAR)
						{
							WriteFile(modem.file, "\r\n", 3, &written, NULL);
							wait_ms = TIMEOUT_MS;
						}
						else if (ch == COMMAND_ESCAPE_CHAR)
						{
							WriteFile(modem.file, "\u001A", 1, &written, NULL);
							wait_ms = 0;
						}
						else if (ch != COMMAND_END_CHAR)
						{
							WriteFile(modem.file, &ch, 1, &written, NULL);
							wait_ms = 0;
						}
						received_response = false;
					}
					wait_ms--;
				}
				else if (mode == MODE_USER_INPUT)
				{
					if (_kbhit())
					{
						ch = _getch();
						if (ch != 27)
							WriteFile(modem.file, &ch, 1, &written, NULL);
						else
							mode = MODE_MENU;
					}
				}
				Sleep(1);
			} while (mode == MODE_USER_INPUT || (mode == MODE_COMMAND_ECHO && ch != '~'));

			if (mode == MODE_COMMAND_ECHO)
			{
				output_file.clear();
				output_file.seekg(0, std::ios::beg);
				
				report_sheet.read_unprocessed(output_file, date, &comp_list);
				
			}
		}
	}

	processed_output_file.clear();
	processed_output_file.seekg(0, std::ios::beg);
	report_sheet.print(processed_output_file);

	// close up and go home.
	output_file.close();
	processed_output_file.close();
	input_file.close();
	phone_list_file.close();

	return 0;
}