#pragma once

#include <string>
#include "Message.h"

class Referral
{
public:
	std::wstring src_number;
	std::wstring src_name;
	std::wstring dest_geog_area;
	std::wstring dest_number;
	std::wstring dest_zone;
	std::wstring name;
	std::wstring number;
	std::wstring info;
	std::wstring contact_state;

	Referral();
	virtual ~Referral();

	void read_message(Message msg);
	std::wstring print(std::wstring date);
	void load(std::wstring line);

	bool locate(CompList* list);
	bool found_dest();
};

