#pragma once
#include "Report.h"
class ReportEnglish :
	public Report
{
public:
	ReportEnglish(Message msg, std::string date);
	ReportEnglish();
	virtual ~ReportEnglish();
};

