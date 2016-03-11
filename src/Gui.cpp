#include "Gui.h"
#include "utility.h"
#include "codes.h"
#include "Area.h"
#include "Referral.h"

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
	str.resize(100);
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
	//gui->terminal->init_user();
	//gui->run_terminal_commands();
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
			std::wstring msg_text = L"Just checking: this phone belongs to the "
				+ it->second.area_name
				+ L", in the "
				+ it->second.zone_name
				+ L" zone, in the "
				+ it->second.district_name
				+ L" district.\nIf not, please call the Operations Assistant.";
			if(it->second.area_name == L"OFFICE_E")
				gui->send_message(it->first, msg_text);
		}
	}
}

void poll_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->poll_msgs();
}

void check_msg_cb(void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->check_msgs();
	gui->update_msg_scroll();
	gui->update_report_scrolls();
}

void process_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int j = 1; j <= gui->unhandled->size(); j++)
	{
		if (gui->unhandled->selected(j))
		{
			gui->process_msg((Message*)gui->unhandled->data(j));
		}
	}
	gui->update_msg_scroll();
	gui->update_report_scrolls();
}

void unprocess_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int i = 1; i <= gui->handled->size(); i++)
	{
		if (gui->handled->selected(i))
		{
			gui->unprocess_msg((Message*)gui->handled->data(i));
		}
	}
	gui->update_msg_scroll();
}

void timer_cb(void* ptr)
{
	Gui* gui = (Gui*)ptr;
	poll_msg_cb(NULL, ptr);
	for (int i = 1; i <= gui->unhandled->size(); i++)
		gui->unhandled->select(i);
	process_msg_cb(NULL, ptr);
	Fl::repeat_timeout(gui->auto_check_s, timer_cb, ptr);
}

void select_all_cb(Fl_Widget* wg, void* ptr)
{

}

Gui::Gui()
{
}


Gui::~Gui()
{
}

void Gui::init(ModemData* modem_data_in)
{
	modem_data = modem_data_in;
	auto_check_s = 300.0f;
	file_manager.files[FILE_OUTPUT].append = true;
	file_manager.files[FILE_OUTPUT].open(File::FILE_TYPE_OUTPUT);

	Fl_PNG_Image* image = new Fl_PNG_Image("../res/logo.png");
	Fl::add_timeout(auto_check_s, timer_cb, this);
	window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "ARPS");
	window->default_icon(image);
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, WINDOW_WIDTH, BAR_HEIGHT);
	{
		menu->add("File/Save", FL_CTRL + 's', save_cb, this);
		menu->add("File/Quit", FL_CTRL + 'q', quit_cb, this);
		menu->add("Edit/Total reports", FL_CTRL + 'r', total_report_cb, this);
		menu->add("Edit/Total English", FL_CTRL + 'e', total_english_cb, this);
		menu->add("Edit/Total baptism source", FL_CTRL + 'b', total_baptism_source_cb, this);
		menu->add("Areas/Send verification text", NULL, send_verify_text_cb, this);
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
				check_msg_button->user_data((void*)this);
				check_msg_button->callback(poll_msg_cb);
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

	comp_list.load(&file_manager);
	report_collection.init(L"../data/");
	report_collection.load_all();
	msg_handler.load(&file_manager);

	file_manager.files[FILE_REFERRALS].open(File::FILE_TYPE_INPUT);
	referral_list.load(&file_manager.files[FILE_REFERRALS]);
	file_manager.files[FILE_REFERRALS].close();

	file_manager.files[L"CONFIG"].open(File::FILE_TYPE_INPUT);
	config.read_fields(&file_manager.files[L"CONFIG"]);
	file_manager.files[L"CONFIG"].close();

	report_wday = config.values[CONFIG_FIELD_REPORT_WDAY];
	english_wday = config.values[CONFIG_FIELD_ENGLISH_WDAY];

	report_date = get_report_date_str(report_wday);
	english_date = get_report_date_str(english_wday);
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
			unreceived_reports->add(tos(it->second.area_name).c_str());
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
		std::wstring display_txt = get_browser_display_txt(msg_handler.msgs_unhandled[i]->sender_name + DISPLAY_TEXT_SEPARATOR + msg_handler.msgs_unhandled[i]->contents);
		unhandled->add(tos(display_txt).c_str(), msg_handler.msgs_unhandled[i]);
	}

	for (size_t i = 0; i < msg_handler.msgs_handled.size(); i++)
	{
		std::wstring display_txt = get_browser_display_txt(msg_handler.msgs_handled[i]->sender_name + DISPLAY_TEXT_SEPARATOR + msg_handler.msgs_handled[i]->contents);
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
	modem_data->clear_command_stream();
	modem_data->push_command(L"AT");
	modem_data->push_command(L"ATE0");
	modem_data->push_command(L"AT+CMGF=0");
	modem_data->push_command(L"AT+CMGL=4");
}

void Gui::send_message(std::wstring dest_ph_number, std::wstring msg_contents)
{
	Message msg;
	msg.contents = msg_contents;
	msg.dest_number = dest_ph_number;
	std::vector<std::wstring> strings = encode_msg(&msg);
	for (size_t i = 0; i < strings.size(); i++)
	{
		std::wstringstream cmd;
		cmd << L"AT+CMGS=";
		cmd << std::dec << (int)(strings[i].length() / 2 - 1);
		modem_data->push_command(cmd.str());

		cmd.str(L"");
		cmd.clear();
		cmd << strings[i];
		modem_data->push_command(cmd.str(), COMMAND_ESCAPE_CHAR);
	}
}

