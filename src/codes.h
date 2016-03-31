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

#include <string>
using namespace std;

//Version
#define VERSION L"1.0"

//Filepaths

#define PATHS_FILEPATH L"paths.txt"

//Baptism source key strings
#define REP_KEY_BAP L"BAP"
#define REP_KEY_BAP_SOURCE L"BAP_SOURCE"
#define REP_KEY_BAP_MISS_FIND L"BAP_MISS_FIND"
#define REP_KEY_BAP_LA_REF L"BAP_LA_REF"
#define REP_KEY_BAP_RC_REF L"BAP_RC_REF"
#define REP_KEY_BAP_MEM_REF L"BAP_MEM_REF"
#define REP_KEY_BAP_ENGLISH L"BAP_ENGLISH"
#define REP_KEY_BAP_TOUR L"BAP_TOUR"

//Phone numbers
#define INT_PH_NUM_PREFIX L"+886"

//File paths
#define PATH_PATH_FILE L"paths.txt"

//File ID strings (to match in paths file)
#define FILE_OUTPUT L"OUTPUT"
#define FILE_CONFIG L"CONFIG"
#define FILE_PH_LIST L"PH_LIST"
#define FILE_MESSAGES_HANDLED L"MESSAGES_HANDLED"
#define FILE_MESSAGES_UNHANDLED L"MESSAGES_UNHANDLED"
#define FILE_MESSAGES_OUTBOX L"MESSAGES_OUTBOX"
#define FILE_REFERRALS L"REFERRALS"

//Config fields (to match in config file)
#define CONFIG_FIELD_REPORT_WDAY L"REPORT_WDAY"
#define CONFIG_FIELD_ENGLISH_WDAY L"ENGLISH_WDAY"
#define CONFIG_FIELD_STRAY_MSG_HANDLER L"STRAY_MSG_HANDLER"
#define CONFIG_FIELD_BAPTISM_RESPONSE L"BAPTISM_RESPONSE"
#define CONFIG_FIELD_BAPTISM_REPORT L"BAPTISM_REPORT"

//String constants
#define PH_NUMBER_HEAD L"+"
#define DISPLAY_TEXT_SEPARATOR L":"
#define ID_STR_SEPARATOR L":"
#define COMMAND_NEWLINE_CHAR L"\r"
#define COMMAND_ESCAPE_CHAR L"\u001A"		//The CTRL-Z character used at the end of PDU payload data

//Timeout values
#define NO_RESPONSE_TIMEOUT_MS 1000
#define MSG_NO_RESPONSE_TIMEOUT_MS 60000

//Message constants
#define TYPE_KEY L"TYPE"
#define KEY_END_CHAR L"\n"
#define TYPE_REPORT_STR L"REPORT"
#define TYPE_ENGLISH_STR L"ENGLISH"
#define TYPE_BAPTISM_STR L"BAPTISM"
#define TYPE_REFERRAL_STR L"REFERRAL"
#define MAX_MSG_LEN 134 	//140 characters of message (for 8-bit encoding); take away 6 bits for possible UDH data

//JSON keys
#define JK_SENDER_NAME "SENDER_NAME"
#define JK_SENDER_NUMBER "SENDER_NUMBER"
#define JK_CONTENTS "CONTENTS"
#define JK_SENT_DATE "SENT_DATE"
#define JK_DATA_CODING "DATA_CODING"
#define JK_MSG_LEN "MSG_LEN"
#define JK_CONCAT "CONCAT"
#define JK_CONCAT_REFNUM "C_REFNUM"
#define JK_CONCAT_NUM_MSG "C_NUM_MSG"
#define JK_CONCAT_INDEX "C_INDEX"

//Serial constants
#define MAX_NUM_TRIES 64		//Try this many times to get a character across the serial cable before quitting

//Graphics options
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BAR_HEIGHT 25
#define BUTTON_WIDTH 100
#define CHECKBOX_WIDTH 25
#define SPACING 5

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