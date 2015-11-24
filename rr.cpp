#include "arrivalcomparison.h"
#include <queue>
#include "memory.h"
#include "process.h"
#include "io.h"
#include "readyqueue.h"
#include <iostream>

void rr_simulation(std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue,
		Memory* mem, unsigned t_cs, unsigned t_slice) {
	RRQueue RR_queue;
	CPU cpu;
	IO io;
	unsigned t = 0;
	unsigned n = arrival_queue.size();
	Process* process_cs = NULL;
	unsigned cs_finish_time;
	unsigned curr_slice_t = 0;

	
	while (n > 0) {

		//Increment the times for the processes in CPU and IO
		cpu.update_process();
		io.update_processes();



		//Check if a process finished using the CPU
		Process* io_process = cpu.remove_finished_process();
		if (io_process) {

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
			curr_slice_t = 0;
		}


		//check if slice is over
		if(curr_slice_t >= t_slice){
			Process *next = RR_queue.getNextProcess();
			if(next){
				Process* preempted = cpu.preempt_process();
				std::cout << "time " << t << "ms: Process '" << preeempted->get_proc_num() << "' preempted due to time slice expiration\n"
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
		}



		//Check if any processes have arrived
		while (!arrival_queue.empty() && arrival_queue.top()->get_arrival_time() <= t) {
			
			int new_time = mem->addProcess(arrival_queue.top(), t);
			
			if (new_time == -1) /*Could not add process to memory*/ {
				//Handle this

			} else if (new_time == t) /*Process was added without defrag*/ {
				std::cout << "time " << t << "ms: Process '" << arrival_queue.top()->get_proc_num() << "' added to system\n";
				std::cout << "time " << t << "ms: ";
				mem->printMemory();
				rr_queue.addProcess(arrival_queue.top());
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

		if (!rr_queue.isEmpty() && process_cs == NULL && cpu.is_empty()) {

			Process* tmp = rr_queue.getNextProcess();
			if (cpu.is_empty()) {
				cs_finish_time = t + t_cs;
				process_cs = tmp;		
			}
			
		}

		t++;

	}
}
