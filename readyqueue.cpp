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

void SRTQueue::updateWaitTimes() {
	std::vector<Process*> tmp_storage;
	while (!this->readyQueue.empty()) {
		Process* tmp = this->readyQueue.top();
		tmp->increment_wait_time();
		tmp_storage.push_back(tmp);
		this->readyQueue.pop();
	}
	for (unsigned int i = 0; i < tmp_storage.size(); i++) {
		this->readyQueue.push(tmp_storage[i]);
	}
}

void RRQueue::addProcess(Process* new_proc){
	this->readyQueue.push_back(new_proc);
}

Process* RRQueue::getNextProcess(){
	if(!(this->readyQueue.empty())){
		Process* tmp = this->readyQueue.front();
		this->readyQueue.pop_front();
		return tmp;
	}
	return NULL;
}
bool RRQueue::isEmpty(){
	return this->readyQueue.empty();
}

void RRQueue::updateWaitTimes() {
	std::list<Process*>::iterator itr = this->readyQueue.begin();
	while (itr != this->readyQueue.end()) {
		(*itr)->increment_wait_time();
	}
}