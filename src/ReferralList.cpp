#include "ReferralList.h"


ReferralList::ReferralList()
{
}


ReferralList::~ReferralList()
{
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
}

void ReferralList::save(File* file, std::wstring date)
{
	for (ReferralList::iterator it = this->begin(); it != this->end(); ++it)
	{
		file->file << it->print(date) << L"\n";
	}
}