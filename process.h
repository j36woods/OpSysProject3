#include <string>

#ifndef PROCESS_H
#define PROCESS_H

typedef enum State {
	IN_RQ, 
	USING_CPU,
	USING_IO,
	NOT_IN_SYSTEM,
	DONE,
	WAITING_FOR_CS,
	UNDEFINED
} State;

class Process {

public:
	Process(char proc_num_, int arrival_time_, int burst_time_, int num_burst_, int io_time_, int memory_, int proc_arrival_num_);
	
	State get_state() const;
	char get_proc_num() const;
	int get_arrival_time() const;
	int get_burst_time() const;
	int get_current_burst_time() const;
	int get_num_burst() const;
	int get_current_num_burst() const;
	int get_io_time() const;
	int get_current_io_time() const;
	unsigned int get_memory() const;
	int get_memory_start_index() const;
	int get_turnaround_time() const;
	int get_wait_time() const;
	int get_remaining_burst_time() const;
	int get_proc_arrival_num() const;
	int get_ready_queue_arrival_time() const;


	void reset_current_burst_time();
	void reset_num_burst();
	void increment_current_burst_time();
	void decrement_num_burst();
	void reset_current_io_time();
	void increment_current_io_time();
	void set_memory_start_index(const int new_index);
	void increment_turnaround_time();
	void reset_turnaround_time();
	void increment_wait_time();
	void reset_wait_time();
	void set_ready_queue_arrival_time(int ready_queue_arrival_time_);

private:
	State state;
	char proc_num;
	int arrival_time;
	int burst_time;
	int current_burst_time;
	int num_burst;
	int current_num_burst;
	int io_time;
	int current_io_time;
	int memory;
	int memory_start_index;
	int turnaround_time;
	int wait_time;
	int proc_arrival_num;
	int ready_queue_arrival_time;
};

#endif