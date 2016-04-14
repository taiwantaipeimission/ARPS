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

#include "utility.h"
#include <string>

void strtoupper(std::wstring& str)
{
	std::string narrow = tos(str);
	for (std::string::iterator it = narrow.begin(); it != narrow.end(); ++it)
		*it = toupper(*it);
	str = tow(narrow);
}