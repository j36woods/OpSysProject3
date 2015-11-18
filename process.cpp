#include "process.h"
#include <string>

Process::Process(char proc_num_, int arrival_time_, int burst_time_, int num_burst_, int io_time_, int memory_) {
	state = NOT_IN_SYSTEM;
	proc_num = proc_num_;
	arrival_time = arrival_time_;
	burst_time = burst_time_;
	current_burst_time = 0;
	num_burst = num_burst_;
	io_time = io_time_;
	current_io_time = 0;
	memory = memory_;
	memory_start_index = -1;
	total_turnaround_time = 0;
	total_wait_time = 0;
}


State Process::get_state() const {
	return state;
}

char Process::get_proc_num() const {
	return proc_num;
}

int Process::get_arrival_time() const {
	return arrival_time;
}

int Process::get_burst_time() const {
	return burst_time;
}

int Process::get_current_burst_time() const {
	return current_burst_time;
}

int Process::get_remaining_burst_time() const{
	return this->get_burst_time() - this->get_current_burst_time();
}

int Process::get_num_burst() const {
	return num_burst;
}

int Process::get_io_time() const {
	return io_time;
}

int Process::get_current_io_time() const {
	return current_io_time;
}

int Process::get_memory() const {
	return memory;
}

int Process::get_memory_start_index() const {
	return memory_start_index;
}

int Process::get_total_turnaround_time() const {
	return total_turnaround_time;
}

int Process::get_total_wait_time() const {
	return total_wait_time;
}


void Process::reset_current_burst_time() {
	current_burst_time = 0;
}

void Process::increment_current_burst_time() {
	current_burst_time++;
}

void Process::decrement_num_burst() {
	num_burst--;
}

void Process::reset_current_io_time() {
	current_io_time = 0;
}

void Process::increment_current_io_time() {
	current_io_time++;
}

void Process::set_memory_start_index(const int new_index) {
	memory_start_index = new_index;
}

void Process::increment_total_turnaround_time() {
	total_turnaround_time++;
}

void Process::increment_total_wait_time() {
	total_wait_time++;
}
