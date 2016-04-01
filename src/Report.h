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

class Message;

#include "utility.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>


class Report
{
public:
	
	ReportType type;
	ReportOrder order;
	
	int date_year;
	int date_month;
	int date_week;
	int date_wday;
	std::wstring sender_name;
	int sub_id;							//Used to differentiate English reports for different class levels, same date, same companionship.  Other report types don't use
	bool use_sub_id;

	std::map<std::wstring, std::wstring> report_values;			//List of all values contained; may contain fields not in vector<string> fields if they are present in processed data

	Report();
	virtual ~Report();

	virtual void set_type(ReportType type);
	virtual void read_message(Message* msg, std::vector<std::wstring> fields_to_read, std::wstring date);

	virtual std::wstring get_id_str();
	virtual std::wstring get_date();
	virtual std::wstring get_sender_name();

	virtual bool operator==(Report& other);
	virtual bool operator!=(Report& other);
	virtual void operator+=(Report& other);

	virtual void clear_values();

	virtual void read_processed(std::wstring input, std::vector<std::wstring> field_order);
	virtual void print(std::wostream& output, std::vector<std::wstring> fields_to_write);
};