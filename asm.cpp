/*****************************************************************************
TITLE: Assembler, Emulator
AUTHOR:   Munesh Meena
ROLLNO:   2101CS47
Declaration of Authorship
This txt file, claims.txt, is part of the assignment of CS322 at the
department of Computer Science and Enginering, IIT Patna .
*****************************************************************************/
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <string.h>
#include <map>
#include <set>
#include <iomanip>
using namespace std;

const int MAXN = 1e5 + 5;

map<string, int> labels;
map<pair<string, int>, int> inst; // inst---->instructions
map<string, bool> visit_labels;
map<string, char> string_map;
set<pair<int, string>> defect;
set<pair<int, string>> warnings;
string fName;
vector<string> m_code(MAXN);
vector<string> sourcecode[MAXN];
int m_int_code[MAXN];


void initialize()
{
	string_map["0000"] = '0', string_map["0001"] = '1', string_map["0010"] = '2', string_map["0011"] = '3', string_map["0100"] = '4';
	string_map["0101"] = '5', string_map["0110"] = '6', string_map["0111"] = '7', string_map["1000"] = '8', string_map["1001"] = '9';
	string_map["1010"] = 'A', string_map["1011"] = 'B', string_map["1100"] = 'C', string_map["1101"] = 'D', string_map["1110"] = 'E', string_map["1111"] = 'F';

//inst ----> instruction
	inst[{"add", 1}] = 6, inst[{"sub", 1}] = 7, inst[{"shl", 1}] = 8;
	inst[{"shr", 1}] = 9, inst[{"a2sp", 1}] = 11, inst[{"sp2a", 1}] = 12;
	inst[{"return", 1}] = 4, inst[{"HALT", 1}] = 18, inst[{"ldc", 2}] = 0;
	inst[{"adc", 2}] = 1, inst[{"ldl", 2}] = 2, inst[{"stl", 2}] = 3;
	inst[{"ldnl", 2}] = 4, inst[{"stnl", 2}] = 5, inst[{"adj", 2}] = 10;
	inst[{"call", 2}] = 13, inst[{"brz", 2}] = 15, inst[{"brlz", 2}] = 16;
	inst[{"br", 2}] = 17, inst[{"SET", 2}] = 19, inst[{"data", 2}] = 20;
	inst[{"call", 3}] = 13, inst[{"brz", 3}] = 15, inst[{"brlz", 3}] = 16;
	inst[{"br", 3}] = 17;
}

bool check_Valid_label(string s){
    // must start with an alphabet
	if (s[0] <= '9'&& s[0] >= '0'){
		return false; 
	}
    // Character other than alphabets and numbers;
	for (char c : s){
		if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_') || (c >= '0' && c <= '9'))){
			return false; 
		}
	}

	return true; // valid label
}

// display the compilation errors.s
void show_errors(){
	cout << "The program is the following some kind of the error :\n";
	for (auto e : defect){
		cout << e.first << ": " << e.second << endl;
	}
}

void show_warnings(){
	cout << "The program issues the following warning :\n";
	for (auto w : warnings){
		cout << w.first << ": " << w.second << endl;
	}
}

string erase_white_space(string s){
	string ans = "";
	int n = s.size();
	int i = 0;
	while (i < n && s[i] == ' '){
		i++;
	}
	bool previous = false;
	while (i < n){
		if (s[i] != ' '){
			ans += s[i];
			i++;
			previous = false;
		}
		else if (s[i] == ' ' && previous){
			i++;
		}
		else{
			ans += s[i];
			previous = true;
		}
	}
	if (ans[ans.size() - 1] == ' '){
		ans = ans.substr(0, ans.size() - 1);
	}
	return ans;
}

char convert_bin_hex(string s){
	return string_map[s];
}
// function to convert decimal into binary 2s complement
string convert_d_b2s(int n, int bits){

	const size_t BITS = bits;
	char bin_str[BITS + 1];
	for (unsigned int i = 0; i < BITS; i++){
		unsigned int mask = 1u << (BITS - 1 - i);
		bin_str[i] = (n & mask) ? '1' : '0';
	}
	bin_str[BITS] = '\0';

	string ret = "";
	for (unsigned int i = 0; i < BITS; i += 4){
		string tmp = "";
		tmp += bin_str[i];
		tmp += bin_str[i + 1];
		tmp += bin_str[i + 2];
		tmp += bin_str[i + 3];
		ret += convert_bin_hex(tmp);
	}
	return ret;
}



