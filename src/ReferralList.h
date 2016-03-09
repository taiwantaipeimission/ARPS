#pragma once

#include "Referral.h"
#include <vector>

class ReferralList :
	public std::vector<Referral>
{
public:
	
	bool changed;
	
	ReferralList();
	virtual ~ReferralList();

	bool is_saved();
	void push_back(Referral ref);
	void load(File* file);
	void save(File* file, std::wstring date);
};

