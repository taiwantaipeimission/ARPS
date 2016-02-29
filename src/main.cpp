#define WIN32

#include <string.h>
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <codecvt>


#include "codes.h"
#include "Gui.h"


/* Create the date stamp for a reporting period, based on the current time and the weekday of reporting.
 * All days up to the day of reporting return back to the previous reporting period (e.g. Thursday-Tuesday will be counted as reports
 * for English reporting session which began on the Wednesday previous).
 */


class Console_streambuf
	: public std::basic_streambuf<wchar_t>
{
	HANDLE m_out;
public:
	Console_streambuf(HANDLE out) : m_out(out) {}

	virtual int_type overflow(int_type c = traits_type::eof())
	{
		wchar_t wc = c;
		DWORD numberOfCharsWritten;
		BOOL res = WriteConsoleW(m_out, &wc, 1, &numberOfCharsWritten, NULL);
		(void)res;
		return 1;
	}
};

int main(int argc, char **argv)
{
	try
	{
		Gui gui;
		gui.init();
		gui.run();
	}
	catch(std::exception e)
	{
		std::wcout << L"Error: " << e.what() << L"\n";
	}
	return 0;
}