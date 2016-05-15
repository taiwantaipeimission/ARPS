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

#pragma once

#include "codes.h"

#include "rapidjson/document.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <codecvt>
#include <locale>
#include <Windows.h>

static const std::wstring tos(int x)
{
	wchar_t str[256] = L"";
	_itow_s(x, str, 256, 10);
	return str;
}

static const std::string tos(std::wstring str)
{
	if (str != L"")
	{
		typedef std::codecvt_utf8<wchar_t> convert_type;
		std::wstring_convert<convert_type, wchar_t> converter;

		std::string st = converter.to_bytes(str);
		return st;
	}
	return "";
}

static const std::wstring tow(std::string str)
{
	if (str != "")
	{
		typedef std::codecvt_utf8<wchar_t> convert_type;
		std::wstring_convert<convert_type, wchar_t> converter;

		std::wstring ws = converter.from_bytes(str);
		return ws;
	}
	return L"";
}

static bool is_integer(const std::wstring& s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	size_t p = 0;
	stoll(s.c_str(), &p, 10);

	return (p == 0);
}

static void strip_chars(std::wstring& str, std::wstring chars_to_remove)
{
	for (unsigned int i = 0; i < chars_to_remove.length(); ++i)
	{
		str.erase(std::remove(str.begin(), str.end(), chars_to_remove[i]), str.end());
	}
}

static void replace_chars(std::wstring& str, std::wstring find, std::wstring replace)
{
	size_t pos;
	while ((pos = str.find(find)) != std::wstring::npos)
	{
		str.replace(pos, find.length(), replace);
	}
}

extern void strtoupper(std::wstring& str);

static const std::vector<std::wstring> tokenize(std::wstring str, wchar_t delim)
{
	std::vector<std::wstring> result;
	std::wstring::iterator it = str.begin();
	bool finished = false;
	do
	{
		std::wstring::iterator begin = it;

		while (it != str.end() && *it != delim)
			++it;

		result.push_back(std::wstring(begin, it));

		if (it != str.end())
			++it;
		else
			finished = true;
	} while (!finished);

	return result;
}

static const std::wstring get_msg_key_val(std::wstring contents, std::wstring key, wchar_t separator, wchar_t val_delim)
{
	int key_pos;
	for (key_pos = contents.find(key + separator); key_pos != std::wstring::npos && key_pos != 0 && !isspace(contents[key_pos - 1]); key_pos = contents.find(key + separator, key_pos + 1))
	{
	}

	int value_pos = contents.find(separator, key_pos) + 1;
	int value_end_pos = (std::min)(contents.find(val_delim, value_pos), contents.find('\r', value_pos));

	std::wstring value = L"";
	if (key_pos != std::wstring::npos && value_pos != std::wstring::npos)
	{
		if (value_pos != value_end_pos)
		{
			value = contents.substr(value_pos, value_end_pos - value_pos);
		}
	}
	return value;
}

static const inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

static const std::wstring get_date_str(tm time_struct)
{
	return tos(1900 + time_struct.tm_year)
		+ g_id_str_separator + tos(time_struct.tm_mon + 1)
		+ g_id_str_separator + tos((time_struct.tm_mday + 7) / 7)
		+ g_id_str_separator + tos(time_struct.tm_wday == 0 ? 7 : time_struct.tm_wday);
}

static const std::wstring get_cur_date_str()
{
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);

	return get_date_str(curtime_st);
}

/* Create the date stamp for a reporting period, based on the current time and the weekday of reporting.
* All days up to the day of reporting return back to the previous reporting period (e.g. Thursday-Tuesday will be counted as reports
* for English reporting session which began on the Wednesday previous).
*/

static const std::wstring get_report_date_str(int report_wday)
{
	std::wstring report_date;
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);

	int cur_wday = (curtime_st.tm_wday == 0 ? 7 : curtime_st.tm_wday);
	
	if (cur_wday == report_wday)
	{
		return get_date_str(curtime_st);
	}
	else
	{
		int days_since_last_report = positive_modulo(cur_wday - report_wday, 7);
		tm last_week_tm_st = curtime_st;
		last_week_tm_st.tm_mday -= days_since_last_report;
		time_t last_week_tm = mktime(&last_week_tm_st);
		localtime_s(&last_week_tm_st, &last_week_tm);
		return get_date_str(last_week_tm_st);
	}
}

static const bool is_final_line(std::wstring line)
{
	return line.find(L"OK\r\n") != std::wstring::npos || line.find(L"> ") != std::wstring::npos || line.find(L"ERROR\r\n") != std::wstring::npos;
}

static const bool is_error(std::wstring line)
{
	return line.find(L"ERROR\r\n") != std::wstring::npos;
}

static const int set_color(const int foreground, const int background)
{

	int color = foreground + (background * 16);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);

	return 0;
}