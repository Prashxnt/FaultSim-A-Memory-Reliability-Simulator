/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include <string>

#include "DRAMDomain.hh"
#include "FaultRange.hh"
#include "dram_common.hh"

FaultRange::FaultRange( DRAMDomain *pDRAM ) :
m_pDRAM(pDRAM)
{
	transient = 0;
	fAddr = fWildMask = 0;
	touched = 0;
	fault_mode=0;
	transient_remove=true;
	recent_touched=false;
	max_faults=0;
	TSV = false;
	
	// Event-driven FaultSim
	timestamp = 0;
	ignore = 0;
}

// Old implementation

bool FaultRange::intersectsSlow( FaultRange *fr )
{
	uint64_t fAddr0 = fAddr;
	uint64_t fMask0 = fWildMask;
	uint64_t fAddr1 = fr->fAddr;
	uint64_t fMask1 = fr->fWildMask;
	for( uint32_t pos = 0; pos < 64; pos++ )
	{
		if( !((fMask0 & 0x1) || (fMask1 & 0x1) || ((fAddr0 & 0x1) == (fAddr1 & 0x1))) ) {
			// no intersection if one of the bits isn't wild or the bits don't match
			return false;
		}

		fAddr0 >>= 1;
		fMask0 >>= 1;
		fAddr1 >>= 1;
		fMask1 >>= 1;

	}

	// DR DEBUG
	//fr->touched++;

	// intersection if there is no conflict on any address bit

	return true;
}

// Fast implementation

bool FaultRange::intersects( FaultRange *fr )
{
	uint64_t fAddr0 = fAddr;
	uint64_t fMask0 = fWildMask;
	uint64_t fAddr1 = fr->fAddr;
	uint64_t fMask1 = fr->fWildMask;

	uint64_t combined_mask = fMask0 | fMask1;
	uint64_t equal_addr = ~( fAddr0 ^ fAddr1 );
	uint64_t finalterm = ~( combined_mask | equal_addr );

	bool result = (finalterm == 0) ? true : false;

	// DEBUG
	//assert( result == intersectsSlow( fr ) );

	// DR DEBUG
	/*
	if (result==true)
	{
		fr->touched++;
	}
	*/

	return result;
}

string FaultRange::toString( void )
{
	char buf[100];

	uint64_t fA = fAddr;
	uint64_t fM = fWildMask;

	// decode address
	uint fbit = fA & ((0x1 << m_pDRAM->getLogBits())-1);
	fA >>= m_pDRAM->getLogBits();
	uint fcol = fA & ((0x1 << m_pDRAM->getLogCols())-1);
	fA >>= m_pDRAM->getLogCols();
	uint frow = fA & ((0x1 << m_pDRAM->getLogRows())-1);
	fA >>= m_pDRAM->getLogRows();
	uint fbank = fA & ((0x1 << m_pDRAM->getLogBanks())-1);
	fA >>= m_pDRAM->getLogBanks();
	uint frank = fA & ((0x1 << m_pDRAM->getLogRanks())-1);
	fA >>= m_pDRAM->getLogRanks();

	// decode mask
	uint mbit = fM & ((0x1 << m_pDRAM->getLogBits())-1);
	fM >>= m_pDRAM->getLogBits();
	uint mcol = fM & ((0x1 << m_pDRAM->getLogCols())-1);
	fM >>= m_pDRAM->getLogCols();
	uint mrow = fM & ((0x1 << m_pDRAM->getLogRows())-1);
	fM >>= m_pDRAM->getLogRows();
	uint mbank = fM & ((0x1 << m_pDRAM->getLogBanks())-1);
	fM >>= m_pDRAM->getLogBanks();
	uint mrank = fM & ((0x1 << m_pDRAM->getLogRanks())-1);
	fM >>= m_pDRAM->getLogRanks();

	//sprintf( buf, "trans %d fAddr 0x%lX fMask 0x%lX", transient, fAddr, fWildMask );

	sprintf( buf, "TSV %d trans %d fAddr (%d,%d,%d,%d,%d) fMask 0x(%X,%X,%X,%X,%X)", TSV, transient,
			 frank, fbank, frow, fcol, fbit,
			 mrank, mbank, mrow, mcol, mbit );

	return string(buf);
}

void FaultRange::clear( void )
{
}

/*
uint64_t FaultRange::maxFaultyBits( uint64_t sym_bits )
{
	// in this range, how many bits could be faulty in any sym_bits-sized symbol?

	// for each of the least significant sym_bits bits,
	// count the number of
}
 */
