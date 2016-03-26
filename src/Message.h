#pragma once

#include "codes.h"

#include <string>

#include "CompList.h"

std::wstring encode_hex_value(std::wstring chars, int rep_num_chars);
std::wstring unpack_septets(std::vector<int> packed_data);
std::wstring encode_phone_number(std::wstring phone_number);
std::wstring extract_phone_number(std::wstringstream&ss, int length);
int get_hex_value(std::wstring hex_rep);

class Message
{
private:
	//Applies to both incoming + outcoming messages
	std::wstring contents;

	//Incoming: fields deducted from PDU content
	std::wstring raw_pdu;
	std::wstring sender_number;
	std::wstring sent_date;
	int data_coding;
	int msg_length;
	bool concatenated;		//The msg is part of a concatenated msg consisting of multiple SMSs
	int concat_refnum;
	int concat_num_msgs;	//Size of concat series
	int concat_index;		//Index in concat series

	//Incoming: fields deducted from AT modem reply context
	std::vector<int> cmgl_ids;	//List of cmgl_ids. If regular, this has size 1.  If concatenated, it contains all cmgl_ids of the sms that it is made of.
	
	//Incoming: fields deducted from comparison to area list
	std::wstring sender_name;

	//Outgoing:
	std::wstring dest_number;

public:
	std::wstring get_contents() { return contents; }
	std::wstring get_raw_pdu() { return raw_pdu; }
	std::wstring get_sender_number() { return sender_number; }
	std::wstring get_sent_date() { return sent_date; }
	int get_data_coding() { return data_coding; }
	int get_msg_length() { return msg_length; }
	bool is_concatenated() { return concatenated; }
	int get_concat_refnum() { return concat_refnum; }
	int get_concat_num_msgs() { return concat_num_msgs; }
	int get_concat_index() { return concat_index; }
	std::vector<int> get_cmgl_ids() { return cmgl_ids; }
	std::wstring get_sender_name() { return sender_name; }
	std::wstring get_dest_number() { return dest_number; }

	void set_contents(std::wstring contents_in) { contents = contents_in; }
	void app_contents(std::wstring contents_app) { contents += contents_app; }
	void push_cmgl_id(int cmgl_in) { cmgl_ids.push_back(cmgl_in); }
	void set_dest_number(std::wstring dest_number_in) { dest_number = dest_number_in; }
	void set_concatenated(bool concatenated_in) { concatenated = concatenated_in; }

	std::vector<std::wstring> encode();
	void decode(std::wstring raw_input, CompList* comp_list);
	void read(std::wstring input);
	std::wstring write();
};




