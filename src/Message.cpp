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
#include "Message.h"
#include "Area.h"
#include "utility.h"

#include "rapidjson\document.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace rapidjson;

int get_hex_value(std::wstring rep)
{
	std::wstringstream conversion_stream;
	int hex_value;

	conversion_stream.clear();
	conversion_stream << std::hex << rep;
	conversion_stream >> hex_value;

	return hex_value;
}

/* Extracts a hexadecimal value from the stringstream.
 * num_rep_chars: the number of chars to extract as a hex representation, usually 2
 */
int extract_hex_value(std::wstringstream& ss, int num_rep_chars)
{
	wchar_t chars[256];
	ss.get(chars, num_rep_chars + 1);
	return get_hex_value((std::wstring)chars);
}

std::wstring encode_hex_value(std::wstring chars, int num_rep_chars)
{
	std::wstringstream ss;
	for (size_t i = 0; i < chars.size(); i++)
	{
		ss << std::setw(num_rep_chars) << std::setfill(L'0') << std::hex << (int)chars[i];
	}
	return ss.str();
}

std::wstring encode_phone_number(std::wstring ph_num_str)
{
	std::wstringstream ss(ph_num_str);
	ss.ignore(256, '+');
	std::wstring encoded = L"";
	wchar_t orig[3] = L"";
	wchar_t swapped[3] = L"";
	ss.get(orig, 3);
	while(!ss.eof())
	{
		swapped[0] = orig[1] == EOF ? 'F' : orig[1];
		swapped[1] = orig[0];
		encoded += swapped;
		ss.get(orig, 3);
	}
	return encoded;
}

std::wstring extract_phone_number(std::wstringstream& ss, int ph_num_length)
{
	std::wstring number = g_ph_number_head;
	int ph_num_octets = (ph_num_length + 1) / 2;

	{
		//original semi-octet
		wchar_t orig[3] = L"";
		//swapped semi-octet
		wchar_t swapped[3] = L"";
		for (int i = 0; i < ph_num_octets; i++)
		{
			ss.get(orig, 3);
			swapped[0] = orig[1];
			swapped[1] = orig[0] == 'F' ? '\0' : orig[0];
			number += swapped;
		}
	}
	return number;
}

std::wstring unpack_septets(std::vector<int> data)
{
	std::wstring decoded_data = L"";

	data.push_back(0);
	int offset = 0;
	unsigned int index = 0;
	unsigned int base = 0u;
	unsigned int adder = 0u;
	unsigned int result = 0u;
	while (index < data.size())
	{
		if (offset == 0)
		{
			base = data[index] & 0x7F;
			adder = 0u;
			result = base | adder;
			offset++;
			index++;
		}
		else if (offset == 7)
		{
			base = 0u;
			adder = (data[index - 1]) >> (8 - offset);
			result = base | adder;
			offset = 0;
		}
		else
		{
			base = ((data[index]) << offset) & 0x7F;
			adder = (data[index - 1]);
			int new_offset = 8 - offset;
			adder = adder >> new_offset;
			result = base | adder;
			offset++;
			index++;
		}
		decoded_data.push_back((wchar_t)result);
	}
	decoded_data.resize(decoded_data.size() - 1);

	return decoded_data;
}

