/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef CHIPKILLREPAIR_CUBE_HH_
#define CHIPKILLREPAIR_CUBE_HH_

#include "RepairScheme.hh"

class ChipKillRepair_cube : public RepairScheme
{
public:
	ChipKillRepair_cube( string name, int n_sym_correct, int n_sym_detect , FaultDomain *fd);

	void repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable );
	uint64_t fill_repl ( FaultDomain *fd );
	void printStats( void );
	void resetStats( void );
	void clear_counters( void );
	void repair_hc(FaultDomain *fd, uint64_t &n_undetect, uint64_t &n_uncorrect);
	void repair_vc(FaultDomain *fd, uint64_t &n_undetect, uint64_t &n_uncorrect);
	int64_t getbank_number(FaultRange fr_number);
private:
	uint64_t m_n_correct, m_n_detect;
	uint64_t counter_prev, counter_now;
	uint32_t logBits, logCols, logRows, banks;
};


#endif /* CHIPKILLREPAIR_CUBE_HH_ */
