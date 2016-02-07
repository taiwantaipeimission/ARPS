#pragma once


class Modem;
class ReportSheet;
class CompList;
class Message;
class File;
class Reminder;

#include <sstream>
#include <vector>

class Terminal
{
public:
	enum TerminalMode
	{
		MODE_USER_INPUT,
		MODE_AUTOMATIC,
		MODE_INACTIVE
	};

	static const int TIMEOUT_MS = 2500;
	static const int MSG_TIMEOUT_MS = 10000;
	static const wchar_t COMMAND_END_CHAR = EOF;
	static const wchar_t COMMAND_ESCAPE_CHAR = ';';
	static const wchar_t COMMAND_NEWLINE_CHAR = '\n';

	TerminalMode mode;

	Modem* modem;
	ReportSheet* report_sheet;
	ReportSheet* english_report_sheet;
	CompList* comp_list;
	File* output_file;
	std::wstring date;					//current date string for saving regular reports
	std::wstring english_date;			//current date string for saving English reports
	std::vector<Reminder> reminders;

	std::wstringstream command_stream;
	std::vector<Message> cur_messages;

	wchar_t user_ch = 0;					//user input
	bool got_user = false;

	wchar_t command_ch = 0;				//command stream input
	bool got_command = false;

	wchar_t modem_ch;						//character received
	std::wstring modem_str;			//string of consecutive chars received
	bool got_modem = false;

	DWORD read, written;				//number of bytes read/written to modem
	double ms_to_wait;			//time to wait before sending data, in ms
	time_t cur_time;

	Terminal(std::wstring date_in, std::wstring english_date_in, Modem* modem_in, ReportSheet* report_sheet_in, ReportSheet* english_report_sheet_in, CompList* comp_list_in, File* output_file_in);
	virtual ~Terminal();

	void parse_messages(std::wstring raw_str);
	void add_reminder(Reminder reminder);
	bool send_reminders();
	void set_mode(TerminalMode new_mode);
	TerminalMode get_mode();

	void update(double millis);
};

