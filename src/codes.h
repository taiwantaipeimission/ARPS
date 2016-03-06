#pragma once

//Filepaths

#define PATHS_FILEPATH L"paths.txt"

//Message text

#define BAPTISM_RESPONSE_MSG L"Congratulations on your baptism[s]! Please send in one copy of this template per baptism."
#define BAPTISM_REPORT_TEMPLATE L"TYPE:BAPTISM\nCONV_NAME:\nBP_DATE:\nCONF_DATE:\nWARD:\nHOME_ADDR:\nPH_NUM:\nBAP_SOURCE:\n1=Missionary contacting\n2=LA referral\n3=RC referral\n4=Active member referral\n5=English class\n6=Temple tour"

//Report value key strings
#define REP_KEY_BAP L"BAP"
#define REP_KEY_BAP_SOURCE L"BAP_SOURCE"
#define REP_KEY_BAP_MISS_FIND L"BAP_MISS_FIND"
#define REP_KEY_BAP_LA_REF L"BAP_LA_REF"
#define REP_KEY_BAP_RC_REF L"BAP_RC_REF"
#define REP_KEY_BAP_MEM_REF L"BAP_MEM_REF"
#define REP_KEY_BAP_ENGLISH L"BAP_ENGLISH"
#define REP_KEY_BAP_TOUR L"BAP_TOUR"

//Phone numbers
#define LOST_REFERRAL_HANDLER L"+886910358944"

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

//String constants
#define PH_NUMBER_HEAD L"+"
#define DISPLAY_TEXT_SEPARATOR L":"
#define ID_STR_SEPARATOR L":"
#define COMMAND_NEWLINE_CHAR L"\r"
#define COMMAND_ESCAPE_CHAR L";"

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

//Graphics options
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BAR_HEIGHT 25
#define BUTTON_WIDTH 100
#define CHECKBOX_WIDTH 25
#define SPACING 5