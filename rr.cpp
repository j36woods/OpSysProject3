#include "arrivalcomparison.h"
#include <queue>
#include "memory.h"
#include "process.h"
#include "io.h"
#include "cpu.h"
#include "readyqueue.h"
#include <iostream>

int rr_simulation(std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue,
		Memory* mem, unsigned t_cs, unsigned t_slice, int& num_context_switches, int& total_wait_time, int& total_turnaround_time) {
	RRQueue rr_queue;
	CPU cpu;
	IO io;
	int t = 0;
	unsigned n = arrival_queue.size();
	Process* process_cs = NULL;
	int cs_finish_time;
	unsigned curr_slice_t = 0;
	int time_process_left_mem = 0;
	int last_defrag_time = -1;

	
	while (n > 0) {
		//Increment the times for the processes in CPU and IO
		cpu.update_process();
		io.update_processes();

		rr_queue.updateWaitTimes();
		

		//Check if a process finished using the CPU
		Process* io_bound_process = cpu.remove_finished_process();
		if (io_bound_process) {
			total_wait_time += io_bound_process->get_wait_time();
			total_turnaround_time += (t - io_bound_process->get_ready_queue_arrival_time());
			io_bound_process->reset_wait_time();
			io_bound_process->set_ready_queue_arrival_time(0);

			std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' completed its CPU burst\n";
			if (io_bound_process->get_current_num_burst() == 0) {
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' finished\n";
				n--;
				mem->removeProcess(io_bound_process);	
				mem->printMemory();
				time_process_left_mem = t;
				io_bound_process->reset_num_burst();
			} else {
				//Send the process to IO
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' performing I/O\n";
				io.add_process(io_bound_process);
			}
			curr_slice_t = 0;
		}

		//check if slice is over
		if(curr_slice_t >= t_slice){
			Process *next = rr_queue.getNextProcess();
			if(next){
				Process* preempted = cpu.preempt_process();
				std::cout << "time " << t << "ms: Process '" << preempted->get_proc_num() << "' preempted due to time slice expiration\n";
				rr_queue.addProcess(preempted);
				process_cs = next;
				cs_finish_time = t + t_cs;
			}
			curr_slice_t = 0;
		}

		//Check if any processes completed IO
		std::vector<Process*> finished_processes = io.remove_finished_processes();
		for (unsigned int i = 0; i < finished_processes.size(); i++) {
			std::cout << "time " << t << "ms: Process '" << finished_processes[i]->get_proc_num() << "' completed I/O\n";
			rr_queue.addProcess(finished_processes[i]);
			finished_processes[i]->set_ready_queue_arrival_time(t);
		}


		std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> new_arrival_queue;
		while (!arrival_queue.empty()) {

			//!!!!!!!!!!!!!!!!!!!!Update IO, turnaround, and wait times during defrag!!!!!!!!!!!!!!!!

			if (arrival_queue.top()->get_arrival_time() <= t) {

				//if (t > 2655 && t < 2665) {
					//std::cout << "process " << arrival_queue.top()->get_proc_num();
					//std::cout << ", last_defrag_time = " << last_defrag_time;
					//std::cout << ", time_process_left_mem = " << time_process_left_mem << std::endl; 
				//}

				unsigned moved = 0;
				unsigned new_time;
				bool success = mem->addProcess(arrival_queue.top(), t, new_time, last_defrag_time, time_process_left_mem, moved);

				//std::cout << "new_time = " << new_time << std::endl;

				if (!success) /*Could not add process*/ {
					//std::cout << "time " << new_time << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' still could not be added to system\n";
					
					//last_defrag_time = new_time;

					//std::cout << "THERE\n";


					for (unsigned int i = t; i < new_time; i++) {
						io.update_processes();


						std::vector<Process*> finished_processes = io.remove_finished_processes();
						for (unsigned int j = 0; j < finished_processes.size(); j++) {
							std::cout << "time " << i+1 << "ms: Process '" << finished_processes[j]->get_proc_num() << "' completed I/O\n";
							finished_processes[j]->set_ready_queue_arrival_time(i+1);
							rr_queue.addProcess(finished_processes[j]);

						}
					}

					if ((unsigned)last_defrag_time == new_time) {
						std::cout << "time " << new_time << "ms: Completed defragmentation (moved " << moved << " memory units)\n";
						std::cout << "time " << new_time << "ms: ";
						mem->printMemory();
					}

					new_arrival_queue.push(arrival_queue.top());

				} else if ((int)new_time == t) /*Process was added without defrag*/ {
					std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";
					std::cout << "time " << t << "ms: ";
					mem->printMemory();
					arrival_queue.top()->set_ready_queue_arrival_time(t);
					rr_queue.addProcess(arrival_queue.top());

				} else /*Process was added after defrag*/ {

					//std::cout << "HERE\n";
					//t = new_time;
					//last_defrag_time = new_time;
					for (unsigned int i = t; i < new_time; i++) {
						io.update_processes();

						std::vector<Process*> finished_processes = io.remove_finished_processes();
						for (unsigned int j = 0; j < finished_processes.size(); j++) {
							std::cout << "time " << i+1 << "ms: Process '" << finished_processes[j]->get_proc_num() << "' completed I/O\n";
							finished_processes[j]->set_ready_queue_arrival_time(i+1);
							rr_queue.addProcess(finished_processes[j]);

						}
					}

					if (moved != 0) {
						std::cout << "time " << new_time << "ms: Completed defragmentation (moved " << moved << " memory units)\n";
					}

					std::cout << "time " << new_time << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";;
					std::cout << "time " << new_time << "ms: ";
					mem->printMemory();
					arrival_queue.top()->set_ready_queue_arrival_time(new_time);
					rr_queue.addProcess(arrival_queue.top());
				}

				//last_defrag_time = new_time;
				t = new_time;

			} else {
				new_arrival_queue.push(arrival_queue.top());
			}

			arrival_queue.pop();
		}
		arrival_queue = new_arrival_queue;




		//Check if processes are waiting in the queue
		if (process_cs && cs_finish_time == t) {
			std::cout << "time " << t << "ms: Process '" << process_cs->get_proc_num() << "' started using the CPU\n";
			//std::cout << "current_burst_time = " << process_cs->get_current_burst_time() << std::endl;
			cpu.add_process(process_cs);
			process_cs = NULL;
			cs_finish_time = 0;
			curr_slice_t = 0;
			num_context_switches++;
		}

		if (!rr_queue.isEmpty() && process_cs == NULL && cpu.is_empty()) {

			Process* tmp = rr_queue.getNextProcess();
			if (cpu.is_empty()) {
				cs_finish_time = t + t_cs;
				process_cs = tmp;		
			}
			
		}

		t++;
		curr_slice_t++;

	}
	return t;
}
