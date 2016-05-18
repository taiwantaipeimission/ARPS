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

#pragma once

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <string>
#include <map>
#include <vector>

using namespace std;

enum ReportType
{
	TYPE_REGULAR,
	TYPE_ENGLISH,
	TYPE_BAPTISM_RECORD,
	TYPE_BAPTISM_SOURCE,
	TYPE_REFERRAL,
	NUM_TYPES
};

enum ReportOrder
{
	COMP,					//Data by companionship
	COMP_MONTH,
	DISTRICT,				//Data by district
	DISTRICT_MONTH,			//Data by district (monthly totals)
	ZONE,					//Data by zone
	ZONE_MONTH,				//Data by zone (monthly totals)
	WARD,					//Ward
	WARD_MONTH,
	STAKE,
	STAKE_MONTH,
	MISSION,
	MISSION_MONTH,
	INDIV,					//Data by individual
	NUM_DATA_ORDERS
};

//Version
extern wstring g_version;

//Baptism source key strings
extern wstring g_rep_key_bap;
extern wstring g_rep_key_bap_source;

//Orders to total
extern map<ReportType, vector<ReportOrder>> g_reports_to_store;

//Phone numbers
extern wstring g_int_ph_num_prefix;

extern std::wstring g_stray_msg_handler;
extern std::wstring g_baptism_response_msg;
extern std::wstring g_baptism_report_template;
extern std::wstring g_reminder_msg;
extern std::wstring g_english_reminder_msg;

extern int report_wday;
extern int english_wday;

//File paths
extern wstring g_path_file;

//File ID strings (to match in paths file)
extern wstring g_file_output;
extern wstring g_file_config;
extern wstring g_file_ph_list;
extern wstring g_file_messages_handled;
extern wstring g_file_messages_unhandled;
extern wstring g_file_messages_outbox;
extern wstring g_file_referrals;

//Config fields (to match in config file)
extern wstring g_config_field_report_wday;
extern wstring g_config_field_english_wday;
extern wstring g_config_field_stray_msg_handler;
extern wstring g_config_field_baptism_response;
extern wstring g_config_field_baptism_report;
extern wstring g_config_field_reminder_msg;
extern wstring g_config_field_english_reminder_msg;

//String constants
extern wstring g_ph_number_head;
extern wstring g_display_text_separator;
extern wstring g_id_str_separator;
extern wstring g_command_newline_char;
extern wstring g_command_escape_char;

//Timeout values
extern double g_no_response_timeout_ms;
extern double g_msg_no_response_timeout_ms;
extern double g_auto_check_s;

//Message constants
extern wstring g_type_key;
extern wstring g_key_end_char;
extern wstring g_type_report_str;
extern wstring g_type_english_str;
extern wstring g_type_baptism_str;
extern wstring g_type_referral_str;
extern int g_max_msg_len;

//JSON keys
extern string g_jk_sender_name;
extern string g_jk_sender_number;
extern string g_jk_contents;
extern string g_jk_sent_date;
extern string g_jk_data_coding;
extern string g_jk_msg_len;
extern string g_jk_concat;
extern string g_jk_concat_refnum;
extern string g_jk_concat_num_msg;
extern string g_jk_concat_index;

//Serial constants
extern int g_max_num_tries;

//Graphics options
extern int g_window_width;
extern int g_window_height;
extern int g_button_height;
extern int g_button_width;
extern int g_spacing;

//Console colors
enum
{
	CC_BLACK = 0,
	CC_DARK_BLUE = 1,
	CC_DARK_GREEN = 2,
	CC_TEAL = 3,
	CC_DARK_RED = 4,
	CC_DARK_PURPLE = 5,
	CC_GOLD = 6,
	CC_GREY = 7,
	CC_DARK_WHITE = 8,
	CC_BLUE = 9,
	CC_GREEN = 10,
	CC_CYAN = 11,
	CC_RED = 12,
	CC_PURPLE = 13,
	CC_YELLOW = 14,
	CC_WHITE = 15
};

void load_config(rapidjson::Document* config);