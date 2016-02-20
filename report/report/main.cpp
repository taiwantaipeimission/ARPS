#define WIN32

#include <string.h>
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <codecvt>
#include <FL/FL.h>
#include <FL/FL_Window.h>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.h>
#include <FL/Fl_Timer.H>
#include <FL/fl_ask.H>

#include "Modem.h"
#include "FileManager.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Terminal.h"
#include "Area.h"
#include "Reminder.h"
#include "Message.h"
#include "MessageHandler.h"

// File handles
#define OUTPUT L"OUTPUT"
#define PH_LIST L"PH_LIST"
#define MESSAGES_PROCESSED L"MESSAGES_PROCESSED"
#define MESSAGES_UNPROCESSED L"MESSAGES_UNPROCESSED"
#define REPORT_DATA L"REPORT_DATA"
#define REPORT_DATA_OLD L"REPORT_DATA_OLD"
#define REPORT_DATA_ZONE L"REPORT_DATA_ZONE"
#define REPORT_DATA_ZONE_MONTH L"REPORT_DATA_ZONE_MONTH"
#define REPORT_DATA_STAKE L"REPORT_DATA_STAKE"
#define REPORT_DATA_STAKE_MONTH L"REPORT_DATA_STAKE_MONTH"
#define REPORT_DATA_MISSION L"REPORT_DATA_MISSION"
#define REPORT_DATA_MISSION_MONTH L"REPORT_DATA_MISSION_MONTH"
#define ENGLISH_DATA L"ENGLISH_DATA"
#define ENGLISH_DATA_OLD L"ENGLISH_DATA_OLD"
#define ENGLISH_DATA_UNIT L"ENGLISH_DATA_UNIT"

#define BAPTISM_RECORD L"BAPTISM_RECORD"
#define BAPTISM_SOURCE L"BAPTISM_SOURCE"
#define BAPTISM_SOURCE_ZONE L"BAPTISM_SOURCE_ZONE"
#define BAPTISM_SOURCE_ZONE_MONTH L"BAPTISM_SOURCE_ZONE_MONTH"
#define BAPTISM_SOURCE_STAKE L"BAPTISM_SOURCE_STAKE"
#define BAPTISM_SOURCE_STAKE_MONTH L"BAPTISM_SOURCE_STAKE_MONTH"
#define REFERRAL_HISTORY L"REFERRAL_HISTORY"

//Characters
#define DATE_STAMP_SEPARATOR_CHAR L":"

const double POLL_INTERVAL = 300.0f;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BAR_HEIGHT = 25;
const int BUTTON_WIDTH = 100;
const int CHECKBOX_WIDTH = BAR_HEIGHT;
const int SPACING = 5;

using namespace std;

bool saved = true;
bool auto_check = true;
double auto_check_s = 300.0f;

std::wstring tos(int x)
{
	wchar_t str[256] = L"";
	_itow_s(x, str, 256, 10);
	return str;
}

void show_report_status(ReportCollection* report_collection, CompList* comp_list, std::wstring date, bool english = false)
{
	std::cout << "Received\t\tNot received\n=============================================" << std::endl;
	for (std::map<std::wstring, Area>::iterator it = comp_list->areas.begin(); it != comp_list->areas.end(); ++it)
	{
		std::wstring id_str = date + L":" + it->second.area_name;
		if (!english || it->second.english_unit_name != L"")
		{
			if (report_collection->reports[english ? Report::TYPE_ENGLISH : Report::TYPE_REGULAR][ReportCollection::COMP].reports.count(id_str) > 0)
			{
				std::wcout << it->second.area_name << std::endl;
			}
			else
			{
				std::wcout << L"\t\t\t" << it->second.area_name << std::endl;
			}
		}
	}
}

