/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

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


#include "rapidjson\stringbuffer.h"
#include "rapidjson\prettywriter.h"

#include <string.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Multiline_Output.H>

using namespace rapidjson;

Gui* gui;

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
	while (!*quit || mod_interface->num_commands() > 0 || terminal->isbusy())
	{
		if (!terminal->isbusy() && mod_interface->num_commands() > 0)
		{
			bool wake_gui = false;
			Command cmd = mod_interface->pop_command();
			terminal->run_command(&cmd);
			if (cmd.success_all)
			{
				mod_interface->push_result(cmd);
				wake_gui = true;
			}
			else if (cmd.n_times_tried < cmd.n_times_to_try)
			{
				mod_interface->push_command(cmd);
			}
			else
			{
				mod_interface->push_result(cmd);
				wake_gui = true;
			}
			if(wake_gui)
				Fl::awake(completed_command_cb, (void*)gui);
		}
	}
}

BOOL WINAPI ConsoleHandler(DWORD dwCtrlEvent)
{
	switch (dwCtrlEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	default:
		return FALSE;
	}
}

int main(int argc, char **argv)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);

	set_color(CC_GREEN, CC_BLACK);
	wcout << L"Loading...\n";
	set_color(CC_GREY, CC_BLACK);

	wcout << L"-Creating objects\n";
	gui = new Gui();
	Terminal terminal;
	ModemInterface modem_interface;
	bool quit = false;

	wcout << L"-Initializing terminal\n";
	terminal.init(&gui->file_manager.files[L"OUTPUT"], gui);
	std::thread terminal_thread(run_terminal_func, &terminal, gui, &modem_interface, &quit);

	wcout << L"-Loading data files\n";
	gui->load();
	wcout << L"-Initializing GUI\n";
	gui->init(&modem_interface);
	set_color(CC_GREEN, CC_BLACK);
	wcout << L"Done\n";
	set_color(CC_WHITE, CC_BLACK);
	gui->run();
		
	quit = true;
	terminal_thread.join();
	delete gui;

	return 0;
}