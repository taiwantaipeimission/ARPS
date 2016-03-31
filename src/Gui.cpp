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

#include "Gui.h"
#include "utility.h"
#include "codes.h"
#include "Area.h"
#include "Referral.h"
#include "Terminal.h"

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
#include <FL/Fl_PNG_Image.H>

#include <ctime>

std::wstring get_browser_display_txt(std::wstring str)
{
	size_t pos = 0;
	while ((pos = str.find('\n', pos)) != std::wstring::npos)
	{
		str.replace(pos, 1, L" ");
	}
	if (str.size() > 100)
	{
		str.resize(100);
		str += L"...";
	}
	return str;
}

void total_report_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->total_reports(Report::TYPE_REGULAR);
}

void total_english_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->total_reports(Report::TYPE_ENGLISH);
}

void total_baptism_source_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->total_reports(Report::TYPE_BAPTISM_SOURCE);
}

void save_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->save();
}

void quit_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;

	bool save = false;
	bool quit = true;
	if (!gui->is_saved())
	{
		int choice = fl_choice("Save changes before quit?", "Cancel", "Save", "Don't save");
		if (choice == 0)
		{
			save = false;
			quit = false;
		}
		else if (choice == 1)
		{
			save = true;
			quit = true;
		}
		else if (choice == 2)
		{
			save = false;
			quit = true;
		}
	}
	if (save)
		gui->save();
	if (quit)
		exit(0);
}

void user_terminal_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;

	set_color(CC_GREEN, CC_BLACK);
	wcout << L"Entered user terminal\n";
	set_color(CC_GREY, CC_BLACK);
	wcout << L"-Enter commands here and press ENTER.\n-To exit terminal, type \'q\'.\n-Type \'$\' for a message-end character.\n-Type \'~' for an escape character.\n";
	set_color(CC_WHITE, CC_BLACK);
	std::wstring cmd;
	std::getline(std::wcin, cmd);
	while (cmd != L"q")
	{
		replace_chars(cmd, L"$", COMMAND_ESCAPE_CHAR);
		replace_chars(cmd, L"~", L"\u001B");
		gui->modem_interface->push_command(cmd + L"\r");
		std::getline(std::wcin, cmd);
	}
	set_color(CC_GREEN, CC_BLACK);
	wcout << L"Exited user terminal\n";
	set_color(CC_WHITE, CC_BLACK);
}

void configure_modem_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;

	gui->configure_modem();
}

void send_reminder_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int i = 1; i <= gui->unreceived_reports->size(); i++)
	{
		if(gui->unreceived_reports->selected(i))
			gui->send_reminder((Area*)gui->unreceived_reports->data(i));
	}
}

void send_english_reminder_cb(Fl_Widget* wg, void*ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int i = 1; i <= gui->unreceived_english->size(); i++)
	{
		if(gui->unreceived_english->selected(i))
			gui->send_reminder((Area*)gui->unreceived_english->data(i));
	}
}

void send_verify_text_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (std::map<std::wstring, Area>::iterator it = gui->comp_list.areas.begin(); it != gui->comp_list.areas.end(); ++it)
	{
		if (!it->first.empty() && !it->second.district_name.empty())
		{
			std::wstring msg_text = L"Checking: this phone belongs to the "
				+ it->second.area_name
				+ L", in the "
				+ it->second.zone_name
				+ L" zone, in the "
				+ it->second.district_name
				+ L" district.\nIf not, please call the Operations Assistant.";
			gui->send_message(it->first, msg_text);
		}
	}
}

