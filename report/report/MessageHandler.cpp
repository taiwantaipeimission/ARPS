#include "MessageHandler.h"
#include "CompList.h"
#include "Message.h"
#include "Report.h"
#include "Terminal.h"
#include "ReportCollection.h"
#include "Referral.h"

MessageHandler::MessageHandler()
{
}


MessageHandler::~MessageHandler()
{
}

void MessageHandler::parse_messages(Terminal* terminal, std::wstring raw_str, CompList* comp_list)
{
	//parses messages from the raw_str modem output string
	int start = raw_str.find(L"+CMGL:");
	int end = raw_str.find(L"+CMGL:", start + 1) - 1;
	while (start != std::wstring::npos)
	{
		Message msg;
		std::wstring msg_str = raw_str.substr(start, end - start + 1);	//String containing message info as well as CMGL response of the modem
		decode_msg(&msg, msg_str, comp_list);
		
		if (msg.concatenated)
		{
			msgs_fragment[msg.concat_refnum].push_back(msg);
		}
		else
		{
			msgs_unhandled.push_back(msg);
		}
		start = end + 1;
		end = raw_str.find(L"+CMGL:", start + 1) - 1;
		terminal->delete_message_from_sim(msg.cmgl_ids[0]);
	}

	//Attempt to piece together concatenated messages
	for (std::map<int, std::vector<Message>>::iterator it = msgs_fragment.begin(); it != msgs_fragment.end(); )
	{
		std::vector<int> correct_order;					//List of the indeces of message fragments for this concat_id, arranged in proper order
		int num_msgs = it->second[0].concat_num_msgs;
		correct_order.assign(num_msgs, -1);				//Fill with -1s so we can see if any msgs are missing
		for (int i = 0; i < it->second.size(); i++)
		{
			correct_order[it->second[i].concat_index - 1] = i;					//Concat indeces inside msg start from 1, so subtract 1
		}
		if (std::count(correct_order.begin(), correct_order.end(), -1) == 0)	//All sub-msgs found that make up this concat msg
		{
			for (unsigned int i = 1; i < correct_order.size(); i++)
			{
				it->second[correct_order[0]].contents += it->second[correct_order[i]].contents;
				it->second[correct_order[0]].cmgl_ids.push_back(it->second[correct_order[i]].cmgl_ids[0]);
			}
			it->second[correct_order[0]].concatenated = false;
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


void MessageHandler::process_messages(Terminal* terminal, ReportCollection* report_collection, CompList* comp_list, std::wstring date, std::wstring english_date)
{
	std::wstring command_string = L"";
	for (std::vector<Message>::iterator it = msgs_unhandled.begin(); it != msgs_unhandled.end();)
	{
		bool processed_this_msg = false;
		if (!it->concatenated)
		{
			if (it->type == Message::TYPE_REPORT)
			{
				Report report;
				report.set_type(Report::TYPE_REGULAR);
				report.read_message(*it, date);
				report_collection->report_by_comp[Report::TYPE_REGULAR].add_report(report);

				processed_this_msg = true;

				int baptisms = _wtoi(report.report_values[L"BAP"].c_str());
				if (baptisms > 0)
				{
					terminal->send_message(it->sender_number, L"Congratulations on your baptism[s]! Please send in one copy of this template per baptism.");
					terminal->send_message(it->sender_number, L"TYPE:BAPTISM\nCONV_NAME:\nBP_DATE:\nCONF_DATE:\nWARD:\nHOME_ADDR:\nPH_NUM:\nBAP_SOURCE:\n1=Missionary contacting\n2=LA referral\n3=RC referral\n4=Active member referral\n5=English class\n6=Temple tour");
				}
			}
			else if (it->type == Message::TYPE_REPORT_ENGLISH)
			{
				Report report;
				report.set_type(Report::TYPE_ENGLISH);
				report.read_message(*it, english_date);
				report_collection->report_by_comp[Report::TYPE_ENGLISH].add_report(report);

				processed_this_msg = true;
			}
			else if (it->type == Message::TYPE_REPORT_BAPTISM)
			{
				Report report;
				report.set_type(Report::TYPE_BAPTISM_RECORD);
				report.read_message(*it, date);
				report_collection->report_by_comp[Report::TYPE_BAPTISM_RECORD].add_report(report);

				int choice = _wtoi(report.report_values[L"BAP_SOURCE"].c_str());
				Report bap_source;
				bap_source.set_type(Report::TYPE_BAPTISM_SOURCE);
				bap_source.id_str = report.id_str;
				if (choice == 1)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_MISS_FIND", L"1"));
				else if (choice == 2)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_LA_REF", L"1"));
				else if (choice == 3)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_RC_REF", L"1"));
				else if (choice == 4)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_MEM_REF", L"1"));
				else if (choice == 5)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_ENGLISH", L"1"));
				else if (choice == 6)
					bap_source.report_values.insert(std::pair<std::wstring, std::wstring>(L"BAP_TOUR", L"1"));

				report_collection->report_by_comp[Report::TYPE_BAPTISM_SOURCE].add_report(bap_source);

				processed_this_msg = true;
			}
			else if (it->type == Message::TYPE_REFERRAL)
			{
				Referral referral;
				referral.read_message(*it);
				referral.locate(comp_list);
				if (referral.found_dest())
				{
					it->dest_number = referral.dest_number;
					terminal->send_message(it->dest_number, it->contents);
					//referral_list.add_sent(referral);
				}
				else
				{
					terminal->send_message(L"+886972576566", it->contents);	//Send it to the recorder!
					//referral_list.add_unsent(referral);
				}

				processed_this_msg = true;
			}
			else if (it->type == Message::TYPE_UNKNOWN)
			{
				processed_this_msg = true;
			}
		}
		if (processed_this_msg)
		{
			msgs_handled.push_back(*it);
			it = msgs_unhandled.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void MessageHandler::read_filed_msgs(std::wistream& input, bool handled)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		do
		{
			std::wstring line;
			std::getline(input, line, MSG_SEPARATOR);
			Message msg;
			read_filed_msg(&msg, line);
			if (msg.concatenated)
			{
				msgs_fragment[msg.concat_refnum].push_back(msg);
			}
			else
			{
				if (handled)
					msgs_handled.push_back(msg);
				else
					msgs_unhandled.push_back(msg);
			}
		} while (input.good());
	}
}

void MessageHandler::write_filed_msgs(std::wostream& output, bool handled)
{
	std::vector<Message>* list_ptr = handled ? &msgs_handled : &msgs_unhandled;
	for (std::vector<Message>::iterator it = list_ptr->begin(); it != list_ptr->end(); ++it)
	{
		output << write_filed_msg(&*it) << MSG_SEPARATOR;
	}
}