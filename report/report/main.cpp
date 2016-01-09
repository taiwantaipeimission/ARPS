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

static int TIMEOUT_MS = 100;
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
	processed_output_file.open("processed_output.txt", std::ios_base::out | std::ios_base::in | std::ios_base::app);
	input_file.open("input.txt");
	phone_list_file.open("phone_list.txt");

	std::string text_data;

	std::string year = "2016";
	std::string month = "1";
	std::string week = "1";
	std::string day = "7";

	ReportSheet report_sheet(year, month, week, day);
	CompList comp_list;

	report_sheet.read_processed(processed_output_file);
	comp_list.load(phone_list_file);
	// process string
	
	while (mode != MODE_QUIT)
	{

		std::cout << "RECEIVED\t\tNOT RECEIVED\n=============================================" << std::endl;
		for (std::map<std::string, std::string>::iterator it = comp_list.phone_name.begin(); it != comp_list.phone_name.end(); ++it)
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
			int elapsed_ms = 0;
			int wait_ms = 0;
			do
			{
				ReadFile(modem.file, buffer, 1, &read, NULL);
				if (read)
				{
					std::cout << buffer;
					output_file << buffer;
					text_data += buffer;
					received_response = true;
				}

				if (mode == MODE_COMMAND_ECHO)
				{
					if (received_response && elapsed_ms >= wait_ms && ch != COMMAND_END_CHAR)
					{
						//ch = commands[command_index];
						input_file.get(ch);
						if (ch == COMMAND_NEWLINE_CHAR)
						{
							WriteFile(modem.file, "\r\n", 3, &written, NULL);
							wait_ms = TIMEOUT_MS;
							elapsed_ms = 0;
						}
						else if (ch == COMMAND_ESCAPE_CHAR)
							WriteFile(modem.file, "\u001A", 1, &written, NULL);
						else if (ch != COMMAND_END_CHAR)
							WriteFile(modem.file, &ch, 1, &written, NULL);
						received_response = false;
					}
					elapsed_ms++;
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


				std::ifstream raw_text_contents;
				std::ofstream processed_output;

				raw_text_contents.open("output.txt");
				processed_output.open("processed_output.txt");



				report_sheet.read_unprocessed(raw_text_contents, &comp_list);

				report_sheet.print(processed_output);

				raw_text_contents.close();
				processed_output.close();
			}
		}
	}

	// close up and go home.
	output_file.close();
	processed_output_file.close();
	input_file.close();
	phone_list_file.close();

	return 0;
}