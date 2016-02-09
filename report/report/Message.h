#pragma once

#include <string>

#include "CompList.h"

std::wstring extract_septets(std::wstringstream&ss, int length);
std::wstring encode_phone_number(std::wstring phone_number);
std::wstring extract_phone_number(std::wstringstream&ss, int length);

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

	MessageType type;
	std::wstring raw_pdu;
	std::wstring sender_name;
	std::wstring sender_number;
	std::wstring contents;
	std::wstring sent_date;
	std::wstring cmgl_id;
	int data_coding;
	int msg_length;

	Message();
	virtual ~Message();

	std::wstring encode(std::wstring dest_number);
	void parse(std::wstring input, CompList* comp_list);
};

