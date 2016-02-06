#pragma once

#include <map>
#include <string>

class ReferralRoutingTable
{
public:
	ReferralRoutingTable();
	~ReferralRoutingTable();

	void load_file(std::istream& input);

	std::map<std::string, std::string> area_table;
};