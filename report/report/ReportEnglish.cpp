#include "ReportEnglish.h"

//const std::string ReportEnglish::key_list_array[] = { "CLASS_LEVEL", "TOTAL_STUDENTS", "NEW_STUDENTS", "NEW_INVESTIGATORS" };
//const int ReportEnglish::num_keys = 4;

ReportEnglish::ReportEnglish()
: Report()
{
	set_key_list();
	type = TYPE_ENGLISH;
}

ReportEnglish::~ReportEnglish()
{
}

void ReportEnglish::set_key_list()
{
	key_list = { "CLASSLEVEL", "TOTALSTUDENTS", "TOTALNONMEM", "NEWSTUDENTS", "NEWINV" };
}