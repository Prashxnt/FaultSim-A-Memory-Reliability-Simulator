/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "DRAMDomain.hh"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include "faultsim.hh"
#include "Settings.hh"

extern struct Settings settings;

DRAMDomain::DRAMDomain( char *name, uint32_t n_bitwidth, uint32_t n_ranks, uint32_t n_banks, uint32_t n_rows, uint32_t n_cols ) : FaultDomain( name )
, dist(0,1)
, gen(eng,dist)
, m_bitwidth( n_bitwidth )
, m_ranks( n_ranks )
, m_banks( n_banks )
, m_rows( n_rows )
, m_cols( n_cols )
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	gen.engine().seed(tv.tv_sec * 1000000 + (tv.tv_usec));

	//gettimeofday (&tv, NULL);
	eng32.seed(tv.tv_sec * 1000000 + (tv.tv_usec));

	for( int i = 0; i < DRAM_MAX; i++ ) {
		n_faults_transient_class[i] = 0;
		n_faults_permanent_class[i] = 0;

		transientFIT[i] = 0;
		permanentFIT[i] = 0;
	}

	n_faults_transient_tsv = n_faults_permanent_tsv = 0;

	m_logRanks = log2( m_ranks );
	m_logBanks = log2( m_banks );
	m_logRows = log2( m_rows );
	m_logCols = log2( m_cols );
	m_logBits = log2( m_bitwidth );

	curr_interval = 0;

	if( settings.verbose )
	{
		double gbits = ((double)(m_ranks*m_banks*m_rows*m_cols*m_bitwidth))/((double)1024*1024*1024);

		cout << "# -------------------------------------------------------------------\n";
		cout << "# DRAMDomain(" << m_name << ")\n";
		cout << "# ranks " << m_ranks << "\n";
		cout << "# banks " << m_banks << "\n";
		cout << "# rows " << m_rows << "\n";
		cout << "# cols " << m_cols << "\n";
		cout << "# bitwidth " << m_bitwidth << "\n";
		cout << "# gbits " << gbits << "\n";
		cout << "# -------------------------------------------------------------------\n";
	}
}

list<FaultRange*> *DRAMDomain::getRanges( void )
{
	return &m_faultRanges;
}

const char *DRAMDomain::faultClassString( int i )
{
	switch( i ) {
	case DRAM_1BIT:
		return "1BIT";
		break;

	case DRAM_1WORD:
		return "1WORD";
		break;

	case DRAM_1COL:
		return "1COL";
		break;

	case DRAM_1ROW:
		return "1ROW";
		break;

	case DRAM_1BANK:
		return "1BANK";
		break;

	case DRAM_NBANK:
		return "NBANK";
		break;

	case DRAM_NRANK:
		return "NRANK";
		break;

	default:
		assert(0);
	}

	return "";
}

int DRAMDomain::update( uint test_mode_t )
{
	int newfault0 = 0;
	int newfault1 = 0;
	newfault0 = FaultDomain::update(test_mode_t); 

	// Insert DRAM die faults
	for( uint i = 0; i < DRAM_MAX; i++ ) {
		if(test_mode_t==0)
		{
			double random = gen();
			if( random <= transientFIT[i] ) {
				n_faults_transient++;
				n_faults_transient_class[i]++;
				generateRanges( i, true );
				newfault1 = 1;			
			}

			random = gen();

			if( random <= permanentFIT[i] ) {
				n_faults_permanent++;
				n_faults_permanent_class[i]++;
				generateRanges( i, false );
				newfault1 = 1;
			}

		}
		else
		{
			if( i == (test_mode_t-1)) {
				n_faults_transient++;
				n_faults_transient_class[i]++;
				generateRanges( i, true );
				newfault1 = 1;
			}

			if( i == (test_mode_t-1) ) {
				n_faults_permanent++;
				n_faults_permanent_class[i]++;
				generateRanges( i, false );
				newfault1 = 1;
			}
		}
	}

	// Insert TSV faults
	if((cube_model_enable>0) && enable_tsv)
	{
		for(uint ii=(children_counter*cube_data_tsv); ii<((children_counter+1)*cube_data_tsv); ii++ )
		{
			if(tsv_bitmap[ii]==true)
			{
				if(tsv_info[ii]==1)
				{
					n_faults_permanent++;
					n_faults_permanent_tsv++;
					newfault1 = 1;

					for(uint jj=0; jj<(m_cols*m_bitwidth/cube_data_tsv); jj++ )
					{
						m_faultRanges.push_back( genRandomRange( 0, 0, 0, 1, 1, false, (ii%cube_data_tsv)+(jj*cube_data_tsv), true ) );
						//cout << "|" <<(ii%cube_data_tsv)+(jj*cube_data_tsv)<< "|";
					}
					tsv_info[ii]=3;
				}
				else if (tsv_info[ii]==2)
				{
					n_faults_transient++;
					n_faults_transient_tsv++;
					newfault1 = 1;

					for(uint jj=0; jj<(m_cols*m_bitwidth/cube_data_tsv); jj++ )
					{
						m_faultRanges.push_back( genRandomRange( 0, 0, 0, 1, 1, true, (ii%cube_data_tsv)+(jj*cube_data_tsv), true ) );
						//cout << "|" <<(ii%cube_data_tsv)+(jj*cube_data_tsv)<< "|";
					} 
					tsv_info[ii]=4;
				}
			}
		}
	}

	curr_interval++;
	
	return (newfault0 || newfault1);
}

