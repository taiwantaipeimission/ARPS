#include "Referral.h"
#include <sstream>

Referral::Referral()
{
}


Referral::~Referral()
{
}

void Referral::read_message(Message msg)
{
	src_number = msg.sender_number;
	src_name = msg.sender_name;

	std::stringstream ss(msg.contents);
	ss.ignore(1000, '\n');	//get rid of type line
	ss.ignore(1000, '\n');	//get rid of type line
	std::getline(ss, dest_number);
	std::getline(ss, name);
	std::getline(ss, info);
}

bool Referral::found_dest()
{
	return(dest_number != "");
}