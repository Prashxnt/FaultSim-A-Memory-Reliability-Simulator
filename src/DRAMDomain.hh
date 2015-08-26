/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DRAMDOMAIN_HH_
#define DRAMDOMAIN_HH_

#include "dram_common.hh"

#include <list>

#include "FaultDomain.hh"
class FaultRange;

// 32-bit random integers for determining fault locations
typedef boost::mt19937						ENG32;

class DRAMDomain : public FaultDomain
{
	public:
	DRAMDomain( char *name, uint32_t n_bitwidth, uint32_t n_ranks, uint32_t n_banks, uint32_t n_rows, uint32_t n_cols);

	void setFIT( int faultClass, bool isTransient, double FIT );
    void init( uint64_t interval, uint64_t sim_seconds, double fit_factor );
	int update(uint test_mode_t);	// perform one iteration
	void repair( uint64_t &n_undetectable, uint64_t &n_uncorrectable );
	void scrub( void );
	virtual void reset( void );
    
	list<FaultRange*> *getRanges( void );

	void dumpState( void );
	void printStats( void );
	void resetStats( void );
	uint32_t getLogBits(void);
	uint32_t getLogRanks(void);
	uint32_t getLogBanks(void);
	uint32_t getLogRows(void);
	uint32_t getLogCols(void);

	uint32_t getBits(void);
	uint32_t getRanks(void);
	uint32_t getBanks(void);
	uint32_t getRows(void);
	uint32_t getCols(void);
    
    //uint32_t getChipNumber(void);


	void generateRanges( int faultClass, bool transient ); // based on a fault, create all faulty address ranges
	FaultRange *genRandomRange( bool rank, bool bank, bool row, bool col, bool bit, bool transient, int64_t rowbit_num, bool isTSV_t );
	const char *faultClassString( int i );

	double transientFIT[DRAM_MAX];
	double permanentFIT[DRAM_MAX];

	// Parameters for event-driven simulation (hours per fault transient followed by permanent
	double hrs_per_fault[DRAM_MAX*2];

	list<FaultRange*> m_faultRanges;

	ENG  eng;
	DIST dist;
	GEN  gen;
	ENG32 eng32;

	uint64_t curr_interval;

	protected:
	uint64_t n_faults_transient_class[DRAM_MAX];
	uint64_t n_faults_permanent_class[DRAM_MAX];

	uint64_t n_faults_transient_tsv, n_faults_permanent_tsv;

	uint32_t m_bitwidth, m_ranks, m_banks, m_rows, m_cols;
	uint32_t m_logBits, m_logRanks, m_logBanks, m_logRows, m_logCols;
};


#endif /* DRAMDOMAIN_HH_ */
