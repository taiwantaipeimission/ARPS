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

#include "codes.h"
#include "MessageHandler.h"
#include "CompList.h"
#include "Message.h"
#include "Report.h"
#include "Terminal.h"
#include "ReportCollection.h"
#include "Referral.h"
#include "File.h"
#include "Gui.h"
#include "utility.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

MessageHandler::MessageHandler()
{
	int x = 0;
	changed = false;
}


MessageHandler::~MessageHandler()
{
	for (std::vector<Message*>::iterator it = msgs_handled.begin(); it != msgs_handled.end(); ++it)
		delete *it;
	for (std::vector<Message*>::iterator it = msgs_unhandled.begin(); it != msgs_unhandled.end(); ++it)
		delete *it;
}

void MessageHandler::parse_messages(std::wstring raw_str, Gui* gui)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find(L"+CMGL:");
	int end = raw_str.find(L"+CMGL:", start + 1) - 1;
	while (start != std::wstring::npos)
	{
		Message* msg = new Message();
		std::wstring msg_str = raw_str.substr(start, end - start + 1);	//String containing message info as well as CMGL response of the modem
		msg->decode(msg_str, &gui->comp_list);
		
		if (msg->is_concatenated())
		{
			unsigned int id = msg->get_concat_num_msgs() | (msg->get_concat_refnum() << 8);
			msgs_fragment[id].push_back(msg);
		}
		else
		{
			msgs_unhandled.push_back(msg);
		}
		start = end + 1;
		end = raw_str.find(L"+CMGL:", start + 1) - 1;
		if(msg->get_cmgl_ids().size() > 0)
			gui->delete_message_from_sim(msg->get_cmgl_ids()[0]);
		changed = true;
	}

	//Attempt to piece together concatenated messages
	for (std::map<unsigned int, std::vector<Message*>>::iterator it = msgs_fragment.begin(); it != msgs_fragment.end(); )
	{
		std::vector<int> correct_order;					//List of the indeces of message fragments for this concat_id, arranged in proper order
		int num_msgs = it->second[0]->get_concat_num_msgs();
		correct_order.assign(num_msgs, -1);				//Fill with -1s so we can see if any msgs are missing
		for (size_t i = 0; i < it->second.size(); i++)
		{
			size_t index = it->second[i]->get_concat_index() - 1;					//Concat indeces inside msg start from 1, so subtract 1
			if (index < correct_order.size())
				correct_order[index] = i;
		}
		if (std::count(correct_order.begin(), correct_order.end(), -1) == 0)	//All sub-msgs found that make up this concat msg
		{
			for (unsigned int i = 1; i < correct_order.size(); i++)
			{
				it->second[correct_order[0]]->app_contents(it->second[correct_order[i]]->get_contents());
				it->second[correct_order[0]]->push_cmgl_id(it->second[correct_order[i]]->get_cmgl_ids()[0]);
			}
			it->second[correct_order[0]]->set_concatenated(false);
			msgs_unhandled.push_back(it->second[correct_order[0]]);
			it = msgs_fragment.erase(it);
			changed = true;
		}
		else
		{
			++it;
		}
	}

	return;
}

void MessageHandler::load(File* file, MessageStorageType type)
{
	Document document;
	if (file->file.good() && !document.Parse<0>(tos(file->extract_contents()).c_str()).HasParseError())
	{
		assert(document.IsArray());
		for (SizeType i = 0; i < document.Size(); i++)
		{
			const Value& m = document[i];
			Message* msg = new Message();
			msg->read(m);
			if (msg->is_concatenated())
			{
				msgs_fragment[msg->get_concat_refnum()].push_back(msg);
			}
			else
			{
				bool duplicate = false;
				std::vector<Message*>* target_vector = NULL;

				if (type == HANDLED)
					target_vector = &msgs_handled;
				else if (type == UNHANDLED)
					target_vector = &msgs_unhandled;
				else if (type == OUTBOX)
					target_vector = &msg_outbox;

				for (std::vector<Message*>::iterator it = target_vector->begin(); it != target_vector->end() && !duplicate; ++it)
				{
					if ((*it)->get_sender_name() == msg->get_sender_name() && (*it)->get_contents() == msg->get_contents())
						duplicate = true;
				}
				if (!duplicate)
				{
					target_vector->push_back(msg);
				}
			}
		}
		changed = false;
	}
}