#define min(a,b) (a<b) ? a : b

void DRAMDomain::repair( uint64_t &n_undetectable, uint64_t &n_uncorrectable )
{
	// override the remaining number of uncorrectable faults seen based on repair results
	FaultDomain::repair( n_undetectable, n_uncorrectable );
}

bool first_time = 1;

void DRAMDomain::reset( void )
{
	FaultDomain::reset();

	// delete all faults
	list<FaultRange*>::iterator it;

	for( it = m_faultRanges.begin(); it != m_faultRanges.end(); it++ )
	{
		delete (*it);
	}

	m_faultRanges.clear();

	// DR DEBUG - insert known faults
	/*
	if( settings.debug && (first_time == 1) )
	{
		first_time = 0;
		list<FaultRange*> *ranges = getRanges();

		// DR DEBUG: inject entire single bit column fault across all banks
		FaultRange *range = new FaultRange( this );
		range->fAddr = 1;
		range->fWildMask = 0x3FFFC001;
		range->max_faults = 9999;
		ranges->push_back( range );

		range = new FaultRange( this );
		range->fAddr = 1;
		range->fWildMask = 0x3FFFC000;
		range->max_faults = 9999;
		ranges->push_back( range );
	}
	*/
}

void DRAMDomain::dumpState( void )
{
	if( m_faultRanges.size() != 0 )
	{
		cout << m_name << " ";

		list<FaultRange*>::iterator it;

		for( it = m_faultRanges.begin(); it != m_faultRanges.end(); it++ )
		{
			cout << (*it)->toString() << "\n";
		}
	}
}

void DRAMDomain::scrub( void )
{
	FaultDomain::scrub();

	// delete all transient faults
	list<FaultRange*>::iterator it;
	for( it = m_faultRanges.begin(); it != m_faultRanges.end(); it++ )
	{
		if( (*it)->transient ) {
			if((*it)->transient_remove)
			{
				it = m_faultRanges.erase( it );
			}
		}
	}
}

void DRAMDomain::setFIT( int faultClass, bool isTransient, double FIT )
{
	if( isTransient ) {
		transientFIT[faultClass] = FIT;
	} else {
		permanentFIT[faultClass] = FIT;
	}
}

void DRAMDomain::init( uint64_t interval, uint64_t sim_seconds, double fit_factor )
{
	FaultDomain::init( interval, sim_seconds, fit_factor );
	// interval in seconds
	// one-time initialization scales FIT rates to interval scale

	// For Event Driven sim ////////////////////////////////////////////
	for( int i = 0; i < DRAM_MAX; i++ ) {
		hrs_per_fault[i] = ((double)1000000000.0) / (transientFIT[i] * fit_factor);
	}
	for( int i = DRAM_MAX; i < DRAM_MAX*2; i++ ) {
		hrs_per_fault[i] = ((double)1000000000.0) / (permanentFIT[i-DRAM_MAX] * fit_factor);
	}
	////////////////////////////////////////////////////////////////////

	// 1 FIT = 10^9 device-hours
	double sec_per_hour = 60 * 60;
	double interval_factor = (interval / sec_per_hour) / 1000000000.0;
	/*	for( int i = 0; i < DRAM_MAX; i++ ) {
		transientFIT[i] = transientFIT[i] * fit_factor * interval_factor;
		permanentFIT[i] = permanentFIT[i] * fit_factor * interval_factor;
	}	*/
	// Comparisons against these FIT values are done using uniform random
	// numbers.  To convert from FIT rate to probability assuming an exponential fault
	// distribution, F(t) = 1 - e^(- lambda t)
	// http://en.wikipedia.org/wiki/Failure_rate

	for( int i = 0; i < DRAM_MAX; i++ ) {
		transientFIT[i] = (double)1.0 - exp( -transientFIT[i] * fit_factor * interval_factor );
		permanentFIT[i] = (double)1.0 - exp( -permanentFIT[i] * fit_factor * interval_factor );
		assert( transientFIT[i] >= 0 );
		assert( transientFIT[i] <= 1 );
		assert( permanentFIT[i] >= 0 );
		assert( permanentFIT[i] <= 1 );
	}
}

