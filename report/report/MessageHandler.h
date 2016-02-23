#pragma once

class Terminal;
class ReportCollection;
class Message;
class CompList;
#include "File.h"

#include <vector>
#include <map>

class MessageHandler
{
public:
	std::map<unsigned int, std::vector<Message>> msgs_fragment;	//All fragments of concatenated messages, keyed by their concat_num_msgs | (concat_refnum << 16)
	std::vector<Message> msgs_unhandled;
	std::vector<Message> msgs_handled;
	File handled_file;
	File unhandled_file;

	MessageHandler();
	virtual ~MessageHandler();

	void parse_messages(Terminal* terminal, std::wstring raw_str, CompList* comp_list);
	void process_messages(Terminal* terminal, ReportCollection* report_collection, CompList* comp_list, std::wstring date, std::wstring english_date);
	void save(File* file, bool handled);
	void load(File* file, bool handled);
	void save();
	void load();
};

