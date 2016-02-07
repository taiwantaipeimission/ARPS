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
	std::wstring name;
	std::wstring number;
	std::wstring info;

	Referral();
	virtual ~Referral();

	void read_message(Message msg);
	bool locate(CompList* list);
	bool found_dest();
};

