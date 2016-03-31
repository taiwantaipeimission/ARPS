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

#include <string>
#include "Message.h"

class Referral
{
public:
	std::wstring src_number;
	std::wstring src_name;
	std::wstring dest_geog_area;
	std::wstring dest_number;
	std::wstring dest_zone;
	std::wstring name;
	std::wstring number;
	std::wstring info;
	std::wstring contact_state;
	std::wstring date;

	Referral();
	virtual ~Referral();

	void read_message(Message* msg, std::wstring date_in);
	std::wstring print();
	void load(std::wstring line);

	bool locate(CompList* list);
	bool found_dest();
};

