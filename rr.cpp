#include "arrivalcomparison.h"
#include <queue>
void rr_simulation(std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue,
		Memory* mem, unsigned t_cs, unsigned t_slice) {
	RRQueue RR_queue;
	CPU cpu;
	IO io;
	unsigned t = 0;
	unsigned n = arrival_queue.size();
	Process* process_cs = NULL;
	unsigned cs_finish_time;

	while (n > 0) {

		//!!!!!!!!!!!Add extra time if defrag was called?!!!!!!!!!!

		//Increment the times for the processes in CPU and IO
		cpu.update_process();
		io.update_processes();



		//Check if a process finished using the CPU
		Process* io_process = cpu.remove_finished_process();
		if (io__process) {

			std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' completed its CPU burst\n";
			//std::cout << "with " << io_bound_process->get_num_burst() << " bursts left\n";
			//Check if the process has completed all of its bursts
			if (io_bound_process->get_num_burst() == 0) {
				std::cout << "time " << t << "ms: Process '" << io_bound_process->get_proc_num() << "' finished\n";
				n--;
				mem->removeProcess();	
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



		//Check if any processes have arrived
		while (!arrival_queue.empty() && arrival_queue.top()->get_arrival_time() <= t) {
			//std::cout << "HERE\n";
			//Try to add the process to memory
			//!!!!!!!!!!!!!Needs work!!!!!!!!!!!!!
			int new_time = mem->addProcess(arrival_queue.top(), t);
			//std::cout << "new_time = " << new_time << std::endl;
			if (new_time == -1) /*Could not add process to memory*/ {
				//Handle this

			} else if (new_time == t) /*Process was added without defrag*/ {
				std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";
				std::cout << "time " << t << "ms: ";
				mem->printMemory();
				srt_queue.addProcess(arrival_queue.top());
				arrival_queue.pop();

			} else /*Process was added after defrag*/ {
				t = new_time;
				std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";;
				srt_queue.addProcess(arrival_queue.top());
				arrival_queue.pop();
			}
		}



		//Check if processes are waiting in the queue
		if (process_cs && cs_finish_time == t) {
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
				cs_finish_time = t + t_cs;
				//std::cout << "cs_finish_time = " << cs_finish_time << std::endl;				
				process_cs = tmp;		
				//std::cout << "process_cs " << process_cs->get_proc_num() << std::endl;		

			} else {
				if ((tmp->get_burst_time() - tmp->get_current_burst_time()) < cpu.get_remaining_time()) {
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
}
