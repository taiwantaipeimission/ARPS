#pragma once

#include <string>

#include "CompList.h"

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
	std::wstring sender_name;
	std::wstring sender_number;
	std::wstring contents;
	std::wstring sent_date;
	std::wstring cmgl_id;

	Message();
	virtual ~Message();

	void parse(std::wstring input, CompList* comp_list);
};

