#include <string>
#include <iostream>
#include <string>
#include <sstream>

static const std::wstring tos(int x)
{
	wchar_t str[256] = L"";
	_itow_s(x, str, 256, 10);
	return str;
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