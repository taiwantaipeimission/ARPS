/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

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