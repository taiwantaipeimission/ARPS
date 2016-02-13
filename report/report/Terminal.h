#pragma once


class Modem;
class ReportCollection;
class CompList;
class Message;
class File;
class Reminder;

#include <sstream>
#include <vector>
#include <queue>

#define COMMAND_NEWLINE_CHAR L"\r"

class Terminal
{
public:

	enum CommandSource
	{
		COMMAND_SOURCE_USER,
		COMMAND_SOURCE_LOGIC
	};

	static const int TIMEOUT_MS = 2500;
	static const int MSG_TIMEOUT_MS = 20000;
	static const wchar_t COMMAND_ESCAPE_CHAR = ';';
	

	CommandSource cmd_source;

	Modem* modem;
	ReportCollection* report_collection;
	CompList* comp_list;
	File* output_file;
	std::wstring date;					//current date string for saving regular reports
	std::wstring english_date;			//current date string for saving English reports
	std::vector<Reminder> reminders;

	std::queue<wchar_t> command_stream;
	
	std::vector<Message> cur_messages;

	
	bool got_user = false;

	wchar_t command_ch = 0;				//command stream input
	bool got_command = false;

	wchar_t modem_ch;						//character received
	std::wstring modem_str;			//string of consecutive chars received
	bool got_modem = true;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_to_wait;					//time to wait before sending data, in ms
	time_t cur_time;

	Terminal(std::wstring date_in, std::wstring english_date_in, Modem* modem_in, ReportCollection* report_collection_in, CompList* comp_list_in, File* output_file_in);
	virtual ~Terminal();

	void init_auto();
	void init_user();
	void parse_messages(std::wstring raw_str);
	void process_messages();
	void send_message(std::wstring dest_number, std::wstring message_contents);
	void delete_message(int msg_cmg_id);
	void add_reminder(Reminder reminder);
	bool send_reminders();
	void push_command(std::wstring cmd);
	bool update(double millis);		//returns false when it wants to quit
};

