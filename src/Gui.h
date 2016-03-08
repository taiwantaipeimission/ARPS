#pragma once

#include "CompList.h"
#include "MessageHandler.h"
#include "Modem.h"
#include "ModemData.h"
#include "ReportCollection.h"
#include "ReferralList.h"

#include <FL/Fl_Multi_Browser.H>

#include <string>

std::wstring get_browser_display_txt(std::wstring str);

void total_report_cb(Fl_Widget* wg, void* ptr);
void total_english_cb(Fl_Widget* wg, void* ptr);
void total_baptism_source_cb(Fl_Widget* wg, void* ptr);
void save_cb(Fl_Widget* wg, void* ptr);
void quit_cb(Fl_Widget* wg, void* ptr);
void user_terminal_cb(Fl_Widget* wg, void* ptr);
void send_reminder_cb(Fl_Widget* wg, void* ptr);
void send_english_reminder_cb(Fl_Widget* wg, void* ptr);
void poll_msg_cb(Fl_Widget* wg, void* ptr);
void check_msg_cb(void* ptr);
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
	ReferralList referral_list;
	CompList comp_list;
	ModemData* modem_data;
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

	bool auto_check;
	double auto_check_s;

	Gui();
	virtual ~Gui();

	void init(ModemData* modem_data_in);
	void run();
	void total_reports(Report::Type type);
	bool is_saved();
	void save();
	void load();
	
	void send_message(std::wstring ph_num, std::wstring contents);
	void send_reminders(bool english);

	void delete_message_from_sim(int msg_cmg_id);

	void poll_msgs();
	void check_msgs();
	void process_msg(Message* msg);
	void unprocess_msg(Message* msg);
	void update_msg_scroll();
	void update_report_scrolls();
};

