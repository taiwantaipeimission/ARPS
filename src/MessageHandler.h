#pragma once

class Terminal;
class ReportCollection;
class CompList;
class Gui;
#include "File.h"
#include "Message.h"
#include "FileManager.h"

#include <vector>
#include <map>

class MessageHandler
{
public:
	std::map<unsigned int, std::vector<Message*>> msgs_fragment;	//All fragments of concatenated messages, keyed by their concat_num_msgs | (concat_refnum << 8)
	std::vector<Message*> msgs_unhandled;
	std::vector<Message*> msgs_handled;

	MessageHandler();
	virtual ~MessageHandler();

	void save(File* file, bool handled);
	void load(File* file, bool handled);
	void save(FileManager* file_manager);
	void load(FileManager* file_manager);

	void MessageHandler::parse_messages(std::wstring raw_str, Gui* gui);
};