void delete_msg_cb(Fl_Widget* wg, void* ptr)
{
	MessageBrowser* browser = (MessageBrowser*)ptr;
	Gui* gui = browser->gui;

	int n_selected = 0;
	for (int i = 1; i <= browser->size(); n_selected += browser->selected(i++) ? 1 : 0)
	{ }

	std::wstring prompt = L"Are you sure you want to delete " + tos(n_selected) + L" messages?";
	if (n_selected > 0 && fl_choice(tos(prompt).c_str(), NULL, "Cancel", "OK") == 2)
	{

		std::vector<Message*>handled_to_erase;
		std::vector<Message*>unhandled_to_erase;
		for (int i = 1; i <= browser->size(); i++)
		{
			if (browser->selected(i))
			{
				Message* msg = (Message*)browser->data(i);
				if (browser->handled)
					handled_to_erase.push_back(msg);
				else
					unhandled_to_erase.push_back(msg);
				//browser->remove(i - 1);
				gui->msg_handler.changed = true;
			}
		}
		for (size_t i = 0; i < handled_to_erase.size(); i++)
		{
			gui->msg_handler.erase_message(handled_to_erase[i], MessageHandler::HANDLED);
		}
		for (size_t i = 0; i < unhandled_to_erase.size(); i++)
		{
			gui->msg_handler.erase_message(unhandled_to_erase[i], MessageHandler::UNHANDLED);
		}
		//browser->redraw();
		gui->update_msg_scroll();
	}
}

void poll_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->poll_msgs();
}

void completed_command_cb(void* ptr)
{
	Gui* gui = (Gui*)ptr;
	if (gui->completed_command_cb())
	{
		gui->update_msg_scroll();
		gui->update_report_scrolls();
	}
}

void process_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	MessageBrowser* browser = gui->unhandled;
	for (int j = 1; j <= browser->size(); j++)
	{
		if (browser->selected(j))
		{
			gui->process_msg((Message*)browser->data(j));
		}
	}
	gui->update_msg_scroll();
	gui->update_report_scrolls();
}

void unprocess_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	MessageBrowser* browser = gui->handled;
	for (int i = 1; i <= browser->size(); i++)
	{
		if (browser->selected(i))
		{
			gui->unprocess_msg((Message*)browser->data(i));
		}
	}
	gui->update_msg_scroll();
}

void select_all_cb(Fl_Widget* wg, void* ptr)
{

}

void about_cb(Fl_Widget* wg, void* ptr)
{
	fl_message(tos((wstring)L"Automatic Reporting System v." + VERSION +
		L"\nCopyright 2016 David B. Elliott\nelliott.david.ballantyne@@gmail.com"
		L"\nTaiwan Taipei Mission"
		L"\nMade and distributed under the GNU General Public License V3").c_str());
}

void window_cb(Fl_Widget* wg, void* ptr)
{
	quit_cb(wg, ptr);
}

MessageBrowser::MessageBrowser(Gui* gui_in, bool handled_in, int x, int y, int w, int h, const char* label)
	: Fl_Multi_Browser(x, y, w, h, label), gui(gui_in), handled(handled_in)
{
}

int MessageBrowser::handle(int event)
{
	int handled = 0;
	if (event == FL_PUSH)
	{
		if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			void* item = find_item(Fl::event_y());
			if (item && item_selected(item))
			{
				handled = 1;
			}
		}
		else if (Fl::event_button() == FL_LEFT_MOUSE && Fl::event_clicks())
		{
			Message* msg = value() ? (Message*)data(value()) : NULL;
			if (msg)
			{
				std::wstring msg_text = msg->get_contents();
				fl_message(tos(msg_text).c_str());
			}
			handled = 1;
		}
	}
	else if(event == FL_RELEASE)
	{
		if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			if (num_selected() > 0)
			{
				Fl_Menu_Item rclick_menu[] = {
					{ "Delete", 0, delete_msg_cb, (void*)this },
					{ 0 }
				};
				const Fl_Menu_Item* m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
				if (m)
					m->do_callback(0, m->user_data());
				handled = 1;
			}
		}
	}
	else if (event == FL_SHORTCUT)
	{
		if (Fl::event_key() == FL_CTRL + 'a')
		{
			for (int i = 1; i <= size(); i++)
				select(i);
		}
	}
	else if (event == FL_KEYDOWN)
	{
		if (Fl::event_key() == FL_Enter)
		{
			Message* msg = value() ? (Message*)data(value()): NULL;
			if (msg)
			{
				std::wstring msg_text = msg->get_contents();
				fl_message(tos(msg_text).c_str());
			}
			handled = 1;
		}
	}
	if(!handled)
		handled = Fl_Multi_Browser::handle(event);
	return handled;
}