void MessageHandler::save(File* file, MessageStorageType type)
{
	Document d;
	d.SetArray();
	std::vector<Message*>* target_vector = NULL;
	if (type == HANDLED)
		target_vector = &msgs_handled;
	else if (type == UNHANDLED)
		target_vector = &msgs_unhandled;
	else if (type == OUTBOX)
		target_vector = &msg_outbox;

	for (std::vector<Message*>::iterator it = target_vector->begin(); it != target_vector->end(); ++it)
		(*it)->write(&d);

	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	d.Accept(writer);
	string str = buffer.GetString();
	file->file << tow(str);
}

bool MessageHandler::is_saved()
{
	return !changed;
}

void MessageHandler::save(FileManager* file_manager)
{
	file_manager->files[FILE_MESSAGES_HANDLED].open(File::FILE_TYPE_OUTPUT);
	save(&file_manager->files[FILE_MESSAGES_HANDLED], HANDLED);
	file_manager->files[FILE_MESSAGES_HANDLED].close();

	file_manager->files[FILE_MESSAGES_UNHANDLED].open(File::FILE_TYPE_OUTPUT);
	save(&file_manager->files[FILE_MESSAGES_UNHANDLED], UNHANDLED);
	file_manager->files[FILE_MESSAGES_UNHANDLED].close();

	file_manager->files[FILE_MESSAGES_OUTBOX].open(File::FILE_TYPE_OUTPUT);
	save(&file_manager->files[FILE_MESSAGES_OUTBOX], OUTBOX);
	file_manager->files[FILE_MESSAGES_OUTBOX].close();

	changed = false;
}

void MessageHandler::load(FileManager* file_manager)
{
	file_manager->files[FILE_MESSAGES_HANDLED].open(File::FILE_TYPE_INPUT);
	load(&file_manager->files[FILE_MESSAGES_HANDLED], HANDLED);
	file_manager->files[FILE_MESSAGES_HANDLED].close();

	file_manager->files[FILE_MESSAGES_UNHANDLED].open(File::FILE_TYPE_INPUT);
	load(&file_manager->files[FILE_MESSAGES_UNHANDLED], UNHANDLED);
	file_manager->files[FILE_MESSAGES_UNHANDLED].close();

	file_manager->files[FILE_MESSAGES_OUTBOX].open(File::FILE_TYPE_INPUT);
	load(&file_manager->files[FILE_MESSAGES_OUTBOX], OUTBOX);

	changed = false;
}

void MessageHandler::add_message(Message* msg, MessageStorageType type)
{
	if (type == HANDLED)
		msgs_handled.push_back(msg);
	else if (type == UNHANDLED)
		msgs_unhandled.push_back(msg);
	else if (type == OUTBOX)
		msg_outbox.push_back(msg);

	changed = true;
}

void MessageHandler::erase_message(Message* msg, MessageStorageType type)
{
	if (type == HANDLED)
	{
		size_t before_size = msgs_handled.size();
		msgs_handled.erase(std::remove(msgs_handled.begin(), msgs_handled.end(), msg), msgs_handled.end());
		changed = msgs_handled.size() != before_size;
	}
	else if(type == UNHANDLED)
	{
		size_t before_size = msgs_unhandled.size();
		msgs_unhandled.erase(std::remove(msgs_unhandled.begin(), msgs_unhandled.end(), msg), msgs_unhandled.end());
		changed = msgs_unhandled.size() != before_size;
	}
	else if (type == OUTBOX)
	{
		size_t before_size = msg_outbox.size();
		msg_outbox.erase(std::remove(msg_outbox.begin(), msg_outbox.end(), msg), msg_outbox.end());
	}
}

std::vector<Message*> const * MessageHandler::get_messages(MessageStorageType type)
{
	if (type == HANDLED)
		return &msgs_handled;
	else if (type == UNHANDLED)
		return &msgs_unhandled;
	else if (type == OUTBOX)
		return &msg_outbox;
	else
		return NULL;
}