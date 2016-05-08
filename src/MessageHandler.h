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
private:

	std::map<unsigned int, std::vector<Message*>> msgs_fragment;	//All fragments of concatenated messages, keyed by their concat_num_msgs | (concat_refnum << 8)
	std::vector<Message*> msgs_unhandled;
	std::vector<Message*> msgs_handled;
	std::vector<Message*> msg_outbox;

	bool changed;

public:

	enum MessageStorageType
	{
		UNHANDLED,
		HANDLED,
		OUTBOX
	};

	MessageHandler();
	virtual ~MessageHandler();

	bool is_saved();
	void save(File* file, MessageStorageType type);
	void load(File* file, MessageStorageType type, Gui* gui);
	void save(FileManager* file_manager);
	void load(FileManager* file_manager, Gui* gui);

	void parse_messages(std::wstring raw_str, Gui* gui);

	void add_message(Message* msg, MessageStorageType type);
	void erase_message(Message* msg, MessageStorageType type);
	void move_message(Message* msg, MessageStorageType from, MessageStorageType to, bool copy = false);

	std::vector<Message*>* get_messages(MessageStorageType type);
};

