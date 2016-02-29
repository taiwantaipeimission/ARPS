#pragma once

#include <string>

class Reminder
{
public:
	int tm_wday;
	int tm_hour;
	int tm_min;
	bool english;
	bool sent;

	Reminder();
	Reminder(std::wstring time_str);
	virtual ~Reminder();

	void read_time_str(std::wstring time_str);
};

