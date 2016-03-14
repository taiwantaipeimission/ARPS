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
#include "ModemInterface.h"



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

void run_terminal_func(Terminal* terminal, Gui* gui, ModemInterface* mod_interface, bool* quit)
{
	while (!*quit || mod_interface->num_commands() > 0)
	{
		if (!terminal->isbusy() && mod_interface->num_commands() > 0)
		{
			wstring cmd = mod_interface->pop_command();
			wstring result = terminal->run_command(cmd);
			mod_interface->push_result(cmd, result);
			Fl::awake(check_msg_cb, (void*)gui);
		}
	}
}

int main(int argc, char **argv)
{
	try
	{
		Gui gui;
		Terminal terminal;
		ModemInterface modem_interface;
		bool quit = false;

		terminal.init(&gui.file_manager.files[L"OUTPUT"], &gui);
		std::thread terminal_thread(run_terminal_func, &terminal, &gui, &modem_interface, &quit);

		gui.load();
		gui.init(&modem_interface);
		gui.run();
		
		quit = true;
		terminal_thread.join();
	}
	catch(std::exception e)
	{
		std::wcout << L"Error: " << e.what() << L"\n";
		wchar_t c;
		std::wcin >> c;
	}
	return 0;
}