#include "process.h"

#ifndef ARRIVALCOMPARISON_H
#define ARRIVALCOMPARISON_H

class ArrivalComparison {
public:
	bool operator()(const Process* lhs, const Process* rhs){
		return lhs->get_arrival_time() > rhs->get_arrival_time();
	}
};

#endif