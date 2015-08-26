/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FaultDomain.hh"
#include "RepairScheme.hh"
#include <iostream>
#include <list>
#include <vector>

FaultDomain::FaultDomain( const char *name_t)
{
	debug = 0;

	resetStats();
	m_name.append( name_t );

	// RAW faults in this domain before detection/correction
	n_faults_transient = n_faults_permanent = 0;
	// Errors after detection/correction
	n_errors_undetected = n_errors_uncorrected = 0;
	tsv_transientFIT = 0;
        tsv_permanentFIT = 0;
	cube_model_enable=0;
	cube_addr_dec_depth=0;
	children_counter=0;
}

string FaultDomain::getName( void )
{
	return m_name;
}

list<FaultDomain*> *FaultDomain::getChildren( void )
{
	return &m_children;
}

void FaultDomain::setDebug( bool dbg )
{
	debug = dbg;
}

void FaultDomain::reset( void )
{
	// reset per-simulation statistics used internally
	n_faults_transient = n_faults_permanent = 0;
	n_errors_undetected = n_errors_uncorrected = 0;	// used to indicate whether the domain failed during a single simulation
	stat_n_simulations++;

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->reset();
	}
}

void FaultDomain::dumpState( void )
{
	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->dumpState();
	}
}
uint64_t FaultDomain::getFaultCountTrans( void )
{
	uint64_t sum = 0;

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		sum += (*it)->getFaultCountTrans();
	}

	sum += n_faults_transient;

	return sum;
}

uint64_t FaultDomain::getFaultCountPerm( void )
{
	uint64_t sum = 0;

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		sum += (*it)->getFaultCountPerm();
	}

	sum += n_faults_permanent;

	return sum;
}

uint64_t FaultDomain::getFaultCountUncorrected( void )
{
	// don't include children in uncorrected failure count
	// because if those faults get corrected here, they're invisible

	return n_errors_uncorrected;
}

uint64_t FaultDomain::getFaultCountUndetected( void )
{
	// don't include children in uncorrected failure count
	// because if those faults get corrected here, they're invisible

	return n_errors_undetected;
}

void FaultDomain::addDomain( FaultDomain *domain, uint32_t domaincounter)
{
	// DR ADDED
	// DR HACK - propagate 3D mode settings from parent to all children as they are added
	domain->cube_model_enable = cube_model_enable;
	domain->tsv_bitmap = tsv_bitmap;
	domain->cube_data_tsv = cube_data_tsv;
	domain->tsv_info = tsv_info;
	domain->children_counter=domaincounter;
	domain->enable_tsv=enable_tsv;
	m_children.push_back( domain );
}

void FaultDomain::init( uint64_t interval, uint64_t sim_seconds, double fit_factor )
{
	m_interval = interval;
	m_sim_seconds = sim_seconds;
	m_fit_factor = fit_factor;

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->init( interval, sim_seconds, fit_factor );
	}
}

int FaultDomain::update(uint test_mode_t)
{
	int return_val=0;
	int temp=0;
	list<FaultDomain*>::iterator it;
	
	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		temp=((*it)->update(test_mode_t));
		
		if(temp==1){
			return_val=1;
		}
	}
	return return_val;
}

void FaultDomain::addRepair( RepairScheme *repair )
{
	m_repairSchemes.push_back( repair );
}

#define min(a,b) (a<b) ? a : b

