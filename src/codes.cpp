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

#include "codes.h"
#include "utility.h"

//Version
wstring g_version = L"1.0";

//Baptism source key strings
wstring g_rep_key_bap = L"BAP";
wstring g_rep_key_bap_source = L"BAP_SOURCE";
wstring g_rep_key_num_reports = L"N_REPORTS";

//Orders to total
map<ReportType, vector<ReportOrder>> g_reports_to_store {
	{ TYPE_REGULAR,{ COMP, COMP_MONTH, DISTRICT, DISTRICT_MONTH, ZONE, ZONE_MONTH, WARD, WARD_MONTH, STAKE, STAKE_MONTH, MISSION, MISSION_MONTH } },
	{ TYPE_ENGLISH,{ COMP, ZONE, ZONE_MONTH, MISSION } },
	{ TYPE_BAPTISM_RECORD,{ COMP } },
	{ TYPE_BAPTISM_SOURCE,{ COMP, COMP_MONTH, DISTRICT, DISTRICT_MONTH, ZONE, ZONE_MONTH, WARD, WARD_MONTH, STAKE, STAKE_MONTH, MISSION, MISSION_MONTH } },
	{ TYPE_REFERRAL,{ COMP } }
};

//Phone numbers
wstring g_int_ph_num_prefix = L"+886";

std::wstring g_stray_msg_handler = L"";
std::wstring g_baptism_response_msg = L"";
std::wstring g_baptism_report_template = L"";
std::wstring g_reminder_msg = L"Please remember to send in your key indicators!";
std::wstring g_english_reminder_msg = L"Please remember to send in your English key indicators!";

int report_wday = 0;
int english_wday = 0;

//File paths
wstring g_path_file = L"paths.txt";

//File ID strings (to match in paths file)
wstring g_file_output = L"OUTPUT";
wstring g_file_config = L"CONFIG";
wstring g_file_ph_list = L"PH_LIST";
wstring g_file_messages_handled = L"MESSAGES_HANDLED";
wstring g_file_messages_unhandled = L"MESSAGES_UNHANDLED";
wstring g_file_messages_outbox = L"MESSAGES_OUTBOX";
wstring g_file_referrals = L"REFERRALS";

//Config fields (to match in config file)
wstring g_config_field_report_wday = L"REPORT_WDAY";
wstring g_config_field_english_wday = L"ENGLISH_WDAY";
wstring g_config_field_stray_msg_handler = L"STRAY_MSG_HANDLER";
wstring g_config_field_baptism_response = L"BAPTISM_RESPONSE";
wstring g_config_field_baptism_report = L"BAPTISM_REPORT";
wstring g_config_field_reminder_msg = L"REMINDER_MSG";
wstring g_config_field_english_reminder_msg = L"ENGLISH_REMINDER_MSG";

//String constants
wstring g_ph_number_head = L"+";
wstring g_display_text_separator = L":";
wstring g_id_str_separator = L":";
wstring g_command_newline_char = L"\r";
wstring g_command_escape_char = L"\u001A";

//Timeout values
double g_no_response_timeout_ms = 5000.0f;
double g_msg_no_response_timeout_ms = 60000.0f;
double g_auto_check_s = 300.0f;

//Message constants
wstring g_type_key = L"TYPE";
wstring g_key_end_char = L"\n";
wstring g_type_report_str = L"REPORT";
wstring g_type_english_str = L"ENGLISH";
wstring g_type_baptism_str = L"BAPTISM";
wstring g_type_referral_str = L"REFERRAL";
int g_max_msg_len = 134;	//140 characters of message (for 8-bit encoding); take away 6 bits for possible UDH data

//JSON keys
string g_jk_sender_name = "SENDER_NAME";
string g_jk_sender_number = "SENDER_NUMBER";
string g_jk_contents = "CONTENTS";
string g_jk_sent_date = "SENT_DATE";
string g_jk_data_coding = "DATA_CODING";
string g_jk_msg_len = "MSG_LEN";
string g_jk_concat = "CONCAT";
string g_jk_concat_refnum = "C_REFNUM";
string g_jk_concat_num_msg = "C_NUM_MSG";
string g_jk_concat_index = "C_INDEX";

//Serial constants
int g_max_num_tries = 64;	//Try this many times to get a character across the serial cable before quitting

//Graphics options
int g_window_width = 800;
int g_window_height = 600;
int g_button_height = 25;
int g_button_width = 100;
int g_spacing = 5;

void load_config(rapidjson::Document* config)
{
	report_wday = (*config)[tos(g_config_field_report_wday).c_str()].GetInt();
	english_wday = (*config)[tos(g_config_field_english_wday).c_str()].GetInt();
	g_stray_msg_handler = tow((*config)[tos(g_config_field_stray_msg_handler).c_str()].GetString());
	g_baptism_response_msg = tow((*config)[tos(g_config_field_baptism_response).c_str()].GetString());
	g_baptism_report_template = tow((*config)[tos(g_config_field_baptism_report).c_str()].GetString());
	g_reminder_msg = tow((*config)[tos(g_config_field_reminder_msg).c_str()].GetString());
	g_english_reminder_msg = tow((*config)[tos(g_config_field_english_reminder_msg).c_str()].GetString());
}