bool save(FileManager* file_manager, ReportCollection* report_collection, CompList* comp_list, MessageHandler* message_handler, std::wstring date, std::wstring english_date)
{
	//open output files
	bool success = file_manager->open_file(REPORT_DATA, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(REPORT_DATA_ZONE, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(REPORT_DATA_ZONE_MONTH, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(REPORT_DATA_STAKE, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(REPORT_DATA_STAKE_MONTH, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(ENGLISH_DATA, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(ENGLISH_DATA_UNIT, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_RECORD, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_SOURCE, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_SOURCE_ZONE, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_SOURCE_ZONE_MONTH, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_SOURCE_STAKE, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(BAPTISM_SOURCE_STAKE_MONTH, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(MESSAGES_UNPROCESSED, File::FILE_TYPE_OUTPUT)
		&& file_manager->open_file(MESSAGES_PROCESSED, File::FILE_TYPE_OUTPUT);

	if (!success)
		return false;

	message_handler->write_filed_msgs(file_manager->files[MESSAGES_UNPROCESSED]->file, false);
	message_handler->write_filed_msgs(file_manager->files[MESSAGES_PROCESSED]->file, true);

	report_collection->write_report(Report::TYPE_REGULAR, ReportCollection::COMP, file_manager->files[REPORT_DATA]);
	report_collection->total_all_reports(Report::TYPE_REGULAR, comp_list, date);
	report_collection->write_report(Report::TYPE_REGULAR, ReportCollection::ZONE, file_manager->files[REPORT_DATA_ZONE]);
	report_collection->write_report(Report::TYPE_REGULAR, ReportCollection::ZONE_MONTH, file_manager->files[REPORT_DATA_ZONE_MONTH]);
	report_collection->write_report(Report::TYPE_REGULAR, ReportCollection::STAKE, file_manager->files[REPORT_DATA_STAKE]);
	report_collection->write_report(Report::TYPE_REGULAR, ReportCollection::STAKE_MONTH, file_manager->files[REPORT_DATA_STAKE_MONTH]);

	report_collection->write_report(Report::TYPE_ENGLISH, ReportCollection::COMP, file_manager->files[ENGLISH_DATA]);
	report_collection->total_all_reports(Report::TYPE_ENGLISH, comp_list, english_date);
	report_collection->write_report(Report::TYPE_ENGLISH, ReportCollection::ZONE, file_manager->files[ENGLISH_DATA_UNIT]);

	report_collection->write_report(Report::TYPE_BAPTISM_RECORD, ReportCollection::COMP, file_manager->files[BAPTISM_RECORD]);

	report_collection->write_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::COMP, file_manager->files[BAPTISM_SOURCE]);
	report_collection->total_all_reports(Report::TYPE_BAPTISM_SOURCE, comp_list, date);
	report_collection->write_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::ZONE, file_manager->files[BAPTISM_SOURCE_ZONE]);
	report_collection->write_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::ZONE_MONTH, file_manager->files[BAPTISM_SOURCE_ZONE_MONTH]);
	report_collection->write_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::STAKE, file_manager->files[BAPTISM_SOURCE_STAKE]);
	report_collection->write_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::STAKE_MONTH, file_manager->files[BAPTISM_SOURCE_STAKE_MONTH]);

	file_manager->close_file(REPORT_DATA);
	file_manager->close_file(REPORT_DATA_ZONE);
	file_manager->close_file(REPORT_DATA_ZONE_MONTH);
	file_manager->close_file(REPORT_DATA_STAKE);
	file_manager->close_file(REPORT_DATA_STAKE_MONTH);
	file_manager->close_file(ENGLISH_DATA);
	file_manager->close_file(ENGLISH_DATA_UNIT);
	file_manager->close_file(BAPTISM_RECORD);
	file_manager->close_file(BAPTISM_SOURCE);
	file_manager->close_file(BAPTISM_SOURCE_ZONE);
	file_manager->close_file(BAPTISM_SOURCE_ZONE_MONTH);
	file_manager->close_file(BAPTISM_SOURCE_STAKE);
	file_manager->close_file(BAPTISM_SOURCE_STAKE_MONTH);
	file_manager->close_file(MESSAGES_UNPROCESSED);
	file_manager->close_file(MESSAGES_PROCESSED);

	return true;
}

bool load(FileManager* file_manager, ReportCollection* report_collection, CompList* comp_list, MessageHandler* message_handler)
{
	file_manager->open_file(PH_LIST, File::FILE_TYPE_INPUT);
	file_manager->open_file(MESSAGES_UNPROCESSED, File::FILE_TYPE_INPUT);
	file_manager->open_file(MESSAGES_PROCESSED, File::FILE_TYPE_INPUT);

	file_manager->open_file(REPORT_DATA, File::FILE_TYPE_INPUT);
	file_manager->open_file(REPORT_DATA_ZONE, File::FILE_TYPE_INPUT);
	file_manager->open_file(REPORT_DATA_ZONE_MONTH, File::FILE_TYPE_INPUT);
	file_manager->open_file(REPORT_DATA_STAKE, File::FILE_TYPE_INPUT);
	file_manager->open_file(REPORT_DATA_STAKE_MONTH, File::FILE_TYPE_INPUT);
	file_manager->open_file(ENGLISH_DATA, File::FILE_TYPE_INPUT);
	file_manager->open_file(ENGLISH_DATA_UNIT, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_SOURCE, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_SOURCE_ZONE, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_SOURCE_ZONE_MONTH, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_SOURCE_STAKE, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_SOURCE_STAKE_MONTH, File::FILE_TYPE_INPUT);
	file_manager->open_file(BAPTISM_RECORD, File::FILE_TYPE_INPUT);

	file_manager->open_file(REPORT_DATA_OLD, File::FILE_TYPE_OUTPUT, true);
	file_manager->files[REPORT_DATA_OLD]->file << "=========================";
	file_manager->files[REPORT_DATA_OLD]->file << file_manager->files[REPORT_DATA]->file.rdbuf();
	file_manager->close_file(REPORT_DATA_OLD);

	message_handler->read_filed_msgs(file_manager->files[MESSAGES_UNPROCESSED]->file, false);
	message_handler->read_filed_msgs(file_manager->files[MESSAGES_PROCESSED]->file, true);

	report_collection->read_report(Report::TYPE_REGULAR, ReportCollection::COMP, file_manager->files[REPORT_DATA]);
	report_collection->read_report(Report::TYPE_REGULAR, ReportCollection::ZONE, file_manager->files[REPORT_DATA_ZONE]);
	report_collection->read_report(Report::TYPE_REGULAR, ReportCollection::ZONE_MONTH, file_manager->files[REPORT_DATA_ZONE_MONTH]);
	report_collection->read_report(Report::TYPE_REGULAR, ReportCollection::STAKE, file_manager->files[REPORT_DATA_STAKE]);
	report_collection->read_report(Report::TYPE_REGULAR, ReportCollection::STAKE_MONTH, file_manager->files[REPORT_DATA_STAKE_MONTH]);

	report_collection->read_report(Report::TYPE_ENGLISH, ReportCollection::COMP, file_manager->files[ENGLISH_DATA]);
	report_collection->read_report(Report::TYPE_ENGLISH, ReportCollection::ZONE, file_manager->files[ENGLISH_DATA_UNIT]);
	report_collection->read_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::COMP, file_manager->files[BAPTISM_SOURCE]);
	report_collection->read_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::ZONE, file_manager->files[BAPTISM_SOURCE_ZONE]);
	report_collection->read_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::ZONE_MONTH, file_manager->files[BAPTISM_SOURCE_ZONE_MONTH]);
	report_collection->read_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::STAKE, file_manager->files[BAPTISM_SOURCE_STAKE]);
	report_collection->read_report(Report::TYPE_BAPTISM_SOURCE, ReportCollection::STAKE_MONTH, file_manager->files[BAPTISM_SOURCE_STAKE_MONTH]);

	report_collection->read_report(Report::TYPE_BAPTISM_RECORD, ReportCollection::COMP, file_manager->files[BAPTISM_RECORD]);

	comp_list->load(file_manager->files[PH_LIST]->file);

	file_manager->close_file(PH_LIST);
	file_manager->close_file(MESSAGES_UNPROCESSED);
	file_manager->close_file(MESSAGES_PROCESSED);
	file_manager->close_file(REPORT_DATA);
	file_manager->close_file(REPORT_DATA_ZONE);
	file_manager->close_file(REPORT_DATA_ZONE_MONTH);
	file_manager->close_file(REPORT_DATA_STAKE);
	file_manager->close_file(REPORT_DATA_STAKE_MONTH);
	file_manager->close_file(ENGLISH_DATA);
	file_manager->close_file(ENGLISH_DATA_UNIT);
	file_manager->close_file(BAPTISM_SOURCE);
	file_manager->close_file(BAPTISM_SOURCE_ZONE);
	file_manager->close_file(BAPTISM_SOURCE_ZONE_MONTH);
	file_manager->close_file(BAPTISM_SOURCE_STAKE);
	file_manager->close_file(BAPTISM_SOURCE_STAKE_MONTH);
	file_manager->close_file(BAPTISM_RECORD);

	return true;
}

