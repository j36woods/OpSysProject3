#ifndef _READY_QUEUE_H_
#define _READY_QUEUE_H_

#include "process.h"
#include <queue>
#include <vector>
#include <list>
class ReadyQueue{

private:

public:
	virtual void addProcess(Process* new_proc) = 0;
	virtual Process* getNextProcess() = 0;
	virtual bool isEmpty() = 0;
	virtual void updateWaitTimes() = 0;
};

class SRTComparison{
public:
	bool operator()(const Process* lhs, const Process* rhs){
		return lhs->get_remaining_burst_time() > rhs->get_remaining_burst_time();
	}
};

typedef std::priority_queue<Process*, std::vector<Process*>, SRTComparison> SRT_PQueue;

class SRTQueue : ReadyQueue{

private:
	SRT_PQueue readyQueue;

public:
	void addProcess(Process* new_proc);
	Process* getNextProcess();
	bool isEmpty();
	void updateWaitTimes();
};

class RRQueue : ReadyQueue{

private:
	std::list<Process*> readyQueue;

public:
	void addProcess(Process* new_proc);
	Process* getNextProcess();
	bool isEmpty();
	void updateWaitTimes();
};

#endif
