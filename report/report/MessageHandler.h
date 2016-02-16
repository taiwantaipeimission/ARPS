#pragma once

class Terminal;
class ReportCollection;
class Message;
class CompList;

#include <vector>
#include <map>

class MessageHandler
{
public:
	std::map<int, std::vector<Message>> msgs_fragment;	//All fragments of concatenated messages, keyed by their concat_index
	std::vector<Message> msgs_unhandled;
	std::vector<Message> msgs_handled;

	MessageHandler();
	virtual ~MessageHandler();

	void parse_messages(Terminal* terminal, std::wstring raw_str, CompList* comp_list);
	void process_messages(Terminal* terminal, ReportCollection* report_collection, CompList* comp_list, std::wstring date, std::wstring english_date);
	void read_filed_msgs(std::wistream& input, bool handled);
	void write_filed_msgs(std::wostream& output, bool handled);
};