/* Create the date stamp for a reporting period, based on the current time and the weekday of reporting.
 * All days up to the day of reporting return back to the previous reporting period (e.g. Thursday-Tuesday will be counted as reports
 * for English reporting session which began on the Wednesday previous).
 */
std::wstring get_report_date_str(std::wstring report_wday)
{
	std::wstring report_date;
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);

	int cur_wday = (curtime_st.tm_wday == 0 ? 7 : curtime_st.tm_wday);
	if (cur_wday == _wtoi(report_wday.c_str()))
	{
		report_date = tos(1900 + curtime_st.tm_year)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(curtime_st.tm_mon + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + tos((curtime_st.tm_mday + 6) / 7)
			+ DATE_STAMP_SEPARATOR_CHAR + report_wday;
	}
	else
	{
		tm last_week_tm_st = curtime_st;
		last_week_tm_st.tm_mday -= 7;
		time_t last_week_tm = mktime(&last_week_tm_st);
		localtime_s(&last_week_tm_st, &last_week_tm);
		report_date = tos(1900 + last_week_tm_st.tm_year)
			+ DATE_STAMP_SEPARATOR_CHAR + tos(last_week_tm_st.tm_mon + 1)
			+ DATE_STAMP_SEPARATOR_CHAR + tos((last_week_tm_st.tm_mday + 6) / 7)
			+ DATE_STAMP_SEPARATOR_CHAR + report_wday;
	}
	return report_date;
}

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