void firstPass(string s, int *p, int line)  // Function to throw errors and identify labels
{

	if (s != ""){

		string commands[5] = {"", "", "", "", ""};
		string temp;
		for (auto c : s){
			temp += c;
			if (c == ':'){
				temp += ' ';
			}
		}

		// Extracting words from line
		stringstream sinn(temp);
		int i = 0;
		while (sinn >> commands[i]){
			i++;
			//	cout<<words[i]<<" ";
		}

		for (int i = 0; i < 5; i++){

			if (commands[i] != "")
			{
				sourcecode[(*p)].push_back(commands[i]);
			}
			// cout<<sourcecode[(*p)][i]<<endl;
		}

		if (commands[0][commands[0].length() - 1] == ':'){
			string label_name = commands[0].substr(0, commands[0].length() - 1);

			// checking if the label name is valid
			if (check_Valid_label(label_name)){
				if (labels.find(label_name) != labels.end()){
					defect.insert({line, "Error!Duplicate labels aren't allowed ."});
				}
				else{
					labels[label_name] = *p;
					if (commands[1].length() > 0 or commands[2].length() > 0)
					{
						(*p)++;
					}
				}
			}
			else{
				defect.insert({line, "Error! Invalid label name  [alphanumeric, starting value can only be letters] ."});
			}

			// If some other innstruction is present in the line or not?
			if (commands[1] != "" && commands[1][commands[1].length() - 1] == ':'){

				string Mnemonic_words_Table = commands[1]; // Mnemonic_words_Table of the given word.
				int id = 0;
				for (auto j : inst){
					if (j.first.first == commands[1]){
						id = j.first.second;
						break;
					}
				}
				if (id == 0){
					defect.insert({line, "There isn't  any such instruction"});
				}

				else if (id == 1){
				
					if (commands[2] != ""){				
						defect.insert({line, "Error! No operands are allowed for this operand  ."});
					}
				}
				else{
					if (commands[2] == ""){
						defect.insert({line, "Error! There must be an operand."});
					}

					if (commands[3] != ""){
						defect.insert({line, "Error! This operaton allows one operand only."});
					}
				}
			}
		}
		else{

			string Mnemonic_words_Table = commands[0];
			int id = 0;
			for (auto j : inst){
				if (j.first.first == commands[0]){
					id = j.first.second;
					break;
				}
			}

			if (id == 0){
				defect.insert({line, "There isn't  any such instruction "});
			}
			else if (id == 1){
				if (commands[1] != ""){
					defect.insert({line, "Error! No operands are allowed for this operand."});
				}
			}
			else{
				if (commands[1] == ""){
					defect.insert({line, "Error! There must be an operand."});
				}

				if (commands[2] != ""){
					defect.insert({line, "Error! This operand allows one operation only"});
				}
			}

			(*p)++;
		}
	}
}

int checkValidoperand(string s){

	if (check_Valid_label(s) and labels.find(s) == labels.end()){
		return 1; // label doesn't exist
	}
	if (check_Valid_label(s) and labels.find(s) != labels.end()){
		return 5; // already present label
	}

	int len = s.length();
	int ok = 3;

	for (int i = 0; i < len; i++){
		if (!(s[i] >= '0' and s[i] <= '7'))
			ok = 2;
	}

	if (ok == 3){
		return 3; // if it is octal number
	}
	for (int i = 0; i < len; i++){
		if (!(s[i] >= '0' and s[i] <= '9'))
			ok = 4; // if valid hexadecimal
	}

	if (ok == 2){
		return 2; // if valid decimal
	}
	for (int i = 0; i < len; i++){
		if (!((s[i] >= 'a' and s[i] <= 'f') or (s[i] >= 'A' and s[i] <= 'F') or (s[i] >= '0' and s[i] <= '9')))
			ok = 0;
	}

	if (ok == 4){
		return 4;
	}
	return 0; // invalid
}

