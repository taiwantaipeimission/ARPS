#pragma once

#include <string>

#include "CompList.h"


#define MAX_MSG_LEN 140	//140 characters of message (for 8-bit encoding)

class Message;

std::wstring encode_octet_value(std::wstring chars, int octet_size);
std::wstring extract_septets(std::wstringstream&ss, unsigned int length);
std::wstring encode_phone_number(std::wstring phone_number);
std::wstring extract_phone_number(std::wstringstream&ss, int length);
int get_octet_value(std::wstring octet_rep, int octet_size);
std::vector<std::wstring> encode_msg(Message* msg);
bool decode_msg(Message* msg, std::wstring raw_input, CompList* comp_list);

class Message
{
public:
	
	enum MessageType
	{
		TYPE_REPORT,
		TYPE_REPORT_ENGLISH,
		TYPE_REFERRAL,
		TYPE_UNKNOWN
	};

	//Fields deducted from PDU content
	std::wstring raw_pdu;
	std::wstring sender_number;
	std::wstring contents;
	std::wstring sent_date;
	int data_coding;
	int msg_length;
	bool concatenated;		//The msg is part of a concatenated msg consisting of multiple SMSs
	int concat_refnum;
	int concat_num_msgs;	//Size of concat series
	int concat_index;		//Index in concat series

	//Fields deducted from AT modem reply context
	std::vector<int> cmgl_ids;	//List of cmgl_ids. If regular, this has size 1.  If concatenated, it contains all cmgl_ids of the sms that it is made of.
	
	//Fields deducted from comparison to area list
	MessageType type;
	std::wstring sender_name;
	
	//Fields used in encoding a send request only
	std::wstring dest_number;
	
	
	Message();
	virtual ~Message();
};

