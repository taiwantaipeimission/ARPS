#include "Gui.h"
#include "utility.h"
#include "codes.h"
#include "Area.h"

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

void save_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->save();
}

void quit_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	if (gui->saved || fl_ask("Unsaved changes! Are you sure you want to quit?"))
		exit(0);
}

void user_terminal_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->terminal.init_user();
	gui->run_terminal_commands();
}

void send_reminder_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->terminal.send_reminders();
	gui->run_terminal_commands();
}

void send_english_reminder_cb(Fl_Widget* wg, void*ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->terminal.send_reminders(true);
	gui->run_terminal_commands();
}

void check_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	gui->terminal.init_auto();
	gui->run_terminal_commands();
	gui->update_msg_scroll();
	gui->update_report_scrolls();
	gui->saved = false;
}

void process_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int j = 1; j <= gui->unhandled->size(); j++)
	{
		if (gui->unhandled->selected(j))
		{
			gui->msg_handler.process_msg((Message*)gui->unhandled->data(j), &gui->terminal, &gui->report_collection, &gui->comp_list, &gui->file_manager.files[FILE_REFERRALS], gui->report_date, gui->english_date);
		}
	}
	gui->run_terminal_commands();
	gui->update_msg_scroll();
	gui->saved = false;
}

void unprocess_msg_cb(Fl_Widget* wg, void* ptr)
{
	Gui* gui = (Gui*)ptr;
	for (int i = 1; i <= gui->handled->size(); i++)
	{
		if (gui->handled->selected(i))
		{
			gui->msg_handler.unprocess_msg((Message*)gui->handled->data(i));
		}
	}
	gui->update_msg_scroll();
	gui->saved = false;
}

void timer_cb(void* ptr)
{
	Gui* gui = (Gui*)ptr;
	check_msg_cb(NULL, ptr);
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

void Gui::init()
{
	auto_check_s = 300.0f;

	terminal.init(report_date, english_date, &modem, &report_collection, &comp_list, &file_manager.files[L"OUTPUT"]);

	Fl::add_timeout(auto_check_s, timer_cb, this);
	Fl_Window* window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT);
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, WINDOW_WIDTH, BAR_HEIGHT);
	{
		menu->add("File/Save", FL_CTRL + 's', save_cb, this);
		menu->add("File/Quit", FL_CTRL + 'q', quit_cb, this);
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
				check_msg_button->callback(check_msg_cb);
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
	Fl::run();
}

void Gui::save()
{
	report_collection.total_all(&comp_list, report_date, english_date);
	report_collection.save_all();
	msg_handler.save(&file_manager);
	saved = true;
}

void Gui::load()
{
	file_manager.load(PATH_PATH_FILE);

	comp_list.load(&file_manager);
	report_collection.init(L"../data/");
	report_collection.load_all();
	msg_handler.load(&file_manager);

	file_manager.files[L"CONFIG"].open(File::FILE_TYPE_INPUT);
	config.read_fields(&file_manager.files[L"CONFIG"]);
	file_manager.files[L"CONFIG"].close();

	report_wday = config.values[CONFIG_FIELD_REPORT_WDAY];
	english_wday = config.values[CONFIG_FIELD_ENGLISH_WDAY];

	report_date = get_report_date_str(report_wday);
	english_date = get_report_date_str(english_wday);
	saved = true;
}

void Gui::run_terminal_commands()
{
	clock_t start = clock();
	clock_t end = start;
	while (terminal.update(double(end - start) / (double)CLOCKS_PER_SEC * 1000.0f, &msg_handler))
	{
		start = end;
		end = clock();
	}
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
			received_reports->add(tos(it->second.area_name).c_str());
		}
		else if (it->second.report_required)
		{
			unreceived_reports->add(tos(it->second.area_name).c_str());
		}

		id_str = english_date + ID_STR_SEPARATOR + L"0" + ID_STR_SEPARATOR + it->second.area_name;
		if (report_collection.reports[Report::TYPE_ENGLISH][ReportCollection::COMP].reports.count(id_str) > 0)
		{
			received_english->add(tos(it->second.area_name).c_str());
		}
		else if (it->second.english_required)
		{
			unreceived_english->add(tos(it->second.area_name).c_str());
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