void run_terminal_commands(Terminal* terminal)
{
	clock_t start = clock();
	clock_t end = start;
	while (terminal->update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f))
	{
		start = end;
		end = clock();
	}
}



void save_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	save(terminal->file_manager, terminal->report_collection, terminal->comp_list, terminal->msg_handler, terminal->date, terminal->english_date);
	saved = true;
}

void quit_cb(Fl_Widget* wg, void* ptr)
{
	if (saved || fl_ask("Unsaved changes! Are you sure you want to quit?"))
		exit(0);
}

void user_terminal_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	terminal->init_user();
	run_terminal_commands(terminal);
}

class MessageScrollItem : public Fl_Group {
	Fl_Check_Button *fixedBox;
	Fl_Box *stretchBox;
public:
	MessageScrollItem(int X, int Y, int W, int H, Message* msg) : Fl_Group(X, Y, W, H, 0) {
		begin();
		// Fixed width box
		
		// Stretchy box
		std::wstring box_contents = msg->sender_name + L": " + msg->contents;
		std::string str(box_contents.begin(), box_contents.end());

		fixedBox = new Fl_Check_Button(X, Y, CHECKBOX_WIDTH, H);
		stretchBox = new Fl_Box(X + CHECKBOX_WIDTH, Y, W - CHECKBOX_WIDTH, H);
		stretchBox->copy_label(str.c_str());
		stretchBox->box(FL_UP_BOX);
		stretchBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP);
		resizable(stretchBox);
		end();
	}
};

// Custom scroll that tells children to follow scroll's width when resized
class MessageScroll : public Fl_Scroll {
public:
	int cur_y;

	MessageScroll(int X, int Y, int W, int H, const char* L = 0)
		: Fl_Scroll(X, Y, W, H, L), cur_y(0)
	{
	}

	// Append new message to bottom
	void AddItem(Message* msg) {
		int X = x() + 1;
		int Y = y() - yposition() + cur_y + 1;
		int W = w() - 20;                           // -20: compensate for vscroll bar
		int H = (std::count(msg->contents.begin(), msg->contents.end(), '\n') + 1) * BAR_HEIGHT;
		Fl_Widget* w = child(0);
		MessageScrollItem* item = new MessageScrollItem(X, Y, W, H, msg);
		add(item);
		redraw();
		cur_y += H;
	}

