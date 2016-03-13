#pragma once

class Gui;

#include "Modem.h"

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

	static const int NO_RESPONSE_TIMEOUT_MS = 5000;

	Modem modem;
	File* output_file;
	Gui* gui;

	std::wstring cur_command;
	std::wstring modem_reply;
	bool got_modem = true;
	bool timeout = false;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_until_timeout;			//time to wait before sending data if no response

	Terminal();
	virtual ~Terminal();

	void init(File* output_file_in, Gui* gui_in);
	bool run_command(std::wstring command);
	bool update(double millis);
	bool isbusy();

	void run();
};