int MessageBrowser::num_selected()
{
	int n_selected = 0;
	for (int i = 1; i <= size(); n_selected += selected(i++) ? 1 : 0)
	{
	}
	return n_selected;
}

Gui::Gui()
{
}


Gui::~Gui()
{
}

void Gui::init(ModemInterface* mod_int_in)
{
	checking_msgs = false;

	modem_interface = mod_int_in;
	auto_check_s = 300.0f;
	file_manager.files[FILE_OUTPUT].append = true;
	file_manager.files[FILE_OUTPUT].open(File::FILE_TYPE_OUTPUT);

	Fl_PNG_Image* image = new Fl_PNG_Image("../res/logo.png");
	window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "ARPS");
	window->default_icon(image);
	window->callback(window_cb, this);
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, WINDOW_WIDTH, BAR_HEIGHT);
	{
		menu->add("File/Save", FL_CTRL + 's', save_cb, this);
		menu->add("File/Quit", FL_CTRL + 'q', quit_cb, this);
		menu->add("Edit/Total reports", FL_CTRL + 'r', total_report_cb, this);
		menu->add("Edit/Total English", FL_CTRL + 'e', total_english_cb, this);
		menu->add("Edit/Total baptism source", FL_CTRL + 'b', total_baptism_source_cb, this);
		menu->add("Areas/Send verification text", NULL, send_verify_text_cb, this);
		menu->add("Modem/Configure modem", NULL, configure_modem_cb, this);
		menu->add("About/About ARPS", NULL, about_cb, this);
	}
	Fl_Tabs* tabs = new Fl_Tabs(0, BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT - BAR_HEIGHT - SPACING);
	{
		Fl_Group* msg_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "Messages");
		{
			unhandled = new MessageBrowser(this, false, SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			{
				unhandled->box(FL_BORDER_BOX);
				unhandled->end();
			}
			handled = new MessageBrowser(this, true, SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			{
				handled->box(FL_BORDER_BOX);
				handled->end();
			}
			check_message_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Check msgs");
			{
				check_message_button->user_data((void*)this);
				check_message_button->callback(poll_msg_cb);
			}
			Fl_Button* process_msg_button = new Fl_Button(BUTTON_WIDTH + 2 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Process");
			{
				process_msg_button->user_data((void*)this);
				process_msg_button->callback(process_msg_cb);
			}
			Fl_Button* unprocess_msg_button = new Fl_Button(2 * BUTTON_WIDTH + 3 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Un-process");
			{
				unprocess_msg_button->user_data((void*)this);
				unprocess_msg_button->callback(unprocess_msg_cb);
			}
			Fl_Button* user_terminal_button = new Fl_Button(3 * BUTTON_WIDTH + 4 * SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Terminal");
			{
				user_terminal_button->user_data((void*)this);
				user_terminal_button->callback(user_terminal_cb);
			}
		}
		msg_tab->end();
		Fl_Group* report_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "Reports");
		{
			unreceived_reports = new Fl_Multi_Browser(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			unreceived_reports->box(FL_BORDER_BOX);
			unreceived_reports->end();
			received_reports = new Fl_Multi_Browser(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			received_reports->box(FL_BORDER_BOX);
			received_reports->end();

			Fl_Button* reminder_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Remind 'em");
			{
				reminder_button->user_data((void*)this);
				reminder_button->callback(send_reminder_cb);
			}
		}
		report_tab->end();
		Fl_Group* english_tab = new Fl_Group(0, 2 * BAR_HEIGHT + SPACING, WINDOW_WIDTH, WINDOW_HEIGHT, "English");
		{
			unreceived_english = new Fl_Multi_Browser(SPACING, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			unreceived_english->box(FL_BORDER_BOX);
			unreceived_english->end();
			received_english = new Fl_Multi_Browser(SPACING * 2 + WINDOW_WIDTH / 2, 2 * BAR_HEIGHT + 2 * SPACING, WINDOW_WIDTH / 2 - 2 * SPACING, WINDOW_HEIGHT - 3 * BAR_HEIGHT - 4 * SPACING);
			received_english->box(FL_BORDER_BOX);
			received_english->end();

			Fl_Button* reminder_button = new Fl_Button(SPACING, WINDOW_HEIGHT - BAR_HEIGHT - SPACING, BUTTON_WIDTH, BAR_HEIGHT, "Remind 'em");
			{
				reminder_button->user_data((void*)this);
				reminder_button->callback(send_english_reminder_cb);
			}
		}
		english_tab->end();
	}
	tabs->end();
	window->end();
	window->show();
	update_msg_scroll();
	update_report_scrolls();
}

void Gui::run()
{
	Fl::lock();
	Fl::run();
	Fl::unlock();
}

void Gui::total_reports(Report::Type type)
{
	window->cursor(FL_CURSOR_WAIT);
	report_collection.total(type, &comp_list, type == Report::TYPE_ENGLISH ? english_date : report_date);
	window->cursor(FL_CURSOR_DEFAULT);
}

bool Gui::is_saved()
{
	return report_collection.is_saved() && msg_handler.is_saved() && referral_list.is_saved();
}

void Gui::save()
{
	window->cursor(FL_CURSOR_WAIT);
	clock_t start = clock();
	if (!report_collection.is_saved())
		report_collection.save_all();
	if (!msg_handler.is_saved())
		msg_handler.save(&file_manager);
	clock_t save = clock() - start;

	if (!referral_list.is_saved())
	{
		file_manager.files[FILE_REFERRALS].open(File::FILE_TYPE_OUTPUT);
		referral_list.save(&file_manager.files[FILE_REFERRALS], report_date);
		file_manager.files[FILE_REFERRALS].close();
	}
	window->cursor(FL_CURSOR_DEFAULT);
}

void Gui::load()
{
	file_manager.load(PATH_PATH_FILE);

	file_manager.files[L"CONFIG"].open(File::FILE_TYPE_INPUT);
	config.read_fields(&file_manager.files[L"CONFIG"]);
	file_manager.files[L"CONFIG"].close();

	report_wday = config.values[CONFIG_FIELD_REPORT_WDAY];
	english_wday = config.values[CONFIG_FIELD_ENGLISH_WDAY];
	stray_msg_handler = config.values[CONFIG_FIELD_STRAY_MSG_HANDLER];

	report_date = get_report_date_str(report_wday);
	english_date = get_report_date_str(english_wday);
	current_date = get_cur_date_str();

	comp_list.load(&file_manager);
	report_collection.init(L"../data/");
	report_collection.load(true, false);
	msg_handler.load(&file_manager);

	file_manager.files[FILE_REFERRALS].open(File::FILE_TYPE_INPUT);
	referral_list.load(&file_manager.files[FILE_REFERRALS]);
	file_manager.files[FILE_REFERRALS].close();
}

void Gui::update_report_scrolls()
{
	int i = 0;
	unreceived_reports->clear();
	received_reports->clear();
	unreceived_english->clear();
	received_english->clear();
	for (std::map<std::wstring, Area>::iterator it = comp_list.areas.begin(); it != comp_list.areas.end(); ++it, i++)
	{
		std::wstring id_str = report_date + ID_STR_SEPARATOR + it->second.area_name;
		if (report_collection.reports[Report::TYPE_REGULAR][ReportCollection::COMP].reports.count(id_str) > 0)
		{
			received_reports->add(tos(it->second.area_name).c_str(), (void*)&it->second);
		}
		else if (it->second.report_required)
		{
			unreceived_reports->add(tos(it->second.area_name).c_str(), (void*)&it->second);
		}

		id_str = english_date + ID_STR_SEPARATOR + L"0" + ID_STR_SEPARATOR + it->second.area_name;
		if (report_collection.reports[Report::TYPE_ENGLISH][ReportCollection::COMP].reports.count(id_str) > 0)
		{
			received_english->add(tos(it->second.area_name).c_str(), (void*)&it->second);
		}
		else if (it->second.english_required)
		{
			unreceived_english->add(tos(it->second.area_name).c_str(), (void*)&it->second);
		}

	}
	unreceived_reports->redraw();
	received_reports->redraw();
	unreceived_english->redraw();
	received_english->redraw();
}

void Gui::update_msg_scroll()
{
	unhandled->clear();
	handled->clear();
	for (size_t i = 0; i < msg_handler.msgs_unhandled.size(); i++)
	{
		std::wstring display_txt = get_browser_display_txt(msg_handler.msgs_unhandled[i]->get_sender_name() + DISPLAY_TEXT_SEPARATOR + msg_handler.msgs_unhandled[i]->get_contents());
		unhandled->add(tos(display_txt).c_str(), msg_handler.msgs_unhandled[i]);
	}

	for (size_t i = 0; i < msg_handler.msgs_handled.size(); i++)
	{
		std::wstring display_txt = get_browser_display_txt(msg_handler.msgs_handled[i]->get_sender_name() + DISPLAY_TEXT_SEPARATOR + msg_handler.msgs_handled[i]->get_contents());
		handled->add(tos(display_txt).c_str(), msg_handler.msgs_handled[i]);
	}
	unhandled->redraw();
	handled->redraw();
}

void Gui::send_reminder(Area* area)
{
	send_message(area->ph_number, L"Please remember to send in your key indicators.");
}

void Gui::poll_msgs()
{
	check_message_button->deactivate();

	Command cmd;
	cmd.is_check_msg_command = true;
	cmd.sub_cmds.push_back(SubCommand(L"AT+CMGL=4\r"));
	modem_interface->push_command(cmd);
}

void Gui::send_message(std::wstring dest_ph_number, std::wstring msg_contents)
{
	Message msg;
	msg.set_contents(msg_contents);
	msg.set_dest_number(dest_ph_number);
	std::vector<std::wstring> strings = msg.encode();
	
	for (size_t i = 0; i < strings.size(); i++)
	{
		Command cmd;
		std::wstringstream cmd_ss;
		cmd_ss << L"AT+CMGS=";
		cmd_ss << std::dec << (int)(strings[i].length() / 2 - 1);
		cmd_ss << L"\r";
		cmd.sub_cmds.push_back(SubCommand(cmd_ss.str()));

		cmd_ss.str(L"");
		cmd_ss.clear();
		cmd_ss << strings[i];
		cmd_ss << COMMAND_ESCAPE_CHAR;
		cmd.sub_cmds.push_back(SubCommand(cmd_ss.str()));

		cmd.n_times_to_try = 3;

		modem_interface->push_command(cmd);
	}
}

void Gui::delete_message_from_sim(int msg_cmg_id)
{
	std::wstringstream cmd_str(L"");
	cmd_str << std::dec << msg_cmg_id;
	std::wstring str_to_push = L"AT+CMGD=";
	str_to_push += cmd_str.str();
	str_to_push += L"\r";
	modem_interface->push_command(str_to_push);
}

// This function is called every time a command is passed to the modem and a response received.
// Returns true if a message was parsed from the resulting output, false otherwise.
bool Gui::completed_command_cb()
{
	bool found_msg = false;
	while (modem_interface->num_results() > 0)
	{
		Command cmd = modem_interface->pop_result();
		if (cmd.is_check_msg_command)
			check_message_button->activate();
		for (vector<SubCommand>::iterator it = cmd.sub_cmds.begin(); it != cmd.sub_cmds.end(); ++it)
		{
			wstring modem_str = it->result;
			if (modem_str.find(L"+CMGL:") != std::wstring::npos)
			{
				msg_handler.parse_messages(modem_str, this);
				found_msg = true;
			}
		}
	}
	return found_msg;
}

void Gui::process_msg(Message* msg)
{
	if (!msg->is_concatenated())
	{
		wstring msg_type = get_msg_key_val(msg->get_contents(), TYPE_KEY, ':', '\n');
		if (msg_type == TYPE_REPORT_STR)
		{
			ReportSheet* sheet = &report_collection.reports[Report::TYPE_REGULAR][ReportCollection::COMP];
			Report report;
			report.set_type(Report::TYPE_REGULAR);
			report.read_message(*msg, sheet->sheet_fields, report_date);
			sheet->add_report(report);

			int baptisms = _wtoi(report.report_values[REP_KEY_BAP].c_str());
			if (baptisms > 0)
			{
				send_message(msg->get_sender_number(), BAPTISM_RESPONSE_MSG);
				send_message(msg->get_sender_number(), BAPTISM_REPORT_TEMPLATE);

				for (int i = 0; i < baptisms; i++)
				{
					//Add a blank baptism record, so we can see if data is missing
					Report bap_record = report;
					bap_record.clear_values();
					bap_record.set_type(Report::TYPE_BAPTISM_RECORD);
					bap_record.sub_id = i;
					report_collection.reports[Report::TYPE_BAPTISM_RECORD][ReportCollection::COMP].add_report(report);
				}
			}
		}
		else if (msg_type == TYPE_ENGLISH_STR)
		{
			ReportSheet* sheet = &report_collection.reports[Report::TYPE_ENGLISH][ReportCollection::COMP];
			Report report;
			report.set_type(Report::TYPE_ENGLISH);
			report.read_message(*msg, sheet->sheet_fields, english_date);
			sheet->add_report(report);
		}
		else if (msg_type == TYPE_BAPTISM_STR)
		{
			ReportSheet* sheet = &report_collection.reports[Report::TYPE_BAPTISM_RECORD][ReportCollection::COMP];
			Report report;
			report.set_type(Report::TYPE_BAPTISM_RECORD);
			report.read_message(*msg, sheet->sheet_fields, report_date);
			sheet->add_report(report);
		}
		else if (msg_type == TYPE_REFERRAL_STR)
		{
			Referral referral;
			referral.read_message(*msg, current_date);
			referral.locate(&comp_list);
			if (!referral.found_dest())
				referral.dest_number = stray_msg_handler;	//Send it to the recorder!
			send_message(referral.dest_number, msg->get_contents());
			referral_list.push_back(referral);
		}
		else
		{
			bool found = false;
			for (ReferralList::iterator it = referral_list.begin(); it != referral_list.end() && !found; ++it)
			{
				if (it->dest_number == msg->get_sender_number())
				{
					std::wstring result = get_msg_key_val(msg->get_contents(), it->name, ':', '\n');
					if (!result.empty())
					{
						strip_chars(result, L"\t\n");
						it->contact_state = result;
						found = true;
					}
				}
			}
			if(!found)
				send_message(stray_msg_handler, msg->get_contents());	//Send it to the recorder!
		}
	}
	msg_handler.msgs_handled.push_back(msg);
	for (size_t i = 0, erased = 0; i < msg_handler.msgs_unhandled.size() && erased == 0; i++)
	{
		if (msg_handler.msgs_unhandled[i] == msg)
		{
			msg_handler.msgs_unhandled.erase(msg_handler.msgs_unhandled.begin() + i);
			erased = true;
		}
	}
	msg_handler.changed = true;
}

void Gui::unprocess_msg(Message* msg)
{
	msg_handler.msgs_unhandled.push_back(msg);
	for (size_t i = 0, found = 0; i < msg_handler.msgs_handled.size() && found == 0; i++)
	{
		if (msg_handler.msgs_handled[i] == msg)
		{
			msg_handler.msgs_handled.erase(msg_handler.msgs_handled.begin() + i);
			found = true;
		}
	}
	msg_handler.changed = true;
}

void Gui::configure_modem()
{
	modem_interface->push_command(L"AT\r");
	modem_interface->push_command(L"ATE0\r");
	modem_interface->push_command(L"AT+CPIN=\"0000\"\r");
	modem_interface->push_command(L"AT+CMGF=0\r");
	modem_interface->push_command(L"AT+CNMI=0,0,0,0\r");
}