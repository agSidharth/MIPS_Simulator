#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;
#include "basic.hpp"

int decode_a(int memory_instruction,int R[],int instruction,int op,pair<int,Node*> busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],int priority[],int reg_used_when_blocked[],int* wait_buffer_size,int prev_dram_ins,int behind_cycle[])
{
	int r3 = ((1<<5)-1) & (memory_instruction>>11);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(r1==0) throw invalid_argument("An attempt to change the value stored in $zero ");
	
	if((busy[r1].first==1 && busy[r2].first==0 && busy[r3].first==0) && busy[r1].second->data!=prev_dram_ins)
	{
		busy[r1].first = 0;

		Node* temp = busy[r1].second;

		cout<<"core :"<<temp->core<<" line number "<<temp->line<<": Deleted from wait buffer"<<endl;

		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;
	}
	else if(busy[r1].first==1 && (busy[r2].first==1 || busy[r3].first==1) && (r1!=r2) && (r1!=r3) && (busy[r1].second->data!=prev_dram_ins))
	{
		busy[r1].first = 0;

		Node* temp1 = busy[r1].second;

		cout<<"core :"<<temp1->core<<" line number "<<temp1->line<<": Deleted from wait buffer"<<endl;

		temp1->prev->next = temp1->next;
		temp1->next->prev = temp1->prev;

		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;

		blocked[core] = true;

		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r3].first == 1) {int a = (busy[r3].second->saved_address/1024);reg_used_when_blocked[2] = r3;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}

		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else if(busy[r1].first==1 || busy[r3].first==1 || busy[r2].first==1) 			//if either of them is busy dont move forward
	{
		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r3].first == 1) {int a = (busy[r3].second->saved_address/1024);reg_used_when_blocked[2] = r3;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}

		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else
	blocked[core] = false;
	
	if(behind_cycle[core]==1)
	{behind_cycle[core]=0;}
	else priority[core] = 100;

	if(op==1) R[r1] = R[r2] + R[r3];
	else if(op==2) R[r1] = R[r2] - R[r3];
	else if(op==3) R[r1] = R[r2]*R[r3];
	else if(op==4 && R[r2]<R[r3]) R[r1] = 1;
	else R[r1] = 0;

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;

	return instruction+1;
}

int decode_b(int memory_instruction,int R[],int instruction,int op,pair<int,Node*> busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],int priority[],int reg_used_when_blocked[],int* wait_buffer_size,int prev_dram_ins,int behind_cycle[])
{
	int address = ((1<<15)-1) & (memory_instruction);		//address is stored in 15 bits now.
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(r1==0) throw invalid_argument("An attempt to change the value stored in $zero ");
	
	if((busy[r1].first==1 && busy[r2].first==0) && busy[r1].second->data!=prev_dram_ins)
	{
		busy[r1].first = 0;

		Node* temp = busy[r1].second;

		cout<<"core :"<<temp->core<<" line number "<<temp->line<<": Deleted from wait buffer"<<endl;
		
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;
	}
	else if(busy[r1].first==1 && busy[r2].first==1 && (r1!=r2) && (busy[r1].second->data!=prev_dram_ins))
	{
		busy[r1].first = 0;

		Node* temp1 = busy[r1].second;

		cout<<"core :"<<temp1->core<<" line number "<<temp1->line<<": Deleted from wait buffer"<<endl;
		
		temp1->prev->next = temp1->next;
		temp1->next->prev = temp1->prev;
		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;	

		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}

		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else if(busy[r1].first ==1 ||busy[r2].first==1) 							//if either of them is busy dont move forward
	{
		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}

		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else blocked[core] = false;

	if(behind_cycle[core]==1)
	{behind_cycle[core]=0;}
	else priority[core] = 100;

	int sign = (memory_instruction & (1<<15));					//for dealing with negative sign
	if(sign!=0) R[r1] = R[r2] - address;
	else R[r1] = R[r2] + address;

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;
	return instruction+1;
}

int decode_c(int memory_instruction,int end_of_instruction,int instruction,int cycle,int core,int ref_ins,bool blocked[],int priority[],int reg_used_when_blocked[])
{
	int new_instruction = ((1<<26)-1) & (memory_instruction);
	if(new_instruction>end_of_instruction) throw invalid_argument("Unexpected output in jump statement");

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<new_instruction - ref_ins<<endl;
	return new_instruction;		//correct this.
}

void take_data(int buffer[],int R[],int r1,int location,int remainder)
{
	if(remainder==0) {R[r1] = buffer[location];return ;}

	if(location==255) throw invalid_argument("cannot access memory from two rows at same time");
	switch(remainder)
	{
		case 1: R[r1] = (buffer[location] <<8) + ((buffer[location+1]>>24)&((1<<8) - 1));break;
		case 2: R[r1] = (buffer[location] <<16)+ ((buffer[location+1]>>16)&((1<<16) - 1));break;
		case 3: R[r1] = (buffer[location] <<24)+ ((buffer[location+1]>>8)&((1<<24) - 1));break;
	}
	if(r1==0 && R[r1]!=0) throw invalid_argument("An attempt to change the value stored in $zero ");
	return ;
}

