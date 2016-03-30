#pragma once

#include "CompList.h"
#include "MessageHandler.h"
#include "Modem.h"
#include "ModemInterface.h"
#include "ReportCollection.h"
#include "ReferralList.h"

class Fl_Button;
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
void send_verify_text_cb(Fl_Widget* wg, void* ptr);
void delete_msg_cb(Fl_Widget* wg, void* ptr);
void poll_msg_cb(Fl_Widget* wg, void* ptr);
void completed_command_cb(void* ptr);
void process_msg_cb(Fl_Widget* wg, void* ptr);
void unprocess_msg_cb(Fl_Widget* wg, void* ptr);
void select_all_cb(Fl_Widget* wg, void* ptr);
void window_cb(Fl_Widget* wg, void* ptr);

class MessageBrowser : public Fl_Multi_Browser
{
public:
	Gui* gui;
	bool handled;
	MessageBrowser(Gui* gui_in, bool handled_in, int x, int y, int w, int h, const char* label = 0);
	int handle(int event);
	int num_selected();
};

class Gui
{
public:
	FileManager file_manager;
	MessageHandler msg_handler;
	ReportCollection report_collection;
	ReferralList referral_list;
	CompList comp_list;
	ModemInterface* modem_interface;
	FieldCollection config;

	Fl_Window* window;
	MessageBrowser* unhandled;
	MessageBrowser* handled;
	Fl_Multi_Browser* unreceived_reports;
	Fl_Multi_Browser* received_reports;
	Fl_Multi_Browser* unreceived_english;
	Fl_Multi_Browser* received_english;
	Fl_Button* check_message_button;

	std::wstring report_date;
	std::wstring english_date;
	std::wstring current_date;
	std::wstring report_wday;
	std::wstring english_wday;
	std::wstring stray_msg_handler;

	bool auto_check;
	double auto_check_s;
	bool checking_msgs;

	Gui();
	virtual ~Gui();

	void init(ModemInterface* mod_int_in);
	void run();
	void total_reports(Report::Type type);
	bool is_saved();
	void save();
	void load();
	
	void send_message(std::wstring ph_num, std::wstring contents);
	void send_reminder(Area* area);

	void delete_message_from_sim(int msg_cmg_id);

	void poll_msgs();
	void configure_modem();
	bool completed_command_cb();
	void process_msg(Message* msg);
	void unprocess_msg(Message* msg);
	void update_msg_scroll();
	void update_report_scrolls();
};

