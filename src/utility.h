#include "codes.h"

#include <string>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

static const std::wstring tos(int x)
{
	wchar_t str[256] = L"";
	_itow_s(x, str, 256, 10);
	return str;
}

static const std::string tos(std::wstring str)
{
	std::string st(str.begin(), str.end());
	return st;
}

static const std::vector<std::wstring> tokenize(std::wstring str, wchar_t delim)
{
	std::wstring token;
	std::vector<std::wstring> results;
	std::wstringstream ss(str);
	while (std::getline(ss, token, delim))
		results.push_back(token);
	return results;
}

static const std::wstring get_report_date_str(std::wstring report_wday)
{
	std::wstring report_date;
	time_t curtime;
	tm curtime_st;
	time(&curtime);
	localtime_s(&curtime_st, &curtime);

	int cur_wday = (curtime_st.tm_wday == 0 ? 7 : curtime_st.tm_wday);
	if (cur_wday == _wtoi(report_wday.c_str()))
	{
		report_date = tos(1900 + curtime_st.tm_year)
			+ ID_STR_SEPARATOR + tos(curtime_st.tm_mon + 1)
			+ ID_STR_SEPARATOR + tos((curtime_st.tm_mday + 6) / 7)
			+ ID_STR_SEPARATOR + report_wday;
	}
	else
	{
		tm last_week_tm_st = curtime_st;
		last_week_tm_st.tm_mday -= 7;
		time_t last_week_tm = mktime(&last_week_tm_st);
		localtime_s(&last_week_tm_st, &last_week_tm);
		report_date = tos(1900 + last_week_tm_st.tm_year)
			+ ID_STR_SEPARATOR + tos(last_week_tm_st.tm_mon + 1)
			+ ID_STR_SEPARATOR + tos((last_week_tm_st.tm_mday + 6) / 7)
			+ ID_STR_SEPARATOR + report_wday;
	}
	return report_date;
}