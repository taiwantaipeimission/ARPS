#pragma once

#include "Referral.h"
#include <vector>

class ReferralList :
	public std::vector<Referral>
{
public:
	ReferralList();
	virtual ~ReferralList();

	void load(File* file);
	void save(File* file, std::wstring date);
};