	void clear()
	{
		Fl_Scroll::clear();
		cur_y = 0;
	}
};

MessageScroll* unhandled;
MessageScroll* handled;

void update_msg_scroll(MessageHandler* msg_handler)
{
	unhandled->clear();
	handled->clear();
	for (int i = 0; i < msg_handler->msgs_unhandled.size(); i++)
	{
		unhandled->AddItem(&msg_handler->msgs_unhandled[i]);
	}
	for (int i = 0; i < msg_handler->msgs_handled.size(); i++)
	{
		handled->AddItem(&msg_handler->msgs_handled[i]);
	}
}

void check_msg_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	terminal->init_auto();
	run_terminal_commands(terminal);
	update_msg_scroll(terminal->msg_handler);
	saved = false;
}

void process_msg_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	terminal->msg_handler->process_messages(terminal, terminal->report_collection, terminal->comp_list, terminal->date, terminal->english_date);
	run_terminal_commands(terminal);
	update_msg_scroll(terminal->msg_handler);
	saved = false;
}

void timer_cb(void* ptr)
{
	check_msg_cb(NULL, ptr);
	process_msg_cb(NULL, ptr);
	Fl::repeat_timeout(POLL_INTERVAL, timer_cb, ptr);
}

int main(int argc, char **argv)
{
	ReportCollection report_collection;
	CompList comp_list;
	MessageHandler msg_handler;
	Modem modem;

	std::wcout << "Loading..." << std::endl;
	FileManager file_manager(L"paths.txt");
	file_manager.open_file(OUTPUT, File::FILE_TYPE_OUTPUT, true);
	load(&file_manager, &report_collection, &comp_list, &msg_handler);
	std::wstring report_wday = file_manager.config_file.values[L"REPORT_WDAY"];
	std::wstring english_wday = file_manager.config_file.values[L"ENGLISH_WDAY"];
	Reminder report_reminder(file_manager.config_file.values[L"REPORT_REMINDER"]);
	Reminder english_reminder(file_manager.config_file.values[L"ENGLISH_REMINDER"]);
	english_reminder.english = true;
	std::wstring report_date = get_report_date_str(report_wday);
	std::wstring english_date = get_report_date_str(english_wday);

	
	Terminal terminal(report_date, english_date, &modem, &report_collection, &comp_list, &msg_handler, file_manager.files[OUTPUT], &file_manager);
	terminal.add_reminder(report_reminder);
	terminal.add_reminder(english_reminder);

	// process string

	bool quit = false;
	bool run_terminal = false;


	Fl::add_timeout(POLL_INTERVAL, timer_cb, &terminal);
	Fl_Window* window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT);
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, WINDOW_WIDTH, BAR_HEIGHT);
	{
		menu->add("File/Save", FL_CTRL + 's', save_cb, &terminal);
		menu->add("File/Quit", FL_CTRL + 'q', quit_cb);
	}
	Fl_Tabs* tabs = new Fl_Tabs(0, BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT - BAR_HEIGHT - SPACING);
	{
		Fl_Group* msg_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "Messages");
		{
			unhandled = new MessageScroll(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			unhandled->box(FL_BORDER_BOX);
			unhandled->end();
			handled = new MessageScroll(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			handled->box(FL_BORDER_BOX);
			handled->end();
			Fl_Button* check_msg_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Check msgs");
			check_msg_button->user_data((void*)(&terminal));
			check_msg_button->callback(check_msg_cb);
			Fl_Button* process_msg_button = new Fl_Button(BUTTON_WIDTH + 2 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Process msgs");
			process_msg_button->user_data((void*)(&terminal));
			process_msg_button->callback(process_msg_cb);
			Fl_Button* user_terminal_button = new Fl_Button(2 * BUTTON_WIDTH + 3 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "User Terminal");
			user_terminal_button->user_data((void*)(&terminal));
			user_terminal_button->callback(user_terminal_cb);
		}
		msg_tab->end();
		Fl_Group* report_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "Reports");
		{

		}
		report_tab->end();
		Fl_Group* english_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "English");
		{

		}
		english_tab->end();
	}
	tabs->end();
	window->end();
	window->show();
	update_msg_scroll(&msg_handler);
	Fl::run();
	return 0;
}