void FaultDomain::repair( uint64_t &n_undetectable, uint64_t &n_uncorrectable )
{
	n_uncorrectable = 0;
	n_undetectable = 0;

	// repair all children

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		uint64_t child_undet, child_uncorr;
		(*it)->repair( child_undet, child_uncorr );
		n_uncorrectable += child_uncorr;
		n_undetectable += child_undet;
	}

	// repair myself
	list<RepairScheme*>::iterator itr;

	// default to the number of faults in myself and all children, in case there are no repair schemes
	uint64_t faults_before_repair = getFaultCountPerm() + getFaultCountTrans();
	n_undetectable = n_uncorrectable = faults_before_repair;

	for( itr = m_repairSchemes.begin(); itr != m_repairSchemes.end(); itr++ ) {
		uint64_t uncorrectable_after_repair = 0;
		uint64_t undetectable_after_repair = 0;
		(*itr)->repair( this, undetectable_after_repair, uncorrectable_after_repair );
		n_uncorrectable = min( n_uncorrectable, uncorrectable_after_repair );
		n_undetectable = min( n_undetectable, undetectable_after_repair );

		// if any repair happened, dump
		if( debug ) {
			if( faults_before_repair != 0 ) {
				cout << ">>> REPAIR " << m_name << " USING " << (*itr)->getName() << " (state dump)\n";
				dumpState();
				cout << "FAULTS_BEFORE: " << faults_before_repair << " FAULTS_AFTER: " << n_uncorrectable << "\n";
				cout << "<<< END\n";
			}
		}
	}

	if( n_undetectable > 0 ) {
		n_errors_undetected++;
	}

	if( n_uncorrectable > 0 ) {
		n_errors_uncorrected++;
	}

	//return n_uncorrectable;
}
uint64_t FaultDomain::fill_repl( void )
{
	uint64_t n_uncorrectable = 0;

	// fill_repl for all children

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		n_uncorrectable += (*it)->fill_repl();
	}

	// fill_repl myself
	list<RepairScheme*>::iterator itr;

	// default to the number of errors in myself and all children, in case there are no repair schemes
	n_uncorrectable = getFaultCountPerm() + getFaultCountTrans();

	for( itr = m_repairSchemes.begin(); itr != m_repairSchemes.end(); itr++ ) {
		uint64_t uncorrectable_after_repair = (*itr)->fill_repl( this );
		n_uncorrectable = min( n_uncorrectable, uncorrectable_after_repair );
	}

	if( n_uncorrectable ) {
		n_errors_uncorrected++;
	}

	return n_uncorrectable;
}
void FaultDomain::setFIT_TSV(bool isTransient_TSV, double FIT_TSV )
{
	if( isTransient_TSV ) {
		tsv_transientFIT = FIT_TSV;
	} else {
		tsv_permanentFIT = FIT_TSV;
	}
}
void FaultDomain::scrub( void )
{
	// repair all children

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
			(*it)->scrub();
	}
}

uint64_t FaultDomain::getFailedSimCount( void )
{
	return stat_n_failures;
}

void FaultDomain::finalize( void )
{
	// walk through all children and observe their error counts
	// If 1 or more children had a fault, record a failed simulation
	// at this level of the hierarchy.

	list<FaultDomain*>::iterator it;
	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->finalize();
	}

	// RAW error rates
	bool failure = false;
	if( getFaultCountPerm() + getFaultCountTrans() != 0 ) {
		failure = true;
	}

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		if( (*it)->getFaultCountPerm() + (*it)->getFaultCountTrans() != 0 )
		{
			failure = true;
			break;
		}
	}

	if( failure )
	{
		stat_n_failures++;
	}

	// Determine per-simulation statistics
	if( getFaultCountUndetected() != 0 ) {
		stat_n_failures_undetected++;
	}

	if( getFaultCountUncorrected() != 0 ) {
		stat_n_failures_uncorrected++;
	}
	// clear repair counters
	list<RepairScheme*>::iterator itr;
	for( itr = m_repairSchemes.begin(); itr != m_repairSchemes.end(); itr++ ){
	(*itr)->clear_counters();
	}
	
}

void FaultDomain::printStats( void )
{
	list<FaultDomain*>::iterator it;
	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->printStats();
	}

	double device_fail_rate = ((double)stat_n_failures)/((double)stat_n_simulations);
	double FIT_raw = device_fail_rate * ((double)60*60*1000000000) / ((double)m_sim_seconds);

	double uncorrected_fail_rate = ((double)stat_n_failures_uncorrected)/((double)stat_n_simulations);
	double FIT_uncorr = uncorrected_fail_rate * ((double)60*60*1000000000) / ((double)m_sim_seconds);

	double undetected_fail_rate = ((double)stat_n_failures_undetected)/((double)stat_n_simulations);
	double FIT_undet = undetected_fail_rate * ((double)60*60*1000000000) / ((double)m_sim_seconds);

	cout << "[" << m_name << "] sims " << stat_n_simulations << " failed_sims " << stat_n_failures
	     << " rate_raw " << device_fail_rate << " FIT_raw " << FIT_raw
	     << " rate_uncorr " << uncorrected_fail_rate << " FIT_uncorr " << FIT_uncorr
	     << " rate_undet " << undetected_fail_rate << " FIT_undet " << FIT_undet << "\n";
}

void FaultDomain::resetStats( void )
{
	stat_n_simulations = stat_n_failures = 0;
	stat_n_failures_undetected = stat_n_failures_uncorrected = 0;

	list<FaultDomain*>::iterator it;

	for( it = m_children.begin(); it != m_children.end(); it++ ) {
		(*it)->resetStats();
	}

	list<RepairScheme*>::iterator it2;

	for( it2 = m_repairSchemes.begin(); it2 != m_repairSchemes.end(); it2++ ) {
		(*it2)->resetStats();
	}
}
