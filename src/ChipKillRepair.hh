/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef CHIPKILLREPAIR_HH_
#define CHIPKILLREPAIR_HH_

#include "RepairScheme.hh"

class ChipKillRepair : public RepairScheme
{
public:
	ChipKillRepair( string name, int n_sym_correct, int n_sym_detect );

	void repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable );
	uint64_t fill_repl ( FaultDomain *fd );
	void printStats( void );
	void resetStats( void );
	void clear_counters( void );

private:
	uint64_t m_n_correct, m_n_detect;
	uint64_t counter_prev, counter_now;
};


#endif /* CHIPKILLREPAIR_HH_ */
