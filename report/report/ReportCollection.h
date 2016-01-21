#pragma once

#include "ReportSheet.h"

class ReportCollection
{
public:
	ReportSheet report_by_comp;		//One report per companionship
	ReportSheet report_by_zone;		//One report per missionary
	ReportSheet report_by_indiv;	//One report per zone

	ReportCollection();
	virtual ~ReportCollection();
};

