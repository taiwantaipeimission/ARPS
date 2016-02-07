#include "Reminder.h"

#include <sstream>

Reminder::Reminder()
	: tm_wday(7), tm_hour(21), tm_min(30), english(false), sent(false)
{
}

Reminder::Reminder(std::wstring time_str)
: Reminder()
{
	read_time_str(time_str);
}

Reminder::~Reminder()
{
}

void Reminder::read_time_str(std::wstring time_str)
{
	std::wstringstream ss;
	ss.str(time_str);
	ss >> tm_wday;
	ss.ignore();
	ss >> tm_hour;
	ss.ignore();
	ss >> tm_min;
	tm_wday = tm_wday % 7;
}