void DRAMDomain::generateRanges( int faultClass, bool transient )
{
	switch( faultClass ) {
	case DRAM_1BIT:
		m_faultRanges.push_back( genRandomRange( 1, 1, 1, 1, 1,transient, -1, false) );
		break;

	case DRAM_1WORD:
		m_faultRanges.push_back( genRandomRange( 1, 1, 1, 1, 0,transient, -1, false) );
		break;

	case DRAM_1COL:
		m_faultRanges.push_back( genRandomRange( 1, 1, 0, 1, 0,transient, -1, false) );
		break;

	case DRAM_1ROW:
		m_faultRanges.push_back( genRandomRange( 1, 1, 1, 0, 0,transient, -1, false) );
		break;

	case DRAM_1BANK:
		m_faultRanges.push_back( genRandomRange( 1, 1, 0, 0, 0,transient, -1, false) );
		break;

	case DRAM_NBANK:
		m_faultRanges.push_back( genRandomRange( 1, 0, 0, 0, 0,transient, -1, false) );
		break;

	case DRAM_NRANK:
		m_faultRanges.push_back( genRandomRange( 0, 0, 0, 0, 0,transient, -1, false) );
		break;

	default:
		assert(0);
	}

}

FaultRange *DRAMDomain::genRandomRange( bool rank, bool bank, bool row, bool col, bool bit, bool transient, int64_t rowbit_num, bool isTSV_t )
{
	FaultRange *fr = new FaultRange( this );
	fr->fAddr = 0;
	fr->fWildMask = 0;
	fr->Chip=0;
    fr->transient = transient;
	fr->TSV = isTSV_t;
	fr->max_faults = 1;	// maximum number of bits covered by FaultRange

	// parameter 1 = fixed, 0 = wild
	if( rank ) {
		fr->fAddr |= (uint64_t)(eng32()%m_ranks);
	} else {
		fr->fWildMask |= (uint64_t)(m_ranks-1);
		fr->max_faults *= m_ranks;
	}

	fr->fAddr <<= m_logBanks;
	fr->fWildMask <<= m_logBanks;

	if( bank ) {
		fr->fAddr |= (uint64_t)(eng32()%m_banks);
	} else {
		fr->fWildMask |= (uint64_t)(m_banks-1);
		fr->max_faults *= m_banks;
	}

	fr->fAddr <<= m_logRows;
	fr->fWildMask <<= m_logRows;

	if( row ) {
		fr->fAddr |= (uint64_t)(eng32()%m_rows);
	} else {
		fr->fWildMask |= (uint64_t)(m_rows-1);
		fr->max_faults *= m_rows;
	}

	// We're not specifying a specific single bit in a row (TSV fault)
	// so generate column and bit values as normal
	if( rowbit_num==-1 )
	{ 
		fr->fAddr <<= m_logCols;
		fr->fWildMask <<= m_logCols;

		if(col)
		{
			fr->fAddr |= (uint64_t)(eng32()%m_cols);
		} else {
			fr->fWildMask |= (uint64_t)(m_cols-1);
			fr->max_faults *= m_cols;
		}

		fr->fAddr <<= m_logBits;
		fr->fWildMask <<= m_logBits;

		if( bit ) {
			fr->fAddr |= (uint64_t)(eng32()%m_bitwidth);
		} else {
			fr->fWildMask |= (uint64_t)(m_bitwidth-1);
			fr->max_faults *= m_bitwidth;
		}
	}
	else
	{
		// For TSV faults we're specifying a single bit position in the row
		// so we treat the column and bit fields as a single field
		fr->fAddr <<= (m_logCols + m_logBits);
		fr->fWildMask <<= (m_logCols + m_logBits);

		fr->fAddr |= (uint64_t)(rowbit_num);
	}


	return fr;
}

uint32_t DRAMDomain::getLogBits(void)
{
	return m_logBits;
}

uint32_t DRAMDomain::getLogRanks(void)
{
	return m_logRanks;
}

uint32_t DRAMDomain::getLogBanks(void)
{
	return m_logBanks;
}

uint32_t DRAMDomain::getLogCols(void)
{
	return m_logCols;
}
uint32_t DRAMDomain::getLogRows(void)
{
	return m_logRows;
}


uint32_t DRAMDomain::getBits(void)
{
	return m_bitwidth;
}

uint32_t DRAMDomain::getRanks(void)
{
	return m_ranks;
}

uint32_t DRAMDomain::getRows(void)
{
	return m_rows;
}

uint32_t DRAMDomain::getCols(void)
{
	return m_cols;
}

uint32_t DRAMDomain::getBanks(void)
{
	return m_banks;
}

void DRAMDomain::printStats( void )
{
	FaultDomain::printStats();

	cout << " Transient: ";

	for( int i = 0; i < DRAM_MAX; i++ ) {
		cout << n_faults_transient_class[i] << " ";
	}

	cout << "TSV " << n_faults_transient_tsv;

	cout << " Permanent: ";
	for( int i = 0; i < DRAM_MAX; i++ ) {
		cout << n_faults_permanent_class[i] << " ";
	}

	cout << "TSV " << n_faults_permanent_tsv;

	cout << "\n";

	// For extra verbose mode, output list of all fault ranges
	if( settings.verbose == 2 ) {
		list<FaultRange*>::iterator it;
		for( it = m_faultRanges.begin(); it != m_faultRanges.end(); it++ )
		{
			cout << "FR " << (*it)->toString() << "\n";
		}
	}
}

void DRAMDomain::resetStats( void )
{
	FaultDomain::resetStats();
}
