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

#include "codes.h"

#include <string>

#include "CompList.h"

#include "rapidjson/document.h"

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

	//Incoming: state tracking
	bool processed;

	//Outgoing:
	std::wstring dest_number;

public:
	std::wstring get_contents() const { return contents; }
	std::wstring get_raw_pdu() const { return raw_pdu; }
	std::wstring get_sender_number() const { return sender_number; }
	std::wstring get_sent_date() const { return sent_date; }
	int get_data_coding() const { return data_coding; }
	int get_msg_length() const { return msg_length; }
	bool is_concatenated() const { return concatenated; }
	int get_concat_refnum() const { return concat_refnum; }
	int get_concat_num_msgs() const { return concat_num_msgs; }
	int get_concat_index() const { return concat_index; }
	std::vector<int> get_cmgl_ids() const { return cmgl_ids; }
	std::wstring get_sender_name() const { return sender_name; }
	bool is_processed() const { return processed; }
	std::wstring get_dest_number() const { return dest_number; }

	bool operator==(const Message& other) const { return contents == other.contents && sender_name == other.sender_name && sent_date == other.sent_date && cmgl_ids == other.cmgl_ids; }

	void set_contents(std::wstring contents_in) { contents = contents_in; }
	void app_contents(std::wstring contents_app) { contents += contents_app; }
	void push_cmgl_id(int cmgl_in) { cmgl_ids.push_back(cmgl_in); }
	void set_dest_number(std::wstring dest_number_in) { dest_number = dest_number_in; }
	void set_concatenated(bool concatenated_in) { concatenated = concatenated_in; }
	void set_sender_name(std::wstring sender_name_in) { sender_name = sender_name_in; }
	void set_processed(bool processed_in) { processed = processed_in; }

	std::vector<std::wstring> encode();
	void decode(std::wstring raw_input, CompList* comp_list);
	void read(const rapidjson::Value& m_val);
	void write(rapidjson::Document* d);
};




