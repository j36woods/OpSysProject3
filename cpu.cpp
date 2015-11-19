#include "cpu.h"

CPU::CPU() {
	p = NULL;
}

bool CPU::is_empty() {
	if (p == NULL) {
		return true;
	} else {
		return false;
	}
}

void CPU::add_process(Process* new_p) {
	p = new_p;
}

void CPU::update_process() {
	if (p == NULL) {
		return;
	}
	p->increment_current_burst_time();
}

int CPU::get_remaining_time() {
	if (p == NULL) {
		return -1;
	}
	return p->get_burst_time() - p->get_current_burst_time();
}

Process* CPU::preempt_process() {
	Process* old_p = p;
	p = NULL;
	return old_p;
}

Process* CPU::remove_finished_process() {
	if (p == NULL) {
		return NULL;
	}
	if (p->get_burst_time() - p->get_current_burst_time() <= 0) {
		Process* old_p = p;
		old_p->reset_current_burst_time();
		old_p->decrement_num_burst();
		p = NULL;
		return old_p;
	}
	return NULL;
}