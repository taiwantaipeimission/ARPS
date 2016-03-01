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

MessageHandler::MessageHandler()
{
	int x = 0;
}


MessageHandler::~MessageHandler()
{
	for (int i = 0; i < msgs_handled.size(); i++)
		delete msgs_handled[i];
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
		decode_msg(msg, msg_str, &gui->comp_list);
		
		if (msg->concatenated)
		{
			unsigned int id = msg->concat_num_msgs | (msg->concat_refnum << 8);
			msgs_fragment[id].push_back(msg);
		}
		else
		{
			msgs_unhandled.push_back(msg);
		}
		start = end + 1;
		end = raw_str.find(L"+CMGL:", start + 1) - 1;
		gui->delete_message_from_sim(msg->cmgl_ids[0]);
	}

	//Attempt to piece together concatenated messages
	for (std::map<unsigned int, std::vector<Message*>>::iterator it = msgs_fragment.begin(); it != msgs_fragment.end(); )
	{
		std::vector<int> correct_order;					//List of the indeces of message fragments for this concat_id, arranged in proper order
		int num_msgs = it->second[0]->concat_num_msgs;
		correct_order.assign(num_msgs, -1);				//Fill with -1s so we can see if any msgs are missing
		for (size_t i = 0; i < it->second.size(); i++)
		{
			correct_order[it->second[i]->concat_index - 1] = i;					//Concat indeces inside msg start from 1, so subtract 1
		}
		if (std::count(correct_order.begin(), correct_order.end(), -1) == 0)	//All sub-msgs found that make up this concat msg
		{
			for (unsigned int i = 1; i < correct_order.size(); i++)
			{
				it->second[correct_order[0]]->contents += it->second[correct_order[i]]->contents;
				it->second[correct_order[0]]->cmgl_ids.push_back(it->second[correct_order[i]]->cmgl_ids[0]);
			}
			it->second[correct_order[0]]->concatenated = false;
			msgs_unhandled.push_back(it->second[correct_order[0]]);
			it = msgs_fragment.erase(it);
		}
		else
		{
			++it;
		}
	}

	return;
}

/*
void MessageHandler::process_msg(Message* msg, Gui* gui)
{
		bool processed_this_msg = false;
		if (!msg->concatenated)
		{
			if (msg->type == TYPE_REPORT)
			{
				Report report;
				report.set_type(Report::TYPE_REGULAR);
				report.read_message(*msg, gui->report_date);
				report_collection->reports[Report::TYPE_REGULAR][ReportCollection::COMP].add_report(report);

				processed_this_msg = true;

				int baptisms = _wtoi(report.report_values[REP_KEY_BAP].c_str());
				if (baptisms > 0)
				{
					terminal->send_message(msg->sender_number, BAPTISM_RESPONSE_MSG);
					terminal->send_message(msg->sender_number, BAPTISM_REPORT_TEMPLATE);
				}
			}
			else if (msg->type == TYPE_REPORT_ENGLISH)
			{
				Report report;
				report.set_type(Report::TYPE_ENGLISH);
				report.read_message(*msg, english_date);
				report_collection->reports[Report::TYPE_ENGLISH][ReportCollection::COMP].add_report(report);

				processed_this_msg = true;
			}
			else if (msg->type == TYPE_REPORT_BAPTISM)
			{
				Report report;
				report.set_type(Report::TYPE_BAPTISM_RECORD);
				report.read_message(*msg, date);
				report_collection->reports[Report::TYPE_BAPTISM_RECORD][ReportCollection::COMP].add_report(report);

				int choice = _wtoi(report.report_values[REP_KEY_BAP_SOURCE].c_str());
				Report bap_source = report;
				bap_source.report_values.clear();
				bap_source.set_type(Report::TYPE_BAPTISM_SOURCE);
				for (std::vector<std::wstring>::iterator it = bap_source.key_list.begin(); it != bap_source.key_list.end(); ++it)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(*it, L"0"));	//Fill with zeros
				if (choice == 1)
					bap_source.report_values[REP_KEY_BAP_MISS_FIND] = L"1";
				else if (choice == 2)
					bap_source.report_values[REP_KEY_BAP_LA_REF] = L"1";
				else if (choice == 3)
					bap_source.report_values[REP_KEY_BAP_RC_REF] = L"1";
				else if (choice == 4)
					bap_source.report_values[REP_KEY_BAP_MEM_REF] = L"1";
				else if (choice == 5)
					bap_source.report_values[REP_KEY_BAP_ENGLISH] = L"1";
				else if (choice == 6)
					bap_source.report_values[REP_KEY_BAP_TOUR] = L"1";

				report_collection->reports[Report::TYPE_BAPTISM_SOURCE][ReportCollection::COMP].add_report(bap_source);

				processed_this_msg = true;
			}
			else if (msg->type == TYPE_REFERRAL)
			{
				Referral referral;
				referral.read_message(*msg);
				referral.locate(comp_list);
				if (referral.found_dest())
				{
					terminal->send_message(referral.dest_number, msg->contents);
					referral_file->file << referral.print(date) << std::endl;
				}
				else
				{
					terminal->send_message(LOST_REFERRAL_HANDLER, msg->contents);	//Send it to the recorder!
					referral_file->file << referral.print(date) << std::endl;
				}

				processed_this_msg = true;
			}
			else if (msg->type == TYPE_UNKNOWN)
			{
				terminal->send_message(LOST_REFERRAL_HANDLER, msg->contents);	//Send it to the recorder!
				processed_this_msg = true;
			}
		}
		if (processed_this_msg)
		{
			msgs_handled.push_back(msg);
			for (int i = 0, erased = 0; i < msgs_unhandled.size() && erased == 0; i++)
			{
				if (msgs_unhandled[i] == msg)
				{
					msgs_unhandled.erase(msgs_unhandled.begin() + i);
					erased = true;
				}
			}
		}
}

void MessageHandler::unprocess_msg(Message* msg)
{
	msgs_unhandled.push_back(msg);
	for (int i = 0, found = 0; i < msgs_handled.size() && found == 0; i++)
	{
		if (msgs_handled[i] == msg)
		{
			msgs_handled.erase(msgs_handled.begin() + i);
			found = true;
		}
	}
}*/

