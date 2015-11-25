#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include "process.h"
#include "cpu.h"
#include "io.h"
#include "readyqueue.h"
#include "memory.h"
#include "srt.cpp"
#include "arrivalcomparison.h"

//Split the string wherever there is a '|' character
std::vector<int> splitString(const std::string& s) {
	std::vector<int> result;
	std::string token = "";
	for (unsigned int i = 2; i < s.size(); i++) {
		if (s[i] == '|') {
			int num = atoi(token.c_str());
			result.push_back(num);
			token = "";
		} else {
			token += s[i];
		}
	}
	result.push_back(atoi(token.c_str()));
	return result;
}

int main(int argc, char* argv[]) {

	//Get the file name
	std::string file = "processes.txt";
	if (argc == 2) {
		file = argv[1];
	}

	//Open the input stream
	std::ifstream in_str;
	in_str.open(file.c_str());

	//Check if the input stream is valid
	if(!in_str.good()) {
		std::cerr << "Can't open " << file << " to read.\n";
		exit(1);
	}

	//Parse input file
	std::string line;
	std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue;
	int n = 0;
	float total_cpu_time = 0;
	float total_cpu_bursts = 0;
	while (std::getline(in_str, line)) {
		//Ignore empty lines or commented lines
		if (line.empty() || line[0] == '#') {
			continue;
		}

		//Create a process object for each input line and add it to the arrival queue
		std::vector<int> properties = splitString(line);
		Process* process = new Process(line[0], properties[0], properties[1], properties[2], properties[3], properties[4], n);
		
		total_cpu_time += (process->get_burst_time() * process->get_num_burst());
		total_cpu_bursts += process->get_num_burst();

		arrival_queue.push(process);
		n++;
	}

	float average_cpu_time = total_cpu_time/total_cpu_bursts;

	//Close the input stream
	in_str.close();

	//Open an output stream
	std::ofstream of_str;
	of_str.open("simout.txt");

	SRTQueue srt_queue;
	FFMemory ff_mem;
	NFMemory nf_mem;
	BFMemory bf_mem;

	int t_cs = 13;
	int end_t;
	int total_wait_time = 0;
	int total_turnaround_time = 0;
	int num_context_switches = 0;

	std::cout << "time 0ms: Simulator started for SRT and First-Fit\n";
	end_t = srt_simulation(arrival_queue, &ff_mem, t_cs, n, num_context_switches, total_wait_time, total_turnaround_time);
	std::cout << "time " << end_t-1 << "ms: Simulator for SRT and First-Fit ended\n";
	
	of_str << "Algorithm SRT and First-Fit\n";
	of_str << "average CPU burst time: " << average_cpu_time << " ms\n";
	of_str << "average wait time: " << (float)total_wait_time / total_cpu_bursts << " ms\n";
	of_str << "average turnaround time: " << (float)total_turnaround_time / total_cpu_bursts << " ms\n";
	of_str << "total number of context switches: " << num_context_switches << std::endl;


	//Reset variables
	total_wait_time = 0;
	total_turnaround_time = 0;
	num_context_switches = 0;

	std::cout << "\ntime 0ms: Simulator started for SRT and Next-Fit\n";
	end_t = srt_simulation(arrival_queue, &nf_mem, t_cs, n, num_context_switches, total_wait_time, total_turnaround_time);
	std::cout << "time " << end_t-1 << "ms: Simulator for SRT and Next-Fit ended\n";
	
	of_str << "\nAlgorithm SRT and Next-Fit\n";
	of_str << "average CPU burst time: " << average_cpu_time << " ms\n";
	of_str << "average wait time: " << (float)total_wait_time / total_cpu_bursts << " ms\n";
	of_str << "average turnaround time: " << (float)total_turnaround_time / total_cpu_bursts << " ms\n";
	of_str << "total number of context switches: " << num_context_switches << std::endl;



	total_wait_time = 0;
	total_turnaround_time = 0;
	num_context_switches = 0;

	std::cout << "\ntime 0ms: Simulator started for SRT and Best-Fit\n";
	end_t = srt_simulation(arrival_queue, &bf_mem, t_cs, n, num_context_switches, total_wait_time, total_turnaround_time);
	std::cout << "time " << end_t-1 << "ms: Simulator for SRT and Best-Fit ended\n";

	of_str << "\nAlgorithm SRT and Best-Fit\n";
	of_str << "average CPU burst time: " << average_cpu_time << " ms\n";
	of_str << "average wait time: " << (float)total_wait_time / total_cpu_bursts << " ms\n";
	of_str << "average turnaround time: " << (float)total_turnaround_time / total_cpu_bursts << " ms\n";
	of_str << "total number of context switches: " << num_context_switches << std::endl;



	//!!!!!!!!!!!!!!!!Free up heap memory at the end!!!!!!!!!!!!!!

	return 0;
}