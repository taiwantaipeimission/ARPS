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

#include "Command.h"

#include <mutex>
#include <queue>

using namespace std;


class ModemInterface
{
private:
	mutex mu;
	queue <Command> cmd_unrun;
	queue <Command> cmd_run;
public:
	ModemInterface();
	virtual ~ModemInterface();

	void push_command(Command cmd);
	void push_command(wstring str);
	Command pop_command();
	size_t num_commands();

	void push_result(Command cmd);
	Command pop_result();
	size_t num_results();
};

