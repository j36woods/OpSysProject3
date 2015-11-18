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


void Memory::set_t_memmove(unsigned t){
	this->t_memmove = t;
}

unsigned Memory::defragment(unsigned start_time){
	std::cout << "time " << start_time << "ms: Starting defragmentation (suspending all processes)" << std::endl;
	this->printMemory();
	unsigned curr_time = start_time;
	unsigned left_index, right_index;
	left_index = right_index = 0;
	while(left_index < this->memory_bank.size() && this->memory_bank[left_index] != '.'){
		++left_index;
		++right_index;
	}
	unsigned moved = 0;
	while(true){
		if(right_index >= this->memory_bank.size) return curr_time;
		
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
	std::cout << "time " << curr_time << "ms: Completed defragmentation (moved " << moved << " memory units)" << std::endl;
	this->printMemory();
}

FFMemory::FFMemory(){
	for(unsigned i = 0; i < 256; ++i){
		this->memory_bank.push('.');
	}
}
NFMemory::NFMemory(){
	this->previous_end = 0;
	for(unsigned i = 0; i < 256; ++i){
		this->memory_bank.push('.');
	}
}
BFMemory::BFMemory(){
	for(unsigned i = 0; i < 256; ++i){
		this->memory_bank.push('.');
	}
}

std::vector<Partition_T> Memory::getPartitions(){
	std::vector<Partition_T> partitions;
	for(unsigned index = 0; index < this->memory_bank.size(); ++index){
		if(this->memory_bank[index] == '.'){
			unsigned p_start = index;
			while(index < this->memory_bank.size() && this->memory_bank[index] == '.'){
				++index;
			}
			partitions.push_back(std::make_pair(p_start, index-1);
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

int FFMemory::addProcess(Process* proc, unsigned start_time){
	std::vector<Partition_T> partitions = this->getPartitions();
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			this->placeProcess(proc, iter->first);
			return (int)start_time;
		}
	}
	unsigned new_time = this->defragment(start_time);
	partitions = this->getPartitions();
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			this->placeProcess(proc, iter->first);
			return (int)new_time;
		}
	}
	return -1;
}

int NFMemory::addProcess(Process* proc, unsigned start_time){
	std::vector<Partition_T> partitions = this->getPartitions();
	Partition_T best_placement;
	bool first = true;
	for(std::vector<Partition_T>::iterator iter = partitions.begin(); iter != partitions.end(); ++iter){
		unsigned p_size = iter->second - iter->first + 1;
		if(p_size >= proc->get_memory()){
			if(first){
				best_placement = *iter;
				first = false;
				if(iter->first > previous_end) break;
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
		return (int)start_time;
	}

	unsigned new_time = this->defragment(start_time);
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
		return (int)new_time;
	}
	return -1;
}

int BFMemory::addProcess(Process* proc, unsigned start_time){
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
				if(iter->second - iter->first < best_placement.second-best_placement->first){
					best_placement = *iter;
				}
			}
		}
	}
	if(!first){
		this->placeProcess(proc, best_placement.first);
		return (int)start_time;
	}

	unsigned new_time = this->defragment(start_time);
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
		return (int)new_time;
	}
	return -1;
}
