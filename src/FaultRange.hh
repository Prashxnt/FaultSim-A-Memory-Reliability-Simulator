/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