int String_To_Decimal(string operand, int bits){
	if (bits == 32 || bits == 24){
		int cur = 0;
		int len = operand.length();
		if (operand == "0")
			return 0;
		// if string is a existing label
		if (checkValidoperand(operand) == 5){
			return cur = labels[operand];
		}
		else if (checkValidoperand(operand) == 1){
			// string is a non-existing label
			defect.insert({0, "No such Label"});
			return 0;
		}
		// string starts with 'positive' sign
		if (operand[0] == '+'){
			// string is octal
			if (operand[1] == '0'){
				if (checkValidoperand(operand.substr(2, len - 2)) == 3){
					int mul = 1;
					for (int i = len - 1; i >= 2; i--){
						cur += ((operand[i] - '0') * mul);
						mul *= 8;
					}
				}
				else
					defect.insert({0, "The specified number format is not valid, Use Suitable Prefix For hex = '0x', oct = '0'"});
			}
			else{
				// string is decimal
				if (checkValidoperand(operand.substr(1, len - 1)) == 3 or checkValidoperand(operand.substr(1, len - 1)) == 2)
					cur = stoi(operand.substr(1, len - 1));
				else
					defect.insert({0, "The specified number format is not valid,Use Suitable Prefix For hex = '0x', oct = '0'"});
			}
			return cur;
		}

		// string starts with 'negative' sign
		if (operand[0] == '-')
		{
			// string is octal
			if (operand[1] == '0'){
				if (checkValidoperand(operand.substr(2, len - 2)) == 3){
					int multiplier = 1;
					for (int i = len - 1; i >= 2; i--)
					{
						cur += ((operand[i] - '0') * multiplier);
						multiplier *= 8;
					}
					cur *= -1;
				}
				else
					defect.insert({0, "The specified number format is not valid, Use Suitable Prefix For hex = '0x', oct = '0'"});
			}
			else{
				// string is decimal
				if (checkValidoperand(operand.substr(1, len - 1)) == 3 or checkValidoperand(operand.substr(1, len - 1)) == 2)
				{
					cur = stoi(operand.substr(1, len - 1));
					cur *= -1;
				}
				else{
					defect.insert({0, "The specified number format is not valid,Use Suitable Prefix For hex = '0x', oct = '0'"});
				}
			}
			return cur;
		}

		// string is hexadecimal
		if (operand[0] == '0' && operand[1] == 'x'){
			if (checkValidoperand(operand.substr(2, len - 2)) == 3 or checkValidoperand(operand.substr(2, len - 2)) == 2 or checkValidoperand(operand.substr(2, len - 2)) == 4){
				unsigned int x;
				string s1 = operand.substr(2, len - 2);
				while ((unsigned int)s1.length() < (unsigned int)(bits) / 4){
					s1.insert(0, "0");
				}
				stringstream ss;
				ss << std::hex << s1;
				ss >> x;
				cur = static_cast<int>(x);
			}
			else
				defect.insert({0, "The specified number format is not valid,Use Suitable Prefix For hex = '0x', oct = '0'"});
		}
		else if (operand[0] == '0'){
			// string is octal
			if (checkValidoperand(operand.substr(1, len - 1)) == 3){
				cur = 0;
				int multiplier = 1;
				for (int i = len - 1; i >= 1; i--)
				{
					cur += ((operand[i] - '0') * multiplier);
					multiplier *= 8;
				}
			}
			else
				defect.insert({0, "The specified number format is not valid,Use Suitable Prefix For hex = '0x', oct = '0'"});
		}
		else{
			// string is decimal
			if (checkValidoperand(operand) == 3 or checkValidoperand(operand) == 2)
				cur = stoi(operand);
			else
				defect.insert({0, "The specified number format is not valid,Use Suitable Prefix For hex = '0x', oct = '0'"});
		}
		return cur;
	}
	else{
		// string is Mnemonic_words_Table

		int id;
		for (auto j : inst){
			if (j.first.first == operand){
				id = j.second;
				break;
			}
		}

		return id;
	}
	return 0;
}

