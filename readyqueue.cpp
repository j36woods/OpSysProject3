#include "readyqueue.h"

void SRTQueue::addProcess(Process* new_proc){
	this->readyQueue.push(new_proc);
}

Process* SRTQueue::getNextProcess(){
	if(!(this->readyQueue.empty())){
		Process* temp = this->readyQueue.top();
		this->readyQueue.pop();
		return temp;
	}
	return NULL;
}

bool SRTQueue::isEmpty(){
	return this->readyQueue.empty();
}

void RRQueue::addPRocess(Process* new_proc){
	this->readyQueue.push_back(new_proc);
}

Process* RRQueue::getNextProcess(){
	if(!(this->readyQueue.empty())){
		return this->readyQueue.pop_front();
	}
	return NULL;
bool RRQueue::isEmpty(){
	return this->readyQueue.empty();
}
