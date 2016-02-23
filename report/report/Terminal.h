#pragma once

class Modem;
class ReportCollection;
class CompList;
class Message;
class File;
class Reminder;
class MessageHandler;
class FileManager;

#include <sstream>
#include <vector>
#include <queue>
#include <Windows.h>

#define COMMAND_NEWLINE_CHAR L"\r"
#define COMMAND_ESCAPE_CHAR L";"

class Terminal
{
public:

	enum CommandSource
	{
		COMMAND_SOURCE_USER,
		COMMAND_SOURCE_LOGIC
	};

	static const int RESPONSE_TIMEOUT_MS = 2500;
	static const int NO_RESPONSE_TIMEOUT_MS = 20000;
	

	CommandSource cmd_source;

	Modem* modem;
	ReportCollection* report_collection;
	CompList* comp_list;
	File* output_file;
	MessageHandler* msg_handler;
	FileManager* file_manager;
	std::wstring date;					//current date string for saving regular reports
	std::wstring english_date;			//current date string for saving English reports
	std::vector<Reminder> reminders;

	std::queue<wchar_t> command_stream;

	
	bool got_user = false;

	wchar_t command_ch = 0;				//command stream input
	bool got_command = false;

	wchar_t modem_ch;						//character received
	std::wstring modem_str;			//string of consecutive chars received
	bool got_modem = true;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_to_wait;					//time to wait before sending data after response, in ms
	double ms_until_timeout;			//time to wait before sending data if no response
	time_t cur_time;

	Terminal(std::wstring date_in, std::wstring english_date_in, Modem* modem_in, ReportCollection* report_collection_in, CompList* comp_list_in, MessageHandler* message_handler_in, File* output_file_in, FileManager* file_manager_in);
	virtual ~Terminal();

	void init_auto();
	void init_user();
	void send_message(std::wstring dest_number, std::wstring message_contents);
	void delete_message_from_sim(int msg_cmg_id);
	void add_reminder(Reminder reminder);
	bool send_reminders();
	void push_command(std::wstring cmd);
	bool update(double millis);		//returns false when it wants to quit
};

