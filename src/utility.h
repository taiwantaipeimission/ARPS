#pragma once

#include "codes.h"

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
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string st = converter.to_bytes(str);
	return st;
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

static const std::wstring get_msg_key_val(std::wstring contents, std::wstring key, wchar_t separator, wchar_t val_delim, bool strip_ws = true)
{
	int key_pos;
	for (key_pos = contents.find(key + separator); key_pos != std::wstring::npos && key_pos != 0 && !isspace(contents[key_pos - 1]); key_pos = contents.find(key + separator, key_pos + 1))
	{
	}

	int value_pos = contents.find(separator, key_pos) + 1;
	int value_end_pos = (std::min)(contents.find(val_delim, value_pos), contents.find('\r', value_pos));

	std::wstring value = L"0";
	if (key_pos != std::wstring::npos && value_pos != std::wstring::npos)
	{
		if (value_pos != value_end_pos)
		{
			value = contents.substr(value_pos, value_end_pos - value_pos);
			if(strip_ws)
				strip_chars(value, L" \n\t\r");		//Strip whitespace from string
		}
	}
	return value;
}

static const inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

static const std::wstring get_report_date_str(std::wstring report_wday)
{
	std::wstring report_date;
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);
	int i_report_wday = _wtoi(report_wday.c_str());

	int cur_wday = (curtime_st.tm_wday == 0 ? 7 : curtime_st.tm_wday);
	
	if (cur_wday == i_report_wday)
	{
		report_date = tos(1900 + curtime_st.tm_year)
			+ ID_STR_SEPARATOR + tos(curtime_st.tm_mon + 1)
			+ ID_STR_SEPARATOR + tos((curtime_st.tm_mday + 6) / 7)
			+ ID_STR_SEPARATOR + report_wday;
	}
	else
	{
		int days_since_last_report = positive_modulo(cur_wday - i_report_wday, 7);
		tm last_week_tm_st = curtime_st;
		last_week_tm_st.tm_mday -= days_since_last_report;
		time_t last_week_tm = mktime(&last_week_tm_st);
		localtime_s(&last_week_tm_st, &last_week_tm);
		report_date = tos(1900 + last_week_tm_st.tm_year)
			+ ID_STR_SEPARATOR + tos(last_week_tm_st.tm_mon + 1)
			+ ID_STR_SEPARATOR + tos((last_week_tm_st.tm_mday + 6) / 7)
			+ ID_STR_SEPARATOR + report_wday;
	}
	return report_date;
}

static const int set_color(const int foreground, const int background)
{

	int color = foreground + (background * 16);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);

	return 0;
}