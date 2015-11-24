#include "process.h"

#ifndef ARRIVALCOMPARISON_H
#define ARRIVALCOMPARISON_H

class ArrivalComparison {
public:
	bool operator()(const Process* lhs, const Process* rhs){
		if (lhs->get_arrival_time() == rhs->get_arrival_time()) {
			return lhs->get_proc_arrival_num() > rhs->get_proc_arrival_num();
		}
		return lhs->get_arrival_time() > rhs->get_arrival_time();
	}
};

#endif