void convert_m_code(int type, string Mnemonic_words_Table, int i, string operand, string lb){

	if (type == 1){

		if (Mnemonic_words_Table == "SET"){

			visit_labels[lb] = true;
			// cout<<operand<<" ";
			int cur = String_To_Decimal(operand, 32);
			// cout<<cur<<" _ "<<endl;
			m_int_code[i] = cur;
			m_code[i] = convert_d_b2s(cur, 32);
		}
		else if (Mnemonic_words_Table == "data"){
			visit_labels[lb] = true;
			// cout<<operand<<" ";
			int cur = String_To_Decimal(operand, 32);
			// cout<<cur<<" _ "<<endl;
			m_int_code[i] = cur;
			m_code[i] = convert_d_b2s(cur, 32);
		}
		else{

			int value2 = String_To_Decimal(Mnemonic_words_Table, 8);
			int value1 = String_To_Decimal(operand, 24);

			int id = -1;
			for (auto j : inst){
				if (j.first.first == Mnemonic_words_Table)
				{
					id = j.first.second;
				}
			}

			if (id == 3){
				if (check_Valid_label(operand))
					value1 = (value1 - i) - 1;
			}
			m_int_code[i] = value1 << 8 | value2;
			// cout<<m_int_code[i]<<"|"<<endl;
			m_code[i] = convert_d_b2s(value1, 24) + convert_d_b2s(value2, 8);
		}
	}
	else if (type == 0){
		m_int_code[i] = inst[{Mnemonic_words_Table, 1}];
		// cout<<m_int_code[i]<<"|"<<endl;
		m_code[i] = convert_d_b2s(inst[{Mnemonic_words_Table, 1}], 32);
	}
}
void second_pass(){

	for (int i = 0; i < MAXN; i++){
		int sz = sourcecode[i].size();

		if (sourcecode[i].empty()){
			break;
		}

		if (sz == 1){
			convert_m_code(0, sourcecode[i][0], i, "", "");
			continue;
		}

		string Mnemonic_words_Table, operand;
		string p = sourcecode[i][0], q = sourcecode[i][1];
		bool ok1 = (p[p.length() - 1] == ':'), ok2 = (q[q.length() - 1] == ':');

		if (ok1 && ok2){

			Mnemonic_words_Table = sourcecode[i][2];

			int id = -1;
			for (auto j : inst){
				if (j.first.first == Mnemonic_words_Table){
					id = j.first.second;
					break;
				}
			}

			if (id == 1){
				convert_m_code(0, Mnemonic_words_Table, i, "", "");
			}
			else{
				if (sz >= 4)
					operand = sourcecode[i][3];

				if (check_Valid_label(operand) and labels.find(operand) != labels.end())
					visit_labels[operand] = true;

				convert_m_code(1, Mnemonic_words_Table, i, operand, q.substr(0, q.length() - 1));
			}
		}
		else if (ok1 && !ok2){

			// instruction after a single label
			Mnemonic_words_Table = sourcecode[i][1];

			int id = -1;
			for (auto j : inst){
				if (j.first.first == Mnemonic_words_Table){
					id = j.first.second;
					break;
				}
			}

			if (id == 1){
				convert_m_code(0, Mnemonic_words_Table, i, "", "");
			}
			else{
				if (sz >= 3)
					operand = sourcecode[i][2];

				if (check_Valid_label(operand) and labels.find(operand) != labels.end())
					visit_labels[operand] = true;

				convert_m_code(1, Mnemonic_words_Table, i, operand, p.substr(0, p.length() - 1));
			}
		}
		else{
			// instruction without label
			Mnemonic_words_Table = sourcecode[i][0];

			int id = -1;
			for (auto j : inst){
				if (j.first.first == Mnemonic_words_Table){
					id = j.first.second;
					break;
				}
			}

			if (id == 1){
				convert_m_code(0, Mnemonic_words_Table, i, "", "");
			}
			else{
				if (sz >= 2)
					operand = sourcecode[i][1];

				if (check_Valid_label(operand) && labels.find(operand) != labels.end()){
					visit_labels[operand] = true;
				}

				convert_m_code(1, Mnemonic_words_Table, i, operand, "");
			}
		}
	}
}

