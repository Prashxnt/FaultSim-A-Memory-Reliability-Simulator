/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef CUBERAIDREPAIR_HH_
#define CUBERAIDREPAIR_HH_

#include "RepairScheme.hh"

class CubeRAIDRepair : public RepairScheme
{
public:
	CubeRAIDRepair( string name, uint n_sym_correct, uint n_sym_detect, uint detect_block_bytes );

	void repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable );
	uint64_t fill_repl ( FaultDomain *fd );
	void printStats( void );
	void resetStats( void );
	void clear_counters( void );

private:
	uint m_n_correct, m_n_detect, m_data_block_bits, m_log_block_bits;
	uint64_t counter_prev, counter_now;
};


#endif /* CUBERAIDREPAIR_HH_ */
