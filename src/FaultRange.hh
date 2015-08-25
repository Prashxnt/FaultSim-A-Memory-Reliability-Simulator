/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef FAULTRANGE_HH_
#define FAULTRANGE_HH_

#include "boost/cstdint.hpp"
#include <list>

class DRAMDomain;

using namespace std;

class FaultRange
{
public:
	FaultRange( DRAMDomain *pDRAM );
	// does this FR intersect with the supplied FR?
	bool intersects( FaultRange *fr );
	// How many bits in any sym_bits-wide symbol could be faulty?
	//uint64_t maxFaultyBits( uint64_t sym_bits );
	string toString( void );	// for debugging
	void clear( void );
	bool isTSV( void );	// is this a TSV fault?
	bool transient;
	bool TSV;
	uint64_t fAddr, fWildMask; // address of faulty range, and bit positions that are wildcards (all values)
	uint64_t touched;
	uint64_t fault_mode;
	bool transient_remove;
	bool recent_touched;
	uint64_t max_faults;
    	uint32_t Chip;  // Chip location of this fault range.
	
	// For Event-driven simulation
	double timestamp;	// time in seconds at which teh FR was inserted
	int ignore;		// whether or not a correctable fault was scrubbed

	DRAMDomain *m_pDRAM;

private:
	bool intersectsSlow( FaultRange *fr );
	list<FaultRange> m_children;	// 'smaller' ranges contained within this one
};


#endif /* FAULTRANGE_HH_ */
