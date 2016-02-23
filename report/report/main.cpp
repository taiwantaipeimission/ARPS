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
#include <FL/Fl_Multi_Browser.H>

#include "Modem.h"
#include "ReportCollection.h"
#include "CompList.h"
#include "Terminal.h"
#include "Area.h"
#include "Reminder.h"
#include "Message.h"
#include "MessageHandler.h"
#include "FieldFile.h"

// File handles
#define OUTPUT L"OUTPUT"
#define PH_LIST L"PH_LIST"
#define MESSAGES_PROCESSED L"MESSAGES_PROCESSED"
#define MESSAGES_UNPROCESSED L"MESSAGES_UNPROCESSED"
#define REFERRAL_HISTORY L"REFERRAL_HISTORY"

//Characters
#define DATE_STAMP_SEPARATOR_CHAR L":"

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

std::string tostr(std::wstring str)
{
	std::string st(str.begin(), str.end());
	return st;
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

bool save(ReportCollection* report_collection, CompList* comp_list, MessageHandler* message_handler, std::wstring date, std::wstring english_date)
{
	report_collection->total_all(comp_list, date);
	report_collection->save_all();
	message_handler->save();

	return true;

}

bool load(ReportCollection* report_collection, CompList* comp_list, MessageHandler* message_handler, FieldFile* config, File* output)
{
	report_collection->load_all();
	comp_list->load();
	message_handler->load();

	config->filepath = L"config/config.txt";
	config->open(File::FILE_TYPE_INPUT, false);

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
	save(terminal->report_collection, terminal->comp_list, terminal->msg_handler, terminal->date, terminal->english_date);
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

void send_reminder_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	terminal->send_reminders();
	run_terminal_commands(terminal);
}

void send_english_reminder_cb(Fl_Widget* wg, void*ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	terminal->send_reminders(true);
	run_terminal_commands(terminal);
}

Fl_Multi_Browser* unhandled;
Fl_Multi_Browser* handled;

void update_msg_scroll(MessageHandler* msg_handler)
{
	unhandled->clear();
	handled->clear();
	for (int i = 0; i < msg_handler->msgs_unhandled.size(); i++)
	{
		unhandled->add(tostr(msg_handler->msgs_unhandled[i]->sender_name + L":" + msg_handler->msgs_unhandled[i]->contents).c_str(), (void*)msg_handler->msgs_unhandled[i]);
	}

	for (int i = 0; i < msg_handler->msgs_handled.size(); i++)
	{
		handled->add(tostr(msg_handler->msgs_handled[i]->sender_name + L":").c_str(), (void*)msg_handler->msgs_handled[i]);
	}
	unhandled->redraw();
	handled->redraw();
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
	wcout << "Before processing:\n";
	for (int i = 1; i <= unhandled->size(); i++)
	{
		wcout << i << ":" << ((Message*)unhandled->data(i))->contents << endl;
	}

	wcout << "During processing:\n";
	Terminal* terminal = (Terminal*)ptr;
	for (int j = 1; j <= unhandled->size(); j++)
	{
		if (unhandled->selected(j))
		{
			wcout << j << L":" << ((Message*)unhandled->data(j))->contents << endl;
			terminal->msg_handler->process_msg((Message*)unhandled->data(j), terminal, terminal->report_collection, terminal->comp_list, terminal->date, terminal->english_date);
		}
	}
	run_terminal_commands(terminal);
	update_msg_scroll(terminal->msg_handler);
	saved = false;
}

void unprocess_msg_cb(Fl_Widget* wg, void* ptr)
{
	Terminal* terminal = (Terminal*)ptr;
	for (int i = 1; i <= handled->size(); i++)
	{
		if (handled->selected(i))
		{
			terminal->msg_handler->unprocess_msg((Message*)handled->data(i));
		}
	}
	update_msg_scroll(terminal->msg_handler);
	saved = false;
}

void timer_cb(void* ptr)
{
	check_msg_cb(NULL, ptr);
	process_msg_cb(NULL, ptr);
	Fl::repeat_timeout(auto_check_s, timer_cb, ptr);
}

int main(int argc, char **argv)
{
	ReportCollection report_collection;
	CompList comp_list;
	MessageHandler msg_handler;
	Modem modem;
	FieldFile config;
	File output;

	std::wcout << "Loading..." << std::endl;
	load(&report_collection, &comp_list, &msg_handler, &config, &output);
	std::wstring report_wday = config.values[L"REPORT_WDAY"];
	std::wstring english_wday = config.values[L"ENGLISH_WDAY"];
	std::wstring report_date = get_report_date_str(report_wday);
	std::wstring english_date = get_report_date_str(english_wday);

	Terminal terminal(report_date, english_date, &modem, &report_collection, &comp_list, &msg_handler, &output);

	Fl::add_timeout(auto_check_s, timer_cb, &terminal);
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
			unhandled = new Fl_Multi_Browser(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			{
				unhandled->box(FL_BORDER_BOX);
				unhandled->end();
			}
			handled = new Fl_Multi_Browser(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			{
				handled->box(FL_BORDER_BOX);
				handled->end();
			}
			Fl_Button* check_msg_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Check msgs");
			{
				check_msg_button->user_data((void*)(&terminal));
				check_msg_button->callback(check_msg_cb);
			}
			Fl_Button* process_msg_button = new Fl_Button(BUTTON_WIDTH + 2 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Process msgs");
			{
				process_msg_button->user_data((void*)(&terminal));
				process_msg_button->callback(process_msg_cb);
			}
			Fl_Button* unprocess_msg_button = new Fl_Button(2 * BUTTON_WIDTH + 3 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Mark as unprocessed");
			{
				unprocess_msg_button->user_data((void*)(&terminal));
				unprocess_msg_button->callback(unprocess_msg_cb);
			}
			Fl_Button* user_terminal_button = new Fl_Button(3 * BUTTON_WIDTH + 4 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "User Terminal");
			{
				user_terminal_button->user_data((void*)(&terminal));
				user_terminal_button->callback(user_terminal_cb);
			}
		}
		msg_tab->end();
		Fl_Group* report_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "Reports");
		{
			Fl_Multi_Browser* unreceived = new Fl_Multi_Browser(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			unreceived->box(FL_BORDER_BOX);
			unreceived->end();
			Fl_Multi_Browser* received = new Fl_Multi_Browser(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			received->box(FL_BORDER_BOX);
			received->end();

			int i = 0;
			for (std::map<std::wstring, Area>::iterator it = comp_list.areas.begin(); it != comp_list.areas.end(); ++it, i++)
			{
				std::wstring id_str = report_date + L":" + it->second.area_name;
				if (report_collection.reports[Report::TYPE_REGULAR][ReportCollection::COMP].reports.count(id_str) > 0)
				{
					received->add(tostr(it->second.area_name).c_str());
				}
				else if (it->second.report_required)
				{
					unreceived->add(tostr(it->second.area_name).c_str());
				}
			}
			unreceived->redraw();
			received->redraw();

			Fl_Button* reminder_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Remind 'em");
			{
				reminder_button->user_data((void*)&terminal);
				reminder_button->callback(send_reminder_cb);
			}
		}
		report_tab->end();
		Fl_Group* english_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "English");
		{
			Fl_Multi_Browser* unreceived = new Fl_Multi_Browser(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			unreceived->box(FL_BORDER_BOX);
			unreceived->end();
			Fl_Multi_Browser* received = new Fl_Multi_Browser(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			received->box(FL_BORDER_BOX);
			received->end();

			int i = 0;
			for (std::map<std::wstring, Area>::iterator it = comp_list.areas.begin(); it != comp_list.areas.end(); ++it, i++)
			{
				std::wstring id_str = english_date + L":" + it->second.area_name;
				if (report_collection.reports[Report::TYPE_ENGLISH][ReportCollection::COMP].reports.count(id_str) > 0)
				{
					received->add(tostr(it->second.area_name).c_str());
				}
				else if (it->second.english_required)
				{
					unreceived->add(tostr(it->second.area_name).c_str());
				}
			}
			unreceived->redraw();
			received->redraw();

			Fl_Button* reminder_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Remind 'em");
			{
				reminder_button->user_data((void*)&terminal);
				reminder_button->callback(send_english_reminder_cb);
			}
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