void enter_data(int buffer[],int location,int remainder,int value)
{
	if(remainder==0) {buffer[location] = value; return;}
	
	if(location==255) throw invalid_argument("cannot access memory from two rows at same time");

	int value1 = buffer[location];
	int value2 = buffer[location+1];
	switch(remainder)
	{
		case 1: buffer[location] = ((value1>>24)<<24) + value>>8;
			buffer[location+1] = ((value & ((1<<8)-1))<<24) + (value2 & ((1<<24)-1));
			break;		
		case 2:
			buffer[location] = ((value1>>16)<<16) + value>>16;
			buffer[location+1] = ((value & ((1<<16)-1))<<16) + (value2 & ((1<<16)-1));
			break;
		case 3: 
			buffer[location] = ((value1>>8)<<8) + value>>24;
			buffer[location+1] = ((value & ((1<<24)-1))<<8) + (value2 & ((1<<8)-1));
			break;
	}
	return ;
}

int decode_d(int memory_instruction,int R[],int instruction,int op,int core,int end_of_instruction,pair<int,Node*> busy[],int R_used[],int buffer[],bool blocked[],int start_address,int priority[],int reg_used_when_blocked[],int* wait_buffer_size,int prev_dram_ins,int behind_cycle[])
{
	int offset = ((1<<15)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if((busy[r1].first==1 && busy[r2].first==0) && busy[r1].second->data!=prev_dram_ins)
	{
		busy[r1].first = 0;

		Node* temp = busy[r1].second;

		cout<<"core :"<<temp->core<<" line number "<<temp->line<<": Deleted from wait buffer"<<endl;
		
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;
	}
	else if(busy[r1].first==1 && busy[r2].first==1 && (r1!=r2) && (busy[r1].second->data!=prev_dram_ins))
	{
		busy[r1].first = 0;

		Node* temp1 = busy[r1].second;

		cout<<"core :"<<temp1->core<<" line number "<<temp1->line<<": Deleted from wait buffer"<<endl;
		
		temp1->prev->next = temp1->next;
		temp1->next->prev = temp1->prev;
		*wait_buffer_size = (*wait_buffer_size) -1;
		busy[r1].second = nullptr;
		
		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		
		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else if(busy[r1].first==1 || busy[r2].first==1) 
	{
		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		
		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else
	blocked[core] = false;
	
	if(behind_cycle[core]==1)
	{behind_cycle[core]=0;}
	else priority[core] = 100;

	//busy[r2] = 1;												//design  decision.
	if(op==8){busy[r1].first = 1;}						//only r1 is locked for lw and permanently 

	//else busy[r1] = busy[r1]+2;							//value of r2 can be accessed but not changed
	R_used[instruction] = r1;
	
	int sign = (memory_instruction & (1<<15));

	int address;
	if(sign!=0) address = R[r2] - 4*offset;
	else address = R[r2] + 4*offset;

	address = address + start_address;
	
	if(address/4>=((1<<18)) || ((address - start_address)/4<=end_of_instruction) || address<0) 
	{throw invalid_argument("Unexpected inputwsdank "+to_string(instruction)+" because of either access of encoded instruction data in memory or address not in memory size");}
		
	if(op==8) take_data(buffer,R,r1,(address/4)%256,address%4)	;		        
	else if(op==9) enter_data(buffer,(address/4)%256,address%4,R[r1]) ;			
	
	return instruction+1;
}

int decode_e(int memory_instruction,int R[],int instruction,int op,pair<int,Node*> busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],int priority[],int reg_used_when_blocked[],int behind_cycle[])
{
	int next_instruction = ((1<<16)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r1].first==1 || busy[r2].first==1) 
	{
		blocked[core] = true;
		priority[core] = 0;
		reg_used_when_blocked[0] = -1;
		reg_used_when_blocked[1] = -1;
		reg_used_when_blocked[2] = -1;
		set<int> temp;
		if(busy[r1].first == 1) {int a = (busy[r1].second->saved_address/1024);reg_used_when_blocked[0] = r1;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		if(busy[r2].first == 1) {int a = (busy[r2].second->saved_address/1024);reg_used_when_blocked[1] = r2;if(temp.count(a) == 0) {priority[core]++;temp.insert(a);}}
		
		if(behind_cycle[core]==0)
		{
			behind_cycle[core] = 1;
			priority[core] = 100;
		}
		return instruction;
	}
	else
	blocked[core] = false;
	
	if(behind_cycle[core]==1)
	{behind_cycle[core]=0;}
	else priority[core] = 100;
	
	if(op==6 && R[r1]==R[r2]) 
	{
		cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<next_instruction - ref_ins<<endl;
		return next_instruction;
	}
	else if(op==5 && R[r1]!=R[r2]) 
	{
		cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<next_instruction - ref_ins<<endl;
		return next_instruction;
	}
	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<instruction+1 - ref_ins<<endl;
	return instruction+1;
}