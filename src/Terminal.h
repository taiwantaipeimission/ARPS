#pragma once

class Gui;

#include "Modem.h"
#include "ModemInterface.h"
#include "ModemData.h"
#include "File.h"

#include <sstream>
#include <vector>
#include <queue>
#include <Windows.h>

class Terminal
{
public:

	std::mutex mu;

	enum CommandResult
	{
		RESULT_SUCCESS,
		RESULT_ERROR
	};

	Modem modem;
	File* output_file;
	Gui* gui;

	wstring cur_command;
	wstring cur_reply;
	bool got_modem = true;
	bool timeout = false;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_until_timeout;			//time to wait before sending data if no response

	Terminal();
	virtual ~Terminal();

	void init(File* output_file_in, Gui* gui_in);
	wstring run_command_str(wstring command);
	void run_command(Command* command);
	bool update(double millis);
	bool isbusy();
};

