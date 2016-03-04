#pragma once
#ifndef MODEM_DATA_H
#define MODEM_DATA_H

#include <queue>
#include <string>
#include <mutex>

class ModemData
{
	std::mutex mu;
	std::queue<wchar_t> command_stream;		//To send to the modem
	std::queue<std::wstring> modem_str;					//string of consecutive chars received from the modem

public:

	ModemData();
	virtual ~ModemData();

	void push_command(std::wstring cmd);
	wchar_t pop_command_ch();
	size_t get_command_stream_size();
	void clear_command_stream();
	
	void push_modem_str(std::wstring modem_chars);
	std::queue<std::wstring> get_modem_strs();
	std::wstring pop_modem_str();
	void clear_modem_strs();
};

#endif