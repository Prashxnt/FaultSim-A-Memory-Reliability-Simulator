/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef REPAIRSCHEME_HH_
#define REPAIRSCHEME_HH_

#include <list>
#include <string>
#include "FaultDomain.hh"

class RepairScheme
{
public:
	RepairScheme( string name );
	string getName( void );

	virtual void repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable ) = 0;
	virtual uint64_t fill_repl (FaultDomain *fd);
	virtual void clear_counters (void)=0;

	void printStats( void );
	void resetStats( void );

protected:
	string m_name;
};


#endif /* REPAIRSCHEME_HH_ */
