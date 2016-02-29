#pragma once

#include "CompList.h"
#include "MessageHandler.h"
#include "Modem.h"
#include "ReportCollection.h"
#include "Terminal.h"

#include <FL/Fl_Multi_Browser.H>

#include <string>

std::wstring get_browser_display_txt(std::wstring str);

void save_cb(Fl_Widget* wg, void* ptr);
void quit_cb(Fl_Widget* wg, void* ptr);
void user_terminal_cb(Fl_Widget* wg, void* ptr);
void send_reminder_cb(Fl_Widget* wg, void* ptr);
void send_english_reminder_cb(Fl_Widget* wg, void*ptr);
void check_msg_cb(Fl_Widget* wg, void* ptr);
void process_msg_cb(Fl_Widget* wg, void* ptr);
void unprocess_msg_cb(Fl_Widget* wg, void* ptr);
void timer_cb(void* ptr);
void select_all_cb(Fl_Widget* wg, void* ptr);

class Gui
{
public:
	FileManager file_manager;
	MessageHandler msg_handler;
	ReportCollection report_collection;
	CompList comp_list;
	Terminal terminal;
	Modem modem;
	FieldCollection config;

	Fl_Multi_Browser* unhandled;
	Fl_Multi_Browser* handled;
	Fl_Multi_Browser* unreceived_reports;
	Fl_Multi_Browser* received_reports;
	Fl_Multi_Browser* unreceived_english;
	Fl_Multi_Browser* received_english;

	std::wstring report_date;
	std::wstring english_date;
	std::wstring report_wday;
	std::wstring english_wday;

	bool saved;
	bool auto_check;
	double auto_check_s;

	Gui();
	virtual ~Gui();

	void init();
	void run();
	void save();
	void load();
	void run_terminal_commands();
	void update_msg_scroll();
	void update_report_scrolls();
};

