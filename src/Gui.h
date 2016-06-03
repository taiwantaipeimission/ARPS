/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "CompList.h"
#include "MessageHandler.h"
#include "Modem.h"
#include "ModemInterface.h"
#include "ReportCollection.h"

#include <rapidjson/document.h>

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
void timer_cb(void* ptr);
void auto_process_button_cb(Fl_Widget* wg, void* ptr);
void compose_message_cb(Fl_Widget* wg, void* ptr);

class MessageBrowser : public Fl_Multi_Browser
{
public:
	Gui* gui;
	Fl_Window* msg_popup;
	Message* popped_msg;
	MessageHandler::MessageStorageType type;
	MessageBrowser(Gui* gui_in, MessageHandler::MessageStorageType message_storage_type_in, int x, int y, int w, int h, const char* label = 0);

	void message_popup(Message* msg);
	int handle(int event);
	int num_selected();

	void select_all();
};

class Gui
{
public:
	FileManager file_manager;
	MessageHandler msg_handler;
	ReportCollection report_collection;
	CompList comp_list;
	ModemInterface* modem_interface;
	rapidjson::Document config;

	Fl_Window* window;
	MessageBrowser* unhandled;
	MessageBrowser* handled;
	Fl_Multi_Browser* unreceived_reports;
	Fl_Multi_Browser* received_reports;
	Fl_Multi_Browser* unreceived_english;
	Fl_Multi_Browser* received_english;
	Fl_Button* check_message_button;

	std::wstring current_date;
	std::wstring english_date;
	std::wstring report_date;

	bool auto_check;
	bool checking_msgs;

	Gui();
	virtual ~Gui();

	void init(ModemInterface* mod_int_in);
	void run();
	void total_reports(ReportType type);
	bool is_saved();
	void save();
	void load();
	
	void send_message(std::wstring ph_num, std::wstring contents);
	void send_reminder(Area* area, bool english);

	void delete_message_from_sim(int msg_cmg_id);

	void poll_msgs();
	void configure_modem();
	bool completed_command_cb();
	void process_msg(Message* msg);
	void unprocess_msg(Message* msg);
	void update_msg_scroll();
	void update_report_scrolls();
};

