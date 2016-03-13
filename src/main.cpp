#define WIN32

#include "utility.h"

#include <string.h>
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>

#include <codecvt>


#include "codes.h"
#include "Gui.h"
#include "Terminal.h"



#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Multiline_Output.H>


/* Create the date stamp for a reporting period, based on the current time and the weekday of reporting.
 * All days up to the day of reporting return back to the previous reporting period (e.g. Thursday-Tuesday will be counted as reports
 * for English reporting session which began on the Wednesday previous).
 */


class Console_streambuf
	: public std::basic_streambuf<wchar_t>
{
	HANDLE m_out;
public:
	Console_streambuf(HANDLE out) : m_out(out) {}

	virtual int_type overflow(int_type c = traits_type::eof())
	{
		wchar_t wc = c;
		DWORD numberOfCharsWritten;
		BOOL res = WriteConsoleW(m_out, &wc, 1, &numberOfCharsWritten, NULL);
		(void)res;
		return 1;
	}
};

std::queue<std::wstring> commands;
bool quit = false;

void run_terminal_func(Terminal* terminal)
{
	while (!quit || commands.size() > 0)
	{
		if (!terminal->isbusy() && commands.size() > 0)
		{
			terminal->run_command(commands.front());
			commands.pop();
		}
	}
}

int main(int argc, char **argv)
{
	try
	{
		Gui gui;
		Terminal terminal;
		//ModemData modem_data;

		//gui.load();
		//gui.init(&modem_data);
		terminal.init(&gui.file_manager.files[L"OUTPUT"], &gui);

		std::thread terminal_thread(run_terminal_func, &terminal);
		std::wstring cmd;
		std::getline(std::wcin, cmd);
		while (cmd != L"q")
		{
			replace_chars(cmd, L";", COMMAND_ESCAPE_CHAR);
			commands.push(cmd + L"\r");
			std::getline(std::wcin, cmd);
		}
		quit = true;

		terminal_thread.join();

		//std::thread terminal_thread(run_terminal_func, &terminal);
		//gui.run();
		//terminal_thread.join();
	}
	catch(std::exception e)
	{
		std::wcout << L"Error: " << e.what() << L"\n";
		wchar_t c;
		std::wcin >> c;
	}
	return 0;
}