void MessageHandler::load(File* file, bool handled)
{
	if (file->file.good())
	{
		do
		{
			std::wstring line;
			std::getline(file->file, line, MSG_SEPARATOR);
			if (!line.empty())
			{
				Message* msg = new Message();
				read_filed_msg(msg, line);
				if (msg->concatenated)
				{
					msgs_fragment[msg->concat_refnum].push_back(msg);
				}
				else
				{
					if (handled)
						msgs_handled.push_back(msg);
					else
						msgs_unhandled.push_back(msg);
				}
			}
		} while (file->file.good());
	}
}

void MessageHandler::save(File* file, bool handled)
{
	for (std::vector<Message*>::iterator it = (handled ? msgs_handled.begin() : msgs_unhandled.begin()); it != (handled ? msgs_handled.end() : msgs_unhandled.end()); ++it)
	{
		file->file << write_filed_msg(*it) << MSG_SEPARATOR;
	}
}

void MessageHandler::save(FileManager* file_manager)
{
	file_manager->files[FILE_MESSAGES_HANDLED].open(File::FILE_TYPE_OUTPUT);
	save(&file_manager->files[FILE_MESSAGES_HANDLED], true);
	file_manager->files[FILE_MESSAGES_HANDLED].close();

	file_manager->files[FILE_MESSAGES_UNHANDLED].open(File::FILE_TYPE_OUTPUT);
	save(&file_manager->files[FILE_MESSAGES_UNHANDLED], false);
	file_manager->files[FILE_MESSAGES_UNHANDLED].close();
}

void MessageHandler::load(FileManager* file_manager)
{
	file_manager->files[FILE_MESSAGES_HANDLED].open(File::FILE_TYPE_INPUT);
	load(&file_manager->files[FILE_MESSAGES_HANDLED], true);
	file_manager->files[FILE_MESSAGES_HANDLED].close();

	file_manager->files[FILE_MESSAGES_UNHANDLED].open(File::FILE_TYPE_INPUT);
	load(&file_manager->files[FILE_MESSAGES_UNHANDLED], false);
	file_manager->files[FILE_MESSAGES_UNHANDLED].close();
}