std::vector<std::wstring> Message::encode()
{
	std::wstring dest_number_encoded = encode_phone_number(dest_number);
	int num_length = dest_number_encoded.length();

	int num_concat_msgs = (contents.length() * 2) / g_max_msg_len + 1;
	std::wstring remaining_contents(contents);
	std::vector<std::wstring> strings;

	int msg_id = rand() % 256;
	for (int i = 0; i < num_concat_msgs; i++)
	{
		int this_msg_length = (std::min)(remaining_contents.length(), (size_t)(g_max_msg_len / 2));
		int all_msg_data_length = this_msg_length * 2 + (num_concat_msgs > 1 ? 6 : 0);
		std::wstring msg_chars = remaining_contents.substr((size_t)0, (size_t)this_msg_length);
		remaining_contents = remaining_contents.substr(this_msg_length, remaining_contents.length() - this_msg_length);

		int pdu_header = 0x11;
		std::wstring udh;			//User data header
		int udhl = 0;					//Length of user data header
		if (num_concat_msgs > 1)
		{
			pdu_header |= 0x40;			//Indicate the presence of a user data header
			udhl = 5;					//5 octets needed for concat info
		}


		std::wstringstream ss;
		ss	<< "00"
			<< std::hex << std::setfill(L'0') << std::setw(2) << pdu_header
			<< "00"
			<< std::hex << std::setfill(L'0') << std::setw(2) << num_length
			<< "91"
			<< dest_number_encoded
			<< "00"
			<< "08"
			<< "AA"
			<< std::hex << std::setfill(L'0') << std::setw(2) << all_msg_data_length;
		if (num_concat_msgs > 1)
		{
			ss << "05"
				<< "00"
				<< "03"
				<< std::hex << std::setfill(L'0') << std::setw(2) << msg_id
				<< std::hex << std::setfill(L'0') << std::setw(2) << num_concat_msgs
				<< std::hex << std::setfill(L'0') << std::setw(2) << ( i + 1);
		}
		ss << encode_hex_value(msg_chars, 4);
		strings.push_back(ss.str());
	}
	return strings;
}

void Message::decode(std::wstring input, CompList* comp_list)
{
	//Extract strings
	std::wstringstream ss;
	ss.clear();
	ss.str(input);
	if (input.find('\n') > 0)
	{
		ss.ignore(256, '\n');
	}
	ss >> raw_pdu;
	ss.clear();
	ss.seekg(0, std::ios::beg);
	ss.str(raw_pdu);

	int service_center_number_length = extract_hex_value(ss, 2);
	if (service_center_number_length < 0)
		return;
	int service_center_type = extract_hex_value(ss, 2);
	std::wstring service_center_number = extract_phone_number(ss, service_center_number_length * 2 - 2);
	int pdu_header = extract_hex_value(ss, 2);
	bool has_udh = false;
	{
		has_udh = (pdu_header & 0x40) != 0;
	}
	
	int sender_number_length = extract_hex_value(ss, 2);
	if (sender_number_length < 0)
		return;
	int sender_number_type = extract_hex_value(ss, 2);
	sender_number = extract_phone_number(ss, sender_number_length);
	int protocol_id = extract_hex_value(ss, 2);
	data_coding = extract_hex_value(ss, 2);
	std::wstring time = extract_phone_number(ss, 14);
	msg_length = extract_hex_value(ss, 2);
	if (msg_length < 0)
		return;
	std::wstring udh;

	contents = L"";
	if (data_coding == 8)
	{
		int i = 0;
		if (has_udh)
		{
			int udhl = extract_hex_value(ss, 2);			//Length of the user data header
			udh.push_back(extract_hex_value(ss, 2));		//IEI
			extract_hex_value(ss, 2);						//IEDL
			udh.push_back(extract_hex_value(ss, 2));		//IED: refnum
			udh.push_back(extract_hex_value(ss, 2));		//IED: num of concat msgs
			udh.push_back(extract_hex_value(ss, 2));		//IED: concat index of this msg
			i += udhl;
		}
		int value = extract_hex_value(ss, 4);
		while (ss.good())
		{
			contents.push_back(value);		//Extract in 2-byte octet pairs
			value = extract_hex_value(ss, 4);
		}
	}
	else
	{
		std::vector<int> packed_data;
		std::wstring str = ss.str();

		int byte = extract_hex_value(ss, 2);
		while (ss.good())
		{
			packed_data.push_back(byte);
			byte = extract_hex_value(ss, 2);
		}

		int udhl = 0;
		if (has_udh)
		{
			udhl = packed_data[0];					//Length of the user data header
			udh.push_back(packed_data[1]);					//IEI
														//IEDL
			udh.push_back(packed_data[3]);					//IED: refnum
			udh.push_back(packed_data[4]);					//IED: num of concat msgs
			udh.push_back(packed_data[5]);					//IED: concat index of this msg
		}
		std::wstring all_contents = unpack_septets(packed_data);
		int udh_num_septets = has_udh ? (((udhl + 1) * 8) + 6) / 7 : 0;
		contents = all_contents.substr(udh_num_septets, all_contents.length() - udh_num_septets);
	}
	if (has_udh)
	{
		concatenated = (udh[0] == 0x0) || (udh[0] == 0x8);
		concat_refnum = udh[1];
		concat_num_msgs = udh[2];
		concat_index = udh[3];
	}
	else
	{
		concatenated = false;
		concat_refnum = 0;
		concat_index = 0;
		concat_num_msgs = 0;
	}

	if (comp_list && comp_list->areas.count(sender_number) > 0)
	{
		sender_name = comp_list->areas[sender_number].area_name;
	}
	else
		sender_name = sender_number;

	int cmgl_id_start = input.find(L"CMGL: ");
	if (cmgl_id_start != std::wstring::npos)
	{
		cmgl_id_start += 6;
		int cmgl_id_end = input.find(L",", cmgl_id_start);
		cmgl_ids.push_back(_wtoi(input.substr(cmgl_id_start, cmgl_id_end - cmgl_id_start).c_str()));
	}
	else
	{
		cmgl_ids.clear();
	}
	replace_chars(contents, L"\u0011", L"_");
}