void gen_log(){
	string s = fName;
	s += ".log";
	fstream log_fprint;
	log_fprint.open(s, ios::out);

	if (log_fprint.is_open()){
		log_fprint << "Below are the label names and values :\n";
		for (auto lb : labels){
			log_fprint << lb.first << " => " << lb.second << "\n";
		}

		log_fprint << "\nBelow are the label names and values which have been used:\n";
		for (auto j : visit_labels)
		{
			if (j.second == true)
				log_fprint << j.first << "\n";
		}

		log_fprint << "\nWarnings: \n";
		for (auto w : warnings)
		{
			log_fprint << "Line " << w.first << " : " << w.second << "\n";
		}

		log_fprint << "\nErrors: \n";
		for (auto e : defect){
			log_fprint << "Line " << e.first << " : " << e.second << "\n";
		}
	}
	log_fprint.close();
	return;
}

int main(int argc, char **argv){

	// initializing the global variables
	initialize();

	if (argc != 2)
	{
		cout << "The given command is invalid. Please recheck for the file format.\n";
	}

	fstream in_ptr;
	in_ptr.open(argv[1], ios::in);
	fName = "";

	// Extracting out the file name
	for (unsigned int i = 0; i < strlen(argv[1]); i++)
	{
		if (argv[1][i] == '.')
			break;
		fName.push_back(argv[1][i]);
	}

	if (!in_ptr.is_open()){
		defect.insert({0, ""});
	}
	else{

		int line = 1;
		int pc = 0; // Program counter
		string LineData;

		// Extract useful data line by line
		while (getline(in_ptr, LineData)){
			string s;
			for (auto c : LineData){
				if (c == ';')
					break;
				s += c;
			}

			firstPass(s, &pc, line); // The extracted information is passed to identify labels and throw assembly errors
			line++;
		}

		// modifying the labels here
		for (int i = 0; i < MAXN; i++){
			if (sourcecode[i].empty())
				break;

			if (sourcecode[i].size() == 3 and sourcecode[i][1] == "SET" and labels.find(sourcecode[i][0].substr(0, sourcecode[i][0].length() - 1)) != labels.end())
			{
				labels[sourcecode[i][0].substr(0, sourcecode[i][0].length() - 1)] = String_To_Decimal(sourcecode[i][2], 32);
			}
			else if (sourcecode[i].size() == 4 and sourcecode[i][2] == "SET" and labels.find(sourcecode[i][1].substr(0, sourcecode[i][1].length() - 1)) != labels.end())
			{
				labels[sourcecode[i][1].substr(0, sourcecode[i][1].length() - 1)] = String_To_Decimal(sourcecode[i][3], 32);
			}
		}

		//-----------

		// Used to convert given assmebly to machine code and along with that identify more errors
		second_pass();

		

		// Identifying the labels which haven't been used.
		for (auto it = labels.begin(); it != labels.end(); it++){
			string s = it->first;
			if (visit_labels.find(s) == visit_labels.end())
				warnings.insert({0, "Unused Label"});
		}
	}

	// Writing labels, errors, warnings into a log file
	gen_log();

	// generating listing file
	fstream List_fprint;
	string s = fName;
	s += ".lst";
	List_fprint.open(s, ios::out);

	if (List_fprint.is_open()){
		for (int i = 0; i < MAXN; i++){
			if (sourcecode[i].empty())
				break;

			List_fprint << convert_d_b2s(i, 32) << " ";
			List_fprint << m_code[i] << " ";

			for (unsigned int k = 0; k < sourcecode[i].size(); k++){
				List_fprint << sourcecode[i][k] << " ";
			}
			List_fprint << "\n";
		}
	}
	List_fprint.close();
	// Closing the listing file

	// Throw errors if present
	if (!defect.empty())
	{
		// print all errors along with existing warnings
		show_errors();
		if (!warnings.empty()){
			show_warnings();
		}
	}
	else
	{
		cout << "Great! Program finished with zero errors.\n";
		if (!warnings.empty())
			show_warnings();
		// generating object file
		FILE *object_fprint;
		string s = fName;
		s += ".o";
		object_fprint = fopen(s.c_str(), "wb");
		int len = 0;
		for (int i = 0; i < MAXN; i++){
			if (sourcecode[i].empty())
				break;
			len++;
		}
		fwrite(m_int_code, sizeof(int), len, object_fprint);
		fclose(object_fprint);
		// closed the object file
	}

	// Closing source file
	in_ptr.close();
	return 0;
}