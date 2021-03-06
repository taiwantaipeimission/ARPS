#include "MessageHandler.h"
#include "CompList.h"
#include "Message.h"
#include "Report.h"
#include "Terminal.h"
#include "ReportCollection.h"
#include "Referral.h"
#include "File.h"

MessageHandler::MessageHandler()
{
	handled_file.filepath = L"messages/handled.txt";
	unhandled_file.filepath = L"messages/unhandled.txt";
}


MessageHandler::~MessageHandler()
{
	for (std::vector<Message*>::iterator it = msgs_handled.begin(); it != msgs_handled.end(); ++it)
		delete *it;
	for (std::vector<Message*>::iterator it = msgs_unhandled.begin(); it != msgs_unhandled.end(); ++it)
		delete *it;
}

void MessageHandler::parse_messages(Terminal* terminal, std::wstring raw_str, CompList* comp_list)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find(L"+CMGL:");
	int end = raw_str.find(L"+CMGL:", start + 1) - 1;
	while (start != std::wstring::npos)
	{
		Message* msg = new Message();
		std::wstring msg_str = raw_str.substr(start, end - start + 1);	//String containing message info as well as CMGL response of the modem
		decode_msg(msg, msg_str, comp_list);
		
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
		terminal->delete_message_from_sim(msg->cmgl_ids[0]);
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


void MessageHandler::process_msg(Message* msg, Terminal* terminal, ReportCollection* report_collection, CompList* comp_list, std::wstring date, std::wstring english_date)
{
		bool processed_this_msg = false;
		if (!msg->concatenated)
		{
			if (msg->type == TYPE_REPORT)
			{
				Report report;
				report.set_type(Report::TYPE_REGULAR);
				report.read_message(*msg, date);
				report_collection->reports[Report::TYPE_REGULAR][ReportCollection::COMP].add_report(report);

				processed_this_msg = true;

				int baptisms = _wtoi(report.report_values[L"BAP"].c_str());
				if (baptisms > 0)
				{
					terminal->send_message(msg->sender_number, L"Congratulations on your baptism[s]! Please send in one copy of this template per baptism.");
					terminal->send_message(msg->sender_number, L"TYPE:BAPTISM\nCONV_NAME:\nBP_DATE:\nCONF_DATE:\nWARD:\nHOME_ADDR:\nPH_NUM:\nBAP_SOURCE:\n1=Missionary contacting\n2=LA referral\n3=RC referral\n4=Active member referral\n5=English class\n6=Temple tour");
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

				int choice = _wtoi(report.report_values[L"BAP_SOURCE"].c_str());
				Report bap_source = report;
				bap_source.report_values.clear();
				bap_source.set_type(Report::TYPE_BAPTISM_SOURCE);
				for (std::vector<std::wstring>::iterator it = bap_source.key_list.begin(); it != bap_source.key_list.end(); ++it)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(*it, L"0"));	//Fill with zeros
				if (choice == 1)
					bap_source.report_values[L"BAP_MISS_FIND"] = L"1";
				else if (choice == 2)
					bap_source.report_values[L"BAP_LA_REF"] = L"1";
				else if (choice == 3)
					bap_source.report_values[L"BAP_RC_REF"] = L"1";
				else if (choice == 4)
					bap_source.report_values[L"BAP_MEM_REF"] = L"1";
				else if (choice == 5)
					bap_source.report_values[L"BAP_ENGLISH"] = L"1";
				else if (choice == 6)
					bap_source.report_values[L"BAP_TOUR"] = L"1";

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
					//referral_list.add_sent(referral);
				}
				else
				{
					terminal->send_message(L"+886972576566", msg->contents);	//Send it to the recorder!
					//referral_list.add_unsent(referral);
				}

				processed_this_msg = true;
			}
			else if (msg->type == TYPE_UNKNOWN)
			{
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
}

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

void MessageHandler::save()
{
	handled_file.open(File::FILE_TYPE_OUTPUT);
	save(&handled_file, true);
	handled_file.close();
	unhandled_file.open(File::FILE_TYPE_OUTPUT);
	save(&unhandled_file, false);
	unhandled_file.close();
}

void MessageHandler::load()
{
	handled_file.open(File::FILE_TYPE_INPUT);
	load(&handled_file, true);
	handled_file.close();
	unhandled_file.open(File::FILE_TYPE_INPUT);
	load(&unhandled_file, false);
	unhandled_file.close();
}