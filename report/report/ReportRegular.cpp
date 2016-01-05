#include "ReportRegular.h"


ReportRegular::ReportRegular()
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", };
}


ReportRegular::~ReportRegular()
{
}

void ReportRegular::add_field(std::string key, std::string value)
{
	report_values[key] = value;
}

void ReportRegular::remove_field(std::string key)
{
	report_values.erase(key);
}