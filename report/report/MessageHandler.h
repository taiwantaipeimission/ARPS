#pragma once

class Terminal;
class ReportCollection;
class Message;

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

	void parse_messages(std::wstring raw_str, CompList* comp_list);
	void process_messages(Terminal* terminal, ReportCollection* report_collection, CompList* comp_list, std::wstring date, std::wstring english_date)
};

