#pragma once

#include <string>

#include "CompList.h"

class Message
{
public:
	
	enum MessageType
	{
		TYPE_REPORT,
		TYPE_REPORT_ENGLISH
	};

	MessageType type;
	std::string sender_name;
	std::string sender_number;
	std::string contents;
	std::string sent_date;
	std::string cmgl_id;

	Message();
	virtual ~Message();

	void parse(std::string input, CompList* comp_list);
};

