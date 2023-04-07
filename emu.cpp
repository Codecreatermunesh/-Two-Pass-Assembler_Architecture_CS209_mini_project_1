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
size_t num;
string fName;
string options;
string mnemonic,operand;
int opcode,val_offset;
map < string,char > string_map;
map < int,string > inst_without_operand; //inst----> instruction
map < int,string > inst_with_one_operand;
map < int,string > inst_with_pcoffset;
set < pair<int,pair <string,string> > > isa;
vector <string> m_code(MAXN);
int m_int_code[MAXN];
int i_cnt = 0;
int registerA, registerB, sp, pc;

// Insert instructions along with their opcode and also categorize instructions according to their format
void initialize(){   


	string_map["0000"] = '0',string_map["0001"] = '1',string_map["0010"] = '2',string_map["0011"] = '3';
	string_map["0100"] = '4',string_map["0101"] = '5',string_map["0110"] = '6',string_map["0111"] = '7';
	string_map["1000"] = '8',string_map["1001"] = '9',string_map["1010"] = 'A',string_map["1011"] = 'B';
	string_map["1100"] = 'C',string_map["1101"] = 'D',string_map["1110"] = 'E',string_map["1111"] = 'F';
	
	inst_without_operand.insert({6,"add"}),inst_without_operand.insert({7,"sub"}),inst_without_operand.insert({8,"shl"});
	inst_without_operand.insert({9,"shr"}),inst_without_operand.insert({11,"a2sp"}),inst_without_operand.insert({12,"sp2a"});
	inst_without_operand.insert({14,"return"}),inst_without_operand.insert({18,"HALT"});


	inst_with_one_operand.insert({0,"ldc"}),inst_with_one_operand.insert({1,"adc"});
	inst_with_one_operand.insert({2,"ldl"}),inst_with_one_operand.insert({3,"stl"});
	inst_with_one_operand.insert({4,"ldnl"}),inst_with_one_operand.insert({5,"stnl"});
	inst_with_one_operand.insert({10,"adj"}),inst_with_one_operand.insert({13,"call"});
	inst_with_one_operand.insert({15,"brz"}),inst_with_one_operand.insert({16,"brlz"});
	inst_with_one_operand.insert({17,"br"}),inst_with_one_operand.insert({19,"SET"});
	inst_with_one_operand.insert({20,"data"});


	inst_with_pcoffset.insert({13,"call"}),inst_with_pcoffset.insert({15,"brz"});
	inst_with_pcoffset.insert({16,"brlz"}),inst_with_pcoffset.insert({17,"br"});
	

	isa.insert({0,{"ldc","value"}}),isa.insert({1,{"adc","value"}}),isa.insert({2,{"ldl","value"}});
	isa.insert({3,{"stl","value"}}),isa.insert({4,{"ldnl","value"}}),isa.insert({5,{"ldnl","value"}});
	isa.insert({6,{"add",""}}),isa.insert({7,{"sub",""}}),isa.insert({8,{"shl",""}}),isa.insert({9,{"shr",""}});
	isa.insert({10,{"adj","value"}}),isa.insert({11,{"a2sp",""}}),isa.insert({12,{"sp2a",""}});
	isa.insert({13,{"call","offset"}}),isa.insert({14,{"return",""}}),isa.insert({15,{"brz","offset"}});
	isa.insert({16,{"brlz","offset"}}),isa.insert({17,{"br","offset"}}),isa.insert({18,{"HALT",""}});
	
}

int s_2_d_24(string s){
	int cur = 0;

	unsigned int x;
	string s1 = s;

	if(s[0] == '8' or s[0] == '9' or s[0] == 'A' or s[0] == 'B' or s[0] == 'C' or s[0] == 'D' or s[0] == 'E' or s[0] == 'F'){
		while(s1.length() < 8){
			s1.insert(0,"F");
		}
	}
	else{
		while(s1.length() < 8){
			s1.insert(0,"0");
		}
	}

	stringstream ss;
	ss << std::hex << s1;
	ss >> x;
	cur = static_cast<int>(x);
		
	return cur;

}