void Message::read(const Value& m, CompList* comp_list)
{
	sender_name = tow(m[g_jk_sender_name.c_str()].GetString());
	sender_number = tow(m[g_jk_sender_number.c_str()].GetString());
	contents = tow(m[g_jk_contents.c_str()].GetString());
	sent_date = tow(m[g_jk_sent_date.c_str()].GetString());
	data_coding = m[g_jk_data_coding.c_str()].GetInt();
	msg_length = m[g_jk_msg_len.c_str()].GetInt();
	concatenated = m[g_jk_concat.c_str()].GetBool();
	concat_refnum = m[g_jk_concat_refnum.c_str()].GetInt();
	concat_num_msgs = m[g_jk_concat_num_msg.c_str()].GetInt();
	concat_index = m[g_jk_concat_index.c_str()].GetInt();
	replace_chars(contents, L"\u0011", L"_");

	if (sender_name == sender_number && comp_list->areas.count(sender_number) > 0)
	{
		sender_name = comp_list->areas[sender_number].area_name;
	}
}

void Message::write(Document* d)
{
		Value v;
		v.SetObject();
		Document::AllocatorType& a = d->GetAllocator();
		
		v.AddMember(Value(g_jk_sender_name.c_str(), a), Value(tos(sender_name).c_str(), a), a);
		v.AddMember(Value(g_jk_sender_number.c_str(), a), Value(tos(sender_number).c_str(), a), a);
		v.AddMember(Value(g_jk_contents.c_str(), a), Value(tos(contents).c_str(), a), a);
		v.AddMember(Value(g_jk_sent_date.c_str(), a), Value(tos(sent_date).c_str(), a), a);
		v.AddMember(Value(g_jk_data_coding.c_str(), a), data_coding, a);
		v.AddMember(Value(g_jk_msg_len.c_str(), a), msg_length, a);
		v.AddMember(Value(g_jk_concat.c_str(), a), concatenated, a);
		v.AddMember(Value(g_jk_concat_refnum.c_str(), a), concat_refnum, a);
		v.AddMember(Value(g_jk_concat_num_msg.c_str(), a), concat_num_msgs, a);
		v.AddMember(Value(g_jk_concat_index.c_str(), a), concat_index, a);

		d->PushBack(v, a);
}