#pragma once

#include "codes.h"

#include <string>

#include "CompList.h"

enum MessageType
{
	TYPE_REPORT,
	TYPE_REPORT_ENGLISH,
	TYPE_REPORT_BAPTISM,
	TYPE_REFERRAL,
	TYPE_UNKNOWN
};

class Message
{
public:

	//Applies to both incoming + outcoming messages
	std::wstring contents;

	//Incoming: fields deducted from PDU content
	std::wstring raw_pdu;
	std::wstring sender_number;
	std::wstring sent_date;
	int data_coding;
	int msg_length;
	bool concatenated;		//The msg is part of a concatenated msg consisting of multiple SMSs
	int concat_refnum;
	int concat_num_msgs;	//Size of concat series
	int concat_index;		//Index in concat series

	//Incoming: fields deducted from AT modem reply context
	std::vector<int> cmgl_ids;	//List of cmgl_ids. If regular, this has size 1.  If concatenated, it contains all cmgl_ids of the sms that it is made of.
	
	//Incoming: fields deducted from comparison to area list
	MessageType type;
	std::wstring sender_name;

	//Outgoing:
	std::wstring dest_number;
	int n_send_tries;
};

std::wstring encode_hex_value(std::wstring chars, int rep_num_chars);
std::wstring unpack_septets(std::vector<int> packed_data);
std::wstring encode_phone_number(std::wstring phone_number);
std::wstring extract_phone_number(std::wstringstream&ss, int length);
int get_hex_value(std::wstring hex_rep);

std::vector<std::wstring> encode_msg(Message* msg);
bool decode_msg(Message* msg, std::wstring raw_input, CompList* comp_list);

void read_filed_msg(Message* msg, std::wstring input);
std::wstring write_filed_msg(Message* msg);


