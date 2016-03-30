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

#include "ReferralList.h"


ReferralList::ReferralList()
: changed(false)
{
}


ReferralList::~ReferralList()
{
}

bool ReferralList::is_saved()
{
	return !changed;
}

void ReferralList::push_back(Referral ref)
{
	std::vector<Referral>::push_back(ref);
	changed = true;
}

void ReferralList::load(File* file)
{
	std::wstring line;
	while (std::getline(file->file, line))
	{
		Referral ref;
		ref.load(line);
		push_back(ref);
	}
	changed = false;
}

void ReferralList::save(File* file, std::wstring date)
{
	for (ReferralList::iterator it = this->begin(); it != this->end(); ++it)
	{
		file->file << it->print() << L"\n";
	}
	changed = false;
}