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

	enum CommandSource
	{
		COMMAND_SOURCE_USER,
		COMMAND_SOURCE_LOGIC
	};

	static const int RESPONSE_TIMEOUT_MS = 2500;
	static const int NO_RESPONSE_TIMEOUT_MS = 25000;
	

	CommandSource cmd_source;

	Modem modem;
	ModemData* modem_data;
	File* output_file;
	Gui* gui;

	std::wstring modem_reply;

	bool got_user = false;
	bool got_command = false;

	bool got_modem = true;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_until_timeout;			//time to wait before sending data if no response

	Terminal();
	virtual ~Terminal();

	void init(ModemData* modem_data_in, File* output_file_in, Gui* gui_in);
	
	bool update(double millis);
	void run();
};

