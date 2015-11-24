#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "process.h"
#include <vector>
#include <iostream>

typedef std::pair<unsigned, unsigned> Partition_T;

class Memory {
protected:
	std::vector<char> memory_bank;
	unsigned t_memmove;
	std::vector<Partition_T> getPartitions();
	void placeProcess(Process* proc, unsigned start);
	Memory();
public:
	void printMemory();
	unsigned defragment(unsigned start_time);
	void set_t_memmove(unsigned t);
	//if defrag is triggered, returns new time, otherwise returns start_time if unable to add, returns -1
	virtual bool addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem) = 0;
	bool removeProcess(Process* proc);
};

class FFMemory : public Memory {
public:
	FFMemory();
	bool addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem);
};

class NFMemory : public Memory{
	unsigned previous_end;
public:
	NFMemory();
	bool addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem);
};
class BFMemory : public Memory{
public:
	BFMemory();
	bool addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem);
};

#endif
