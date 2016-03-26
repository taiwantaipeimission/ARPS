#include "codes.h"
#include "Message.h"
#include "Area.h"
#include "utility.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

#define OLDVER

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
	for (int i = 0; i < chars.size(); i++)
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
	std::wstring number = PH_NUMBER_HEAD;
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

	int num_concat_msgs = (contents.length() * 2) / MAX_MSG_LEN + 1;
	std::wstring remaining_contents(contents);
	std::vector<std::wstring> strings;

	int msg_id = rand() % 256;
	for (int i = 0; i < num_concat_msgs; i++)
	{
		int this_msg_length = (std::min)(remaining_contents.length(), (size_t)(MAX_MSG_LEN / 2));
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
	std::wstring forbidden_chars = L"";
	forbidden_chars += MSG_FILE_FIELD_SEPARATOR;
	forbidden_chars += MSG_SEPARATOR;
	strip_chars(contents, forbidden_chars);
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
}

void Message::read(std::wstring input)
{
#ifdef OLDVER
	std::wstringstream ss(input);

	std::getline(ss, sender_name, MSG_FILE_FIELD_SEPARATOR);
	std::getline(ss, sender_number, MSG_FILE_FIELD_SEPARATOR);
	std::getline(ss, contents, MSG_FILE_FIELD_SEPARATOR);
	std::getline(ss, sent_date, MSG_FILE_FIELD_SEPARATOR);
	std::wstring data_coding;
	std::getline(ss, data_coding, MSG_FILE_FIELD_SEPARATOR);
	data_coding = _wtoi(data_coding.c_str());
	std::wstring msg_length;
	std::getline(ss, msg_length, MSG_FILE_FIELD_SEPARATOR);
	msg_length = _wtoi(msg_length.c_str());
	std::wstring concatenated;
	std::getline(ss, concatenated, MSG_FILE_FIELD_SEPARATOR);
	concatenated = _wtoi(concatenated.c_str()) == 0 ? false : true;
	std::wstring concat_refnum;
	std::getline(ss, concat_refnum, MSG_FILE_FIELD_SEPARATOR);
	concat_refnum = _wtoi(concat_refnum.c_str());
	std::wstring concat_num_msgs;
	std::getline(ss, concat_num_msgs, MSG_FILE_FIELD_SEPARATOR);
	concat_num_msgs = _wtoi(concat_num_msgs.c_str());
	std::wstring concat_index;
	std::getline(ss, concat_index, MSG_FILE_FIELD_SEPARATOR);
	concat_index = _wtoi(concat_index.c_str());
#else






#endif
}

std::wstring Message::write()
{
	std::wstringstream ss;
	ss << sender_name << MSG_FILE_FIELD_SEPARATOR
		<< sender_number << MSG_FILE_FIELD_SEPARATOR
		<< contents << MSG_FILE_FIELD_SEPARATOR
		<< sent_date << MSG_FILE_FIELD_SEPARATOR
		<< data_coding << MSG_FILE_FIELD_SEPARATOR
		<< msg_length << MSG_FILE_FIELD_SEPARATOR
		<< concatenated << MSG_FILE_FIELD_SEPARATOR
		<< concat_refnum << MSG_FILE_FIELD_SEPARATOR
		<< concat_num_msgs << MSG_FILE_FIELD_SEPARATOR
		<< concat_index;
	std::wstring return_str = ss.str();
	return return_str;
}