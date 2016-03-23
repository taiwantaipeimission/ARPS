#pragma once

#include <string>
using namespace std;

//Internals

//Filepaths

#define PATHS_FILEPATH L"paths.txt"

//Message text

#define BAPTISM_RESPONSE_MSG L"Congratulations on your baptism[s]! Please send in one copy of this template per baptism."
#define BAPTISM_REPORT_TEMPLATE L"TYPE:BAPTISM\nCONV_NAME:\nBP_DATE:\nCONF_DATE:\nWARD:\nHOME_ADDR:\nPH_NUM:\nBAP_SOURCE:\n1=Missionary contacting\n2=LA referral\n3=RC referral\n4=Active member referral\n5=English class\n6=Temple tour"

//Report key strings
#define REPORT_FIELDS L"A,B,C,D,NEXTWEEKBAP,BAP,CONF,BD,SAC,PK,OL,NIMISSFIND,NIMEMREF,RCLA,LAC,RCT"
#define ENGLISH_FIELDS L"CLASSLEVEL,TOTALSTUDENTS,TOTALNONMEM,NEWSTUDENTS,NEWINV"
#define BAPTISM_RECORD_FIELDS L"CONV_NAME,BP_DATE,CONF_DATE,WARD,HOME_ADDR,PH_NUM"
#define BAPTISM_SOURCE_FIELDS L"BAP_MISS_FIND,BAP_LA_REF,BAP_RC_REF,BAP_MEM_REF,BAP_ENGLISH,BAP_TOUR"

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
#define FILE_REFERRALS L"REFERRALS"

//Config fields (to match in config file)
#define CONFIG_FIELD_REPORT_WDAY L"REPORT_WDAY"
#define CONFIG_FIELD_ENGLISH_WDAY L"ENGLISH_WDAY"
#define CONFIG_FIELD_STRAY_MSG_HANDLER L"STRAY_MSG_HANDLER"

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
#define MSG_FILE_FIELD_SEPARATOR L'|'
#define MSG_SEPARATOR L'\\'

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