#pragma once

#include <mutex>
#include <queue>

using namespace std;

class ModemInterface
{
private:
	mutex mu;
	queue <wstring> commands;
	queue <pair<wstring, wstring>> results;
public:
	ModemInterface();
	virtual ~ModemInterface();

	void push_command(wstring cmd);
	wstring pop_command();
	size_t num_commands();

	void push_result(wstring cmd, wstring result);
	pair<wstring, wstring> pop_result();
	size_t num_results();
};

