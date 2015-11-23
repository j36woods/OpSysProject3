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

	std::string line;
	std::priority_queue<Process*, std::vector<Process*>, ArrivalComparison> arrival_queue;
	while (std::getline(in_str, line)) {
		//Ignore empty lines or commented lines
		if (line.empty() || line[0] == '#') {
			continue;
		}

		//Create a process object for each input line and add it to the arrival queue
		std::vector<int> properties = splitString(line);
		Process* process = new Process(line[0], properties[0], properties[1], properties[2], properties[3], properties[4]);
		arrival_queue.push(process);
	}

	//Close the input stream
	in_str.close();

	SRTQueue srt_queue;
	FFMemory ff_mem;
	FFMemory* ff_ptr = &ff_mem;

	int t_cs = 13;

	std::cout << "time 0ms: Simulator started for SRT and First-Fit\n";
	srt_simulation(arrival_queue, ff_ptr, t_cs);



	return 0;
}