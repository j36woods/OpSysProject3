#include "arrivalcomparison.h"
#include "process.h"
#include "readyqueue.h"
#include "cpu.h"
#include "io.h"
#include "memory.h"
#include <iostream>
#include <queue>

int srt_simulation(std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue,
		Memory* mem, int t_cs_, int n_, int& num_context_switches, int& total_wait_time, int& total_turnaround_time) {
	SRTQueue srt_queue;
	CPU cpu;
	IO io;
	int t = 0;
	int t_cs = t_cs_;
	int n = n_;
	Process* process_cs = NULL;
	int cs_finish_time;
	int time_process_left_mem = 0;
	int last_defrag_time = -1;

	//Continue until there are no processes left
	while (n > 0) {

		//Increment the counts in the cpu, io and queue
		cpu.update_process();
		io.update_processes();
		srt_queue.updateWaitTimes();

		//Check if a process finished using the CPU
		Process* io_bound_process = cpu.remove_finished_process();
		if (io_bound_process) {
			
			//Analysis variables
			total_wait_time += io_bound_process->get_wait_time();
			total_turnaround_time += (t - io_bound_process->get_ready_queue_arrival_time());
			io_bound_process->reset_wait_time();
			io_bound_process->set_ready_queue_arrival_time(0);

			std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' completed its CPU burst ";
			srt_queue.printQueue();

			//Check if the process has completed all of its bursts
			if (io_bound_process->get_current_num_burst() == 0) {
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' finished ";
				srt_queue.printQueue();
				
				n--;

				//Remove process from memory
				mem->removeProcess(io_bound_process);
				mem->printMemory();
				
				time_process_left_mem = t;
				io_bound_process->reset_num_burst();

			} else {
				//Send the process to IO
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' performing I/O ";
				srt_queue.printQueue();
				io.add_process(io_bound_process);
			}
		}

		//Check if any processes completed IO
		std::vector<Process*> finished_processes = io.remove_finished_processes();
		for (unsigned int i = 0; i < finished_processes.size(); i++) {
			finished_processes[i]->set_ready_queue_arrival_time(t);
			srt_queue.addProcess(finished_processes[i]);
			std::cout << "time " << t << "ms: Process '" << finished_processes[i]->get_proc_num() << "' completed I/O ";
			srt_queue.printQueue();
		}

		//Search through the arrival queue to see if any processes can be added to the ready queue
		std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> new_arrival_queue;
		while (!arrival_queue.empty()) {

			//Check if the process has arrived
			if (arrival_queue.top()->get_arrival_time() <= t) {

				unsigned moved = 0;
				unsigned new_time;

				//Try to add the process
				bool success = mem->addProcess(arrival_queue.top(), t, new_time, last_defrag_time, time_process_left_mem, moved);

				if (!success) /*Could not add process*/ {

					//Update the processes in IO if defrag was called
					for (unsigned int i = t; i < new_time; i++) {
						io.update_processes();

						//Check for any finished IO processes
						std::vector<Process*> finished_processes = io.remove_finished_processes();
						for (unsigned int j = 0; j < finished_processes.size(); j++) {
							finished_processes[j]->set_ready_queue_arrival_time(i+1);
							srt_queue.addProcess(finished_processes[j]);
							std::cout << "time " << i+1 << "ms: Process '" << finished_processes[j]->get_proc_num() << "' completed I/O ";
							srt_queue.printQueue();
						}
					}

					//Print the new memory if defrag was successful
					if ((unsigned)last_defrag_time == new_time) {
						std::cout << "time " << new_time << "ms: Completed defragmentation (moved " << moved << " memory units)\n";
						std::cout << "time " << new_time << "ms: ";
						mem->printMemory();
					}

					new_arrival_queue.push(arrival_queue.top());

				} else if ((int)new_time == t) /*Process was added without defrag*/ {
					//Add the process to the ready queue
					arrival_queue.top()->set_ready_queue_arrival_time(t);
					srt_queue.addProcess(arrival_queue.top());
					std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system ";
					srt_queue.printQueue();
					std::cout << "time " << t << "ms: ";
					mem->printMemory();

				} else /*Process was added after defrag*/ {

					//Update the processes in IO
					for (unsigned int i = t; i < new_time; i++) {
						io.update_processes();

						//Check for finished IO processes
						std::vector<Process*> finished_processes = io.remove_finished_processes();
						for (unsigned int j = 0; j < finished_processes.size(); j++) {
							finished_processes[j]->set_ready_queue_arrival_time(i+1);
							srt_queue.addProcess(finished_processes[j]);
							std::cout << "time " << i+1 << "ms: Process '" << finished_processes[j]->get_proc_num() << "' completed I/O ";
							srt_queue.printQueue();
						}
					}

					//Print the new memory if defrag was successful
					if (moved != 0) {
						std::cout << "time " << new_time << "ms: Completed defragmentation (moved " << moved << " memory units)\n";
					}

					std::cout << "time " << new_time << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system";
					srt_queue.printQueue();
					std::cout << "time " << new_time << "ms: ";
					mem->printMemory();
					arrival_queue.top()->set_ready_queue_arrival_time(new_time);
					srt_queue.addProcess(arrival_queue.top());
				}

				t = new_time;

			} else {
				new_arrival_queue.push(arrival_queue.top());
			}

			arrival_queue.pop();
		}

		arrival_queue = new_arrival_queue;

		//Check if process is waiting for context switch
		if (process_cs && cs_finish_time == t) {
			std::cout << "time " << t << "ms: Process '" << process_cs->get_proc_num() << "' started using the CPU ";
			srt_queue.printQueue();
			//Add process to the queue
			cpu.add_process(process_cs);
			process_cs = NULL;
			cs_finish_time = 0;
		}

		if (!srt_queue.isEmpty() && process_cs == NULL) {

			//Grab the first process in the queue
			Process* tmp = srt_queue.getNextProcess();

			//Get a process ready waiting for the context switch
			if (cpu.is_empty()) {
				num_context_switches++;
				cs_finish_time = t + t_cs;
				process_cs = tmp;		

			} else {

				//Check for preemption
				if ((tmp->get_burst_time() - tmp->get_current_burst_time()) < cpu.get_remaining_time()) {
					num_context_switches++;
					cs_finish_time = t + t_cs;
					process_cs = tmp;
					tmp = cpu.preempt_process();
					srt_queue.addProcess(tmp);
					std::cout << "time " << t << "ms: Process '" << tmp->get_proc_num() << "' preempted by Process '" << process_cs->get_proc_num() << "' ";
					srt_queue.printQueue();
				} else {
					srt_queue.addProcess(tmp);
				}
				
			}
			
		}

		t++;

	}

	return t;
}
