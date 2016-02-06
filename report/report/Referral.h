#pragma once

#include <string>
#include "Message.h"

class Referral
{
public:
	std::string src_number;
	std::string src_name;
	std::string dest_geog_area;
	std::string dest_number;
	std::string name;
	std::string number;
	std::string info;

	Referral();
	virtual ~Referral();

	void read_message(Message msg);
	bool locate(CompList* list);
	bool found_dest();
};

