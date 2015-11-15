#include <vector>
#include <list>
#include "process.h"

class IO {

public:
	IO();
	void add_process(const Process& p);
	void update_processes();
	std::vector<Process> remove_finished_processes();

private:
	std::list<Process> io_processes;

};