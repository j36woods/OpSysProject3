#include "arrivalcomparison.h"
#include "process.h"
#include "readyqueue.h"
#include "cpu.h"
#include "io.h"
#include "memory.h"
#include <iostream>
#include <queue>

int srt_simulation(std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue,
		Memory* mem, int t_cs_, int n_, int& num_context_switches, int& total_wait_time) {
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


	while (n > 0) {

		//!!!!!!!!!!!Print queue at the end of each line!!!!!!!!!!!

		cpu.update_process();
		io.update_processes();
		srt_queue.updateWaitTimes();

		//Check if a process finished using the CPU
		Process* io_bound_process = cpu.remove_finished_process();
		if (io_bound_process) {
			
			//std::cout << "wait time = " << io_bound_process->get_wait_time() << std::endl;
			
			total_wait_time += io_bound_process->get_wait_time();
			io_bound_process->reset_wait_time();

			std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' completed its CPU burst\n";
			//std::cout << "with " << io_bound_process->get_num_burst() << " bursts left\n";
			//Check if the process has completed all of its bursts
			if (io_bound_process->get_current_num_burst() == 0) {
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' finished\n";
				n--;
				mem->removeProcess(io_bound_process);
				mem->printMemory();
				time_process_left_mem = t;
				io_bound_process->reset_num_burst();
				//delete io_bound_process;
			} else {
				//Send the process to IO
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' performing I/O\n";
				io.add_process(io_bound_process);
			}
		}



		//Check if any processes completed IO
		std::vector<Process*> finished_processes = io.remove_finished_processes();
		for (unsigned int i = 0; i < finished_processes.size(); i++) {
			std::cout << "time " << t << "ms: Process '" << finished_processes[i]->get_proc_num() << "' completed I/O\n";
			//Add the process back to the queue
			//Process finished_process = finished_processes[i];
			//Process* ptr = &finished_process;
			srt_queue.addProcess(finished_processes[i]);

			//std::cout << "CPU has remaining time = " << cpu.get_remaining_time() << std::endl; 
		}


		//std::cout << "HERE\n";

		std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> new_arrival_queue;
		while (!arrival_queue.empty()) {

			//!!!!!!!!!!!!!!!!!!!!Update IO, turnaround, and wait times during defrag!!!!!!!!!!!!!!!!

			if (arrival_queue.top()->get_arrival_time() <= t) {

				if (t > 2655 && t < 2665) {
					std::cout << "process " << arrival_queue.top()->get_proc_num();
					std::cout << ", last_defrag_time = " << last_defrag_time;
					std::cout << ", time_process_left_mem = " << time_process_left_mem << std::endl; 
				}

				unsigned new_time;
				unsigned moved;
				bool success = mem->addProcess(arrival_queue.top(), t, new_time, last_defrag_time, time_process_left_mem, moved);

				//std::cout << "new_time = " << new_time << std::endl;

				if (!success) /*Could not add process*/ {
					//std::cout << "time " << new_time << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' still could not be added to system\n";
					
					//last_defrag_time = new_time;


					for (unsigned int i = 0; i < (new_time-t); i++) {
						io.update_processes();
					}

					new_arrival_queue.push(arrival_queue.top());

				} else if ((int)new_time == t) /*Process was added without defrag*/ {
					std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";
					std::cout << "time " << t << "ms: ";
					mem->printMemory();
					srt_queue.addProcess(arrival_queue.top());

				} else /*Process was added after defrag*/ {
					//t = new_time;
					//last_defrag_time = new_time;
					std::cout << "i goes to " << new_time-t << std::endl;
					for (unsigned int i = 0; i < (new_time-t); i++) {
						io.update_processes();
					}

					std::cout << "time " << new_time << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";;
					std::cout << "time " << new_time << "ms: ";
					mem->printMemory();
					srt_queue.addProcess(arrival_queue.top());
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
			//std::cout << "wait time = " << process_cs->get_wait_time() << std::endl;
			//process_cs->reset_wait_time();
			std::cout << "time " << t << "ms: Process '" << process_cs->get_proc_num() << "' started using the CPU\n";
			//std::cout << "current_burst_time = " << process_cs->get_current_burst_time() << std::endl;
			cpu.add_process(process_cs);
			process_cs = NULL;
			cs_finish_time = 0;
		}

		//std::cout << srt_queue.isEmpty() << " & " << (process_cs == NULL) << std::endl;
		if (!srt_queue.isEmpty() && process_cs == NULL) {
		//std::cout << "HERE\n";

			Process* tmp = srt_queue.getNextProcess();
			//std::cout << "Process " << tmp->get_proc_num() << std::endl;

			if (cpu.is_empty()) {
				//std::cout << "CPU is empty\n";
				num_context_switches++;
				cs_finish_time = t + t_cs;
				//std::cout << "cs_finish_time = " << cs_finish_time << std::endl;				
				process_cs = tmp;		
				//std::cout << "process_cs " << process_cs->get_proc_num() << std::endl;		

			} else {
				if ((tmp->get_burst_time() - tmp->get_current_burst_time()) < cpu.get_remaining_time()) {
					num_context_switches++;
					cs_finish_time = t + t_cs;
					process_cs = tmp;
					tmp = cpu.preempt_process();
					std::cout << "time " << t << "ms: Process '" << tmp->get_proc_num() << "' preempted by Process '" << process_cs->get_proc_num() << "'\n";
				}
				//std::cout << "Adding process " << tmp->get_proc_num() << " to the queue\n";
				srt_queue.addProcess(tmp);
			}
			
		}

		t++;

	}

	return t;
}
