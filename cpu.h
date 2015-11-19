#include "process.h"

class CPU {

public:
	CPU();
	bool is_empty();
	void add_process(Process& new_p);
	void update_process();
	int get_remaining_time();
	Process* preempt_process(Process& new_p);
	Process* remove_finished_process();

private:
	Process* p;

};
