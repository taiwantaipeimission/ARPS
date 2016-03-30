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

#include "FieldCollection.h"

FieldCollection::FieldCollection()
 : values()
{

}

FieldCollection::~FieldCollection()
{

}

void FieldCollection::read_fields(File* file)
{
	std::wstring line;
	std::wstring key;
	std::wstring value;

	while (file->file.good())
	{
		std::getline(file->file, line);
		int key_beg_pos = 0;
		int key_end_pos = line.find(L"=", key_beg_pos) - 1;
		int value_beg_pos = key_end_pos + 2;
		int value_end_pos = line.find(L"\n", value_beg_pos + 1) - 1;

		key = line.substr(key_beg_pos, key_end_pos - key_beg_pos + 1);
		value = line.substr(value_beg_pos, value_end_pos - value_beg_pos + 1);
		values[key] = value;
	}
}

std::wstring FieldCollection::get_field(std::wstring key)
{
	if (values.count(key) > 0)
	{
		return values[key];
	}
	else
	{
		return L"-";
	}
}