void Gui::delete_message_from_sim(int msg_cmg_id)
{
	std::wstringstream cmd_str(L"");
	cmd_str << std::dec << msg_cmg_id;
	std::wstring str_to_push = L"AT+CMGD=";
	str_to_push += cmd_str.str();
	modem_data->push_command(str_to_push);
}

void Gui::check_msgs()
{
	while (modem_data->get_modem_strs().size() > 0)
	{
		std::wstring modem_str = modem_data->pop_modem_str();
		if (modem_str.find(L"+CMGL:") != std::wstring::npos)
		{
			msg_handler.parse_messages(modem_str, this);
		}
	}
}

void Gui::process_msg(Message* msg)
{
	bool processed_this_msg = false;
	if (!msg->concatenated)
	{
		if (msg->type == TYPE_REPORT)
		{
			Report report;
			report.set_type(Report::TYPE_REGULAR);
			report.read_message(*msg, report_date);
			report_collection.reports[Report::TYPE_REGULAR][ReportCollection::COMP].add_report(report);

			processed_this_msg = true;

			int baptisms = report.int_values[REP_KEY_BAP];
			if (baptisms > 0)
			{
				send_message(msg->sender_number, BAPTISM_RESPONSE_MSG);
				send_message(msg->sender_number, BAPTISM_REPORT_TEMPLATE);

				for (int i = 0; i < baptisms; i++)
				{
					//Add a blank baptism record, so we can see if data is missing
					Report bap_record = report;
					bap_record.clear_values();
					bap_record.set_type(Report::TYPE_BAPTISM_SOURCE);
					bap_record.sub_id = i;
					report_collection.reports[Report::TYPE_BAPTISM_RECORD][ReportCollection::COMP].add_report(report);
				}
			}
		}
		else if (msg->type == TYPE_REPORT_ENGLISH)
		{
			Report report;
			report.set_type(Report::TYPE_ENGLISH);
			report.read_message(*msg, english_date);
			report_collection.reports[Report::TYPE_ENGLISH][ReportCollection::COMP].add_report(report);

			processed_this_msg = true;
		}
		else if (msg->type == TYPE_REPORT_BAPTISM)
		{
			Report report;
			report.set_type(Report::TYPE_BAPTISM_RECORD);
			report.read_message(*msg, report_date);
			report_collection.reports[Report::TYPE_BAPTISM_RECORD][ReportCollection::COMP].add_report(report);

			int choice = report.int_values[REP_KEY_BAP_SOURCE];
			Report bap_source = report;
			bap_source.clear_values();
			bap_source.set_type(Report::TYPE_BAPTISM_SOURCE);
			for (std::vector<std::wstring>::iterator it = bap_source.int_key_list.begin(); it != bap_source.int_key_list.end(); ++it)
				bap_source.int_values[*it] = 0;	//Fill with zeros
			if (choice == 1)
				bap_source.int_values[REP_KEY_BAP_MISS_FIND] = 1;
			else if (choice == 2)
				bap_source.int_values[REP_KEY_BAP_LA_REF] = 1;
			else if (choice == 3)
				bap_source.int_values[REP_KEY_BAP_RC_REF] = 1;
			else if (choice == 4)
				bap_source.int_values[REP_KEY_BAP_MEM_REF] = 1;
			else if (choice == 5)
				bap_source.int_values[REP_KEY_BAP_ENGLISH] = 1;
			else if (choice == 6)
				bap_source.int_values[REP_KEY_BAP_TOUR] = 1;

			if (report_collection.reports[Report::TYPE_BAPTISM_SOURCE][ReportCollection::COMP].reports.count(bap_source.get_id_str()) > 0)
			{
				//Add on to the existing baptism source report
				report_collection.reports[Report::TYPE_BAPTISM_SOURCE][ReportCollection::COMP].reports[bap_source.get_id_str()] += bap_source;
			}
			else
			{
				//Create a new baptism source report
				report_collection.reports[Report::TYPE_BAPTISM_SOURCE][ReportCollection::COMP].add_report(bap_source);
			}

			processed_this_msg = true;
		}
		else if (msg->type == TYPE_REFERRAL)
		{
			Referral referral;
			referral.read_message(*msg);
			referral.locate(&comp_list);
			if (!referral.found_dest())
				referral.dest_number = LOST_REFERRAL_HANDLER;	//Send it to the recorder!
			send_message(referral.dest_number, msg->contents);
			referral_list.push_back(referral);

			processed_this_msg = true;
		}
		else if (msg->type == TYPE_UNKNOWN)
		{
			bool found = false;
			for (ReferralList::iterator it = referral_list.begin(); it != referral_list.end() && !found; ++it)
			{
				if (it->dest_number == msg->sender_number)
				{
					std::wstring result = get_msg_key_val(msg->contents, it->name, ':', '\n');
					if (!result.empty())
					{
						strip_chars(result, L"\t\n");
						it->contact_state = result;
						found = true;
					}
				}
			}
			if(!found)
				send_message(LOST_REFERRAL_HANDLER, msg->contents);	//Send it to the recorder!
			processed_this_msg = true;
		}
	}
	if (processed_this_msg)
	{
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