#include "ReportEnglish.h"

//const std::string ReportEnglish::key_list_array[] = { "CLASS_LEVEL", "TOTAL_STUDENTS", "NEW_STUDENTS", "NEW_INVESTIGATORS" };
//const int ReportEnglish::num_keys = 4;

ReportEnglish::ReportEnglish(Message msg, std::string date)
: Report(msg, date)
{
}

ReportEnglish::ReportEnglish()
	: Report()
{
}

ReportEnglish::~ReportEnglish()
{
}