// Convert string to decimal accordingly
int s_2_d(string s,int no_of_bits){
	int value = 0;
		
	unsigned int x;
	string s1 = s;
	while(s1.length() < (unsigned int) (no_of_bits)/4){
		s1.insert(0,"0");
	}
	stringstream ss;
	ss << std::hex << s1;
	ss >> x;
	value = static_cast<int>(x);
		
	return value;	
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
		if((n & mask)){
			bin_str[i]='1';
		}
		else{
			bin_str[i]='0';
		}
		
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

void execute(){
	for(pc=0;(unsigned int)pc<num;pc++){
		i_cnt++;
		opcode = s_2_d(m_code[pc].substr(6,2),8);

		val_offset = s_2_d_24(m_code[pc].substr(0,6));
        
		if(inst_without_operand.find(opcode) != inst_without_operand.end()){
			if(options == "-trace")
			printf("PC=%s, SP=%s, A=%s, B=%s, %s\n",convert_d_b2s(pc,32).c_str(),convert_d_b2s(sp,32).c_str(),convert_d_b2s(registerA,32).c_str(),convert_d_b2s(registerB,32).c_str(),inst_without_operand[opcode].c_str());
		
			if(opcode == 6){
				registerA = registerB + registerA;
			}
			else if(opcode == 7){
				registerA = registerB - registerA;
			}
			else if(opcode == 8){
				registerA = (registerB << registerA);
			}
			else if(opcode == 9){
			
				registerA = (registerB >> registerA);
			}
			else if(opcode == 11){
			
				sp = registerA;
				registerA = registerB;
			}
			else if(opcode == 12){
			
				registerB = registerA;
				registerA = sp;
			}
			else if(opcode == 14){
			
				pc = registerA;
				registerA = registerB;
			}
			else if(opcode == 18){
				if(options == "-trace")
				cout << "Program Execution Finished\n";
				return;
			}
			else{
				cout << "Invalid Instruction, Program Execution Stopped\n";
				return;
			}
		}
		else if(inst_with_one_operand.find(opcode) != inst_with_one_operand.end()){
			if(options == "-trace")
			printf("PC=%s, SP=%s, A=%s, B=%s, %s %s\n",convert_d_b2s(pc,32).c_str(),convert_d_b2s(sp,32).c_str(),convert_d_b2s(registerA,32).c_str(),convert_d_b2s(registerB,32).c_str(),inst_with_one_operand[opcode].c_str(),convert_d_b2s(val_offset,32).c_str());

	
			if(inst_with_pcoffset.find(opcode) != inst_with_pcoffset.end()){	
				if(opcode == 13){
					registerB = registerA;
					registerA = pc;
					pc = pc + val_offset;
				}
				else if(opcode == 15){	
					if(registerA == 0)
						pc = pc + val_offset;
				}
				else if(opcode == 16){
					if(registerA < 0)
						pc = pc + val_offset;
				}
				else if(opcode == 17){	
					pc = pc + val_offset;
				}
				else{
					cout << "Invalid Instruction, Program Execution Stopped\n";
					return;
				}
			}
			else
			{
				if(opcode == 0){
					registerB = registerA;
					registerA = val_offset;
				}
				else if(opcode == 1){
					registerA = registerA + val_offset;
				}
				else if(opcode == 2){
					registerB = registerA;
					if(sp + val_offset < 0 or sp + val_offset > MAXN){
						if(options == "-trace")
						cout << "ERROR: The memory being accessed is not valid\n";
						return;
					}
					registerA = m_int_code[sp+val_offset];
					if(options == "-read"){
						printf("Reading Memory[%s] finds %s\n",convert_d_b2s(sp+val_offset,32).c_str(),convert_d_b2s(registerA,32).c_str());
					}

				}
				else if(opcode == 3){
					if(sp + val_offset < 0 or sp + val_offset > MAXN){
						if(options == "-trace")
						cout << "ERROR: The memory being accessed is not valid\n";
						return;
					}
					if(options == "-write"){
						printf("Writing Memory[%s] was %s ",convert_d_b2s(sp+val_offset,32).c_str(),convert_d_b2s(m_int_code[sp+val_offset],32).c_str());
					}
					m_int_code[sp+val_offset] = registerA;
					if(options == "-write")
						printf("now %s\n",convert_d_b2s(registerA,32).c_str());
					registerA = registerB;
				}
				else if(opcode == 4){
					if(registerA + val_offset < 0 or registerA + val_offset > MAXN){
						if(options == "-trace")
						cout << "ERROR: The memory being accessed is not valid\n";
						return;
					}
					if(options == "-read"){
						printf("Reading Memory[%s] finds %s\n",convert_d_b2s(registerA+val_offset,32).c_str(),convert_d_b2s(m_int_code[registerA+val_offset],32).c_str());
					}
					registerA = m_int_code[registerA+val_offset];				
				}
				else if(opcode == 5){
					if(registerA + val_offset < 0 or registerA + val_offset > MAXN){
						if(options == "-trace")
						cout << "ERROR: The memory being accessed is not valid\n";
						return;
					}
					if(options == "-write"){
						printf("Writing Memory[%s] was %s ",convert_d_b2s(registerA+val_offset,32).c_str(),convert_d_b2s(m_int_code[registerA+val_offset],32).c_str());
					}
					m_int_code[registerA+val_offset] = registerB;
					if(options == "-write")
						printf("now %s\n",convert_d_b2s(registerB,32).c_str());
					
				}
				else if(opcode == 10){
					sp = sp + val_offset;
				}
				else{
					cout << "Invalid Instruction, Program Execution Stopped\n";
					return;
				}
			}
		}

		if((unsigned int)pc >= num-1){
			if(options == "-trace")
			cout << "ERROR: The memory being accessed is not valid\n";
			return;
		}
	}
}


int main(int argc,char** argv)
{
	// Store all instructions with their format specifications
	initialize();

	// Check if proper arguments are provided or not 
	if(argc != 3){
		cout << "Invalid Command, the file format is not correct. : ./emu [options] filename.o\n";
		cout << "-trace  display instruction trace\n";
		cout << "-read   display memory reads\n";
		cout << "-write  display memory writes\n";
		cout << "-before display memory dump before execution\n";
		cout << "-after  display memory dump after execution\n";
		cout << "-wipe   wipe written flags before execution\n"; 
		cout << "-isa    display isa\n";

		return 0;
	}

	// Extract FileName from given arguments
	for(unsigned int i=0;i<strlen(argv[2]);i++){
		if(argv[2][i] == '.')
			break;
		fName += argv[2][i];
	}

	// Extract options from given arguments
	for(unsigned int i=0;i<strlen(argv[1]);i++){
		options += argv[1][i];
	}
	

	// Open object file for reading
	FILE* obj_fptr = NULL;
	string s = fName;
	s += ".o";
	obj_fptr = fopen(s.c_str(),"rb");
	
	if(obj_fptr != NULL){	
		num = fread(m_int_code,sizeof(int),MAXN,obj_fptr);

		for(unsigned int i=0;i<num;i++){
			string curr = convert_d_b2s(m_int_code[i],32);
			m_code[i] = curr;
		}

		if(options == "-isa"){
			cout << "\nOpcode Mnemonic Operand\n";
			
			for(auto it=isa.begin();it!=isa.end();it++){
				mnemonic = (it->second).first;
				operand = (it->second).second;
				cout << it->first << "      " << mnemonic << "       " << operand << "\n"; 
			}
			cout << "        " << "SET       value\n\n";
		}

		if(options == "-wipe"){
			cout << "All flags(PC,SP,A,B) have been reset\n";
			registerA = 0;
			registerB = 0;
			pc = 0;
			sp = 0;
		}

		if(options == "-before"){
			cout << "The memory dump before execution \n\n";

			for(unsigned int i=0;(i+4)<num;i+=4){
				cout << convert_d_b2s(i,32) << " " << m_code[i] << " " << m_code[i+1] << " " << m_code[i+2] << " " << m_code[i+3] << "\n";
			}

			cout << "\n\n";
		}

		execute();	
		
		if(options == "-after"){
			cout << "The memory dump after execution \n\n";

			for(unsigned int i=0;(i+4)<num;i+=4){
				cout << convert_d_b2s(i,32) << " " << m_code[i] << " " << m_code[i+1] << " " << m_code[i+2] << " " << m_code[i+3] << "\n";
			}

			cout << "\n\n";
		}

		printf(" \n %d instructions executed \n",i_cnt);
		
		// Close object file
		fclose(obj_fptr);
	} 
	else{
		cout << "There doesn't exist any file with name: " << s << " \n"; 
	}

		

	return 0;
}

