#include "io.h"
#include <vector>
#include <list>

IO::IO() {

}

void IO::add_process(Process* p) {
	io_processes.push_back(p);
}

//Increment every process's current IO time
void IO::update_processes() {
	std::list<Process*>::iterator itr = io_processes.begin();
	while (itr != io_processes.end()) {
		(*itr)->increment_current_io_time();
		itr++;
	}
}

//Remove and return the processes that are done with IO
std::vector<Process*> IO::remove_finished_processes() {
	std::vector<Process*> finished_processes;
	std::list<Process*>::iterator itr = io_processes.begin();
	while (itr != io_processes.end()) {
		Process* p = *itr;
		if (p->get_io_time() - p->get_current_io_time() <= 0) {
			itr = io_processes.erase(itr);
			p->reset_current_io_time();
			finished_processes.push_back(p);
		} else {
			++itr;
		}
	}
	return finished_processes;
}
