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