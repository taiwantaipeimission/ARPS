#pragma once

#include <queue>

using namespace std;

class ModemInterface
{
private:
	queue <wstring> commands;
	queue <pair<wstring, wstring>> results;
public:
	ModemInterface();
	virtual ~ModemInterface();

	void push_command(wstring cmd);
	wstring pop_command();

	void push_result(wstring cmd, wstring result);
	pair<wstring, wstring> pop_result();
};

