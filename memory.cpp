#include "memory.h"

void Memory::printMemory(){
	std::cout << "Simulated Memory:" << std::endl;
	unsigned line_size = 32;
	for(unsigned i = 0; i < line_size; ++i){
		std::cout << "=";
	}
	std::cout << std::endl;
	for(unsigned j = 0; j < this->memory_bank.size(); j+= line_size){
		for(unsigned i = j; i < j+32; ++i){
			std::cout << memory_bank[i];
		}
		std::cout << std::endl;
	}
	for(unsigned i = 0; i < line_size; ++i){
		std::cout << "=";
	}
	std::cout << std::endl;
}
void Memory::printMemoryWithout(char c){
	std::cout << "Simulated Memory:" << std::endl;
	unsigned line_size = 32;
	for(unsigned i = 0; i < line_size; ++i){
		std::cout << "=";
	}
	std::cout << std::endl;
	for(unsigned j = 0; j < this->memory_bank.size(); j+= line_size){
		for(unsigned i = j; i < j+32; ++i){
			if(memory_bank[i] == c){
				std::cout << '.';
			}else{
				std::cout << memory_bank[i];
			}
		}
		std::cout << std::endl;
	}
	for(unsigned i = 0; i < line_size; ++i){
		std::cout << "=";
	}
	std::cout << std::endl;
}

void Memory::set_t_memmove(unsigned t){
	this->t_memmove = t;
}
bool Memory::removeProcess(Process* proc){
	bool removed = false;
	for(unsigned i = 0; i < this->memory_bank.size(); ++i){
		if(this->memory_bank[i] == proc->get_proc_num()){
			this->memory_bank[i] = '.';
			removed = true;
		}
	}
	return removed;

}
unsigned Memory::defragment(unsigned start_time, unsigned& moved){
	std::cout << "time " << start_time << "ms: Starting defragmentation (suspending all processes)" << std::endl;
	this->printMemory();
	unsigned curr_time = start_time;
	unsigned left_index, right_index;
	left_index = right_index = 0;
	while(left_index < this->memory_bank.size() && this->memory_bank[left_index] != '.'){
		++left_index;
		++right_index;
	}
	moved = 0;
	while(true){
		if(right_index >= this->memory_bank.size()) {
			return curr_time;
		}
		
		if(this->memory_bank[right_index] == '.'){
			++right_index;
			continue;
		}

		if(this->memory_bank[right_index] != '.'){
			this->memory_bank[left_index] = this->memory_bank[right_index];
			++left_index;
			this->memory_bank[right_index] = '.';
			++right_index;
			curr_time += t_memmove;
			++moved;
			continue;
		}

	}
}

Memory::Memory(){
	t_memmove = 10;
	for(unsigned i = 0; i < 256; ++i){
		this->memory_bank.push_back('.');
	}
}

FFMemory::FFMemory(){}

NFMemory::NFMemory(){
	this->previous_end = 0;
}

BFMemory::BFMemory(){}

std::vector<Partition_T> Memory::getPartitions(){
	std::vector<Partition_T> partitions;
	for(unsigned index = 0; index < this->memory_bank.size(); ++index){
		if(this->memory_bank[index] == '.'){
			unsigned p_start = index;
			while(index < this->memory_bank.size() && this->memory_bank[index] == '.'){
				++index;
			}
			partitions.push_back(std::make_pair(p_start, index-1));
		}
	}
	return partitions;
}

void Memory::placeProcess(Process* proc, unsigned start){
	for(unsigned i = start; i < start+proc->get_memory(); ++i){
		if(this->memory_bank[i] != '.'){
			std::cerr << "ERROR: cannot place process at designated location" << std::endl;
		}
		this->memory_bank[i] = proc->get_proc_num();
	}
}

bool FFMemory::addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem, unsigned &moved){
	new_time = start_time;
	std::vector<Partition_T> partitions = this->getPartitions();
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			this->placeProcess(proc, iter->first);
			return true;
		}
	}
	
	if (last_defrag < time_process_left_mem) {
		std::cout << "time " << start_time << "ms: Process '" << proc->get_proc_num() << "' unable to be added; lack of memory\n";
		new_time = this->defragment(start_time,moved);
		last_defrag = new_time;
		partitions = this->getPartitions();
		for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
			unsigned p_size = iter->second - iter->first + 1;
			if(p_size >= proc->get_memory()){
				this->placeProcess(proc, iter->first);
				return true;
			}
		}
	}
	return false;
}

bool NFMemory::addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem, unsigned &moved){
	new_time = start_time;
	std::vector<Partition_T> partitions = this->getPartitions();
	Partition_T best_placement;
	bool first = true;
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			if(first){
				best_placement = *iter;
				first = false;
				if(iter->first >= previous_end) break;
			}else{
				if(iter->first >= previous_end){
					best_placement = *iter;
					break;
				}
			}
		}
	}
	if(!first){
		this->placeProcess(proc, best_placement.first);
		previous_end = best_placement.first+proc->get_memory();
		return true;
	}

	if (last_defrag < time_process_left_mem) {
		std::cout << "time " << start_time << "ms: Process '" << proc->get_proc_num() << "' unable to be added; lack of memory\n";
		moved = 0;
		new_time = this->defragment(start_time, moved);
		last_defrag = new_time;
		//retry
		previous_end = 0;
		partitions = this->getPartitions();
		first = true;
		for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
			unsigned p_size = iter->second - iter->first + 1;
			if(p_size >= proc->get_memory()){
				if(first){
					best_placement = *iter;
					first = false;
					if(iter->first>previous_end) break;
				}else{
					if(iter->first > previous_end){
						best_placement = *iter;
						break;
					}
				}
			}
		}
		if(!first){
			this->placeProcess(proc, best_placement.first);
			previous_end = best_placement.first+proc->get_memory();
			return true;
		}
	}
	return false;
}

bool BFMemory::addProcess(Process* proc, unsigned start_time, unsigned& new_time, int& last_defrag, int time_process_left_mem, unsigned &moved){
	new_time = start_time;
	std::vector<Partition_T> partitions = this->getPartitions();
	Partition_T best_placement;
	bool first = true;
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			if(first){
				best_placement = *iter;
				first = false;
			}else{
				if(iter->second - iter->first < best_placement.second-best_placement.first){
					best_placement = *iter;
				}
			}
		}
	}
	if(!first){
		this->placeProcess(proc, best_placement.first);
		return true;
	}
	if (last_defrag < time_process_left_mem) {
		std::cout << "time " << start_time << "ms: Process '" << proc->get_proc_num() << "' unable to be added; lack of memory\n";
		new_time = this->defragment(start_time, moved);
		last_defrag = new_time;
		//retry
		partitions = this->getPartitions();
		first = true;
		for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
			unsigned p_size = iter->second - iter->first + 1;
			if(p_size >= proc->get_memory()){
				if(first){
					best_placement = *iter;
					first = false;
				}else{
					if(iter->second-iter->first < best_placement.second-best_placement.first){
						best_placement = *iter;
					}
				}
			}
		}
		if(!first){
			this->placeProcess(proc, best_placement.first);
			return true;
		}
	}
	return false;
}
