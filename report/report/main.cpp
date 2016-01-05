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

enum TerminalMode
{
	MODE_COMMAND_ECHO = 0,
	MODE_USER_INPUT
};

int main(int argc, char **argv)
{
	TerminalMode mode = MODE_COMMAND_ECHO;

	Modem modem;

	//HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);

	std::ofstream output_file;
	std::ofstream processed_output_file;
	std::ifstream input_file;
	std::ifstream config_file;

	output_file.open("output.txt");
	processed_output_file.open("processed_output.txt");
	input_file.open("input.txt");
	config_file.open("config.txt");
	
	char config_data[1000];
	char commands[1000];

	std::string text_data;

	if(config_file.is_open())
	{
		config_file.getline(config_data, 1000, '\0');
	}
	if (input_file.is_open())
	{
		input_file.getline(commands, 1000, '\0');
	}

	//char commands[] = "AT\nAT+CMGS=\"+886910358944\"\nTHIS IS A MESSAGE.\u001A";

	// basic terminal loop:

	{
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

			if(mode == MODE_COMMAND_ECHO)
			{
				if (received_response && idle_ms > 10 && ch != '\0')
				{
					ch = commands[command_index];
					if (ch == '\n')
						WriteFile(modem.file, "\r\n", 3, &written, NULL);
					else if (ch == ';')
						WriteFile(modem.file, "\u001A", 1, &written, NULL);
					else
						WriteFile(modem.file, &ch, 1, &written, NULL);
					command_index++;
					received_response = false;
				}
			}
			else if(mode == MODE_USER_INPUT)
			{
				if (_kbhit())
				{
					ch = _getch();
					WriteFile(modem.file, &ch, 1, &written, NULL);
				}
			}
			Sleep(1);
		} while (mode == MODE_USER_INPUT || ch != '\0');
	}

	if (mode == MODE_COMMAND_ECHO)
	{
		// process string

		int msg_start_pos = text_data.find("TYPE", 0);
		int msg_end_pos = text_data.find("TYPE", msg_start_pos + 1);
		std::string extracted_msg;
		while (msg_start_pos != std::string::npos)
		{

			ReportRegular report;
			for (unsigned int i = 0; i < report.key_list.size(); i++)
			{
				std::string key = report.key_list[i];
				std::string value;

				int value_start_pos = text_data.find(key + ":", msg_start_pos) + key.length() + 1;
				int value_end_pos = text_data.find("\n", value_start_pos + 1);

				value = text_data.substr(value_start_pos, value_end_pos - value_start_pos);
				report.add_field(key, value);
				std::cout << key << "\t" << value << std::endl;
				processed_output_file << key << "\t" << value << std::endl;
			}

			msg_start_pos = msg_end_pos;
			msg_end_pos = text_data.find("TYPE", msg_start_pos + 1);
		}
	}

	/*]
	do {
		// check for data on port and display it on screen.
		ReadFile(modem.file, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (_kbhit()) {
			ch = _getch();
			WriteFile(modem.file, &ch, 1, &written, NULL);
			WriteFile(screen, &ch, 1, &written, NULL);
		}
		// until user hits escape.
	} while (ch != 27);*/

	// close up and go home.
	output_file.close();
	processed_output_file.close();
	input_file.close();
	config_file.close();

	return 0;
}