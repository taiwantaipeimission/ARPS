#pragma once

#include "codes.h"

#include <queue>
#include <string>
#include <mutex>

class ModemData
{
	std::mutex mu;
	std::queue<std::wstring> command_stream;		//To send to the modem
	std::queue<std::wstring> modem_str;					//string of consecutive chars received from the modem

public:

	ModemData();
	virtual ~ModemData();

	void push_command(std::wstring cmd, std::wstring terminator = COMMAND_NEWLINE_CHAR);
	std::wstring pop_command_str();
	size_t get_command_stream_size();
	void clear_command_stream();
	
	void push_modem_str(std::wstring modem_chars);
	std::queue<std::wstring> get_modem_strs();
	std::wstring pop_modem_str();
	void clear_modem_strs();
};