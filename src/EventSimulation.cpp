/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "boost/cstdint.hpp"
#include "EventSimulation.hh"
#include "FaultDomain.hh"
#include "DRAMDomain.hh"
#include "FaultRange.hh"
#include <list>
#include <iostream>
#include <fstream>
#include <queue>
#include <iomanip>
#include <stdio.h>
#include <math.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
using namespace std;

class CompareFR {
public:
	bool operator()(FaultRange*& f1, FaultRange*& f2)
	{
		if( f1->timestamp < f2->timestamp ) return true;
		return false;
	}
};


EventSimulation::EventSimulation( uint64_t interval_t, uint64_t scrub_interval_t, double fit_factor_t , uint test_mode_t,
									bool debug_mode_t, bool cont_running_t, uint64_t output_bucket_t)
: Simulation( interval_t, scrub_interval_t, fit_factor_t, test_mode_t, debug_mode_t, cont_running_t, output_bucket_t)
{
}

// Event-driven simulation takes over the task of injecting errors into the chips
// from the DRAMDomains. It also advances time in variable increments according to event times

uint64_t EventSimulation::runOne( uint64_t max_s, int verbose, uint64_t bin_length)
{
	// returns number of uncorrectable simulations
	priority_queue<FaultRange*, vector<FaultRange*>, CompareFR> q1;

	// reset the domain states e.g. recorded errors for the simulated timeframe
	reset();
	uint64_t bin;

	// New for Event-Driven: set up the time-ordered event list
	// Get access to a DRAM domain
	list<FaultDomain*> *pChips = m_domains.front()->getChildren();

	int err_inserted = 0;

	int devices = 0;
	for( list<FaultDomain*>::iterator it1 = pChips->begin(); it1 != pChips->end(); it1++ )
	{
		DRAMDomain* pD = (DRAMDomain*)(*it1);
		double period=0;
		for(int errtype=0; errtype<DRAM_MAX*2; errtype++)
		{
			double currtime=0;
			while(currtime <= ((double)max_s)){
				period = -1*log(pD->gen())*pD->hrs_per_fault[errtype] * (60 * 60); //Exponential interval in SECONDS
				currtime += period;
				if(currtime <= max_s){
					double timestamp = currtime;
					FaultRange *fr = NULL;
					if(errtype==0)
					{
						fr = pD->genRandomRange( 1, 1, 1, 1, 1, 1, -1, 0);
					}
					else if(errtype==1)
					{
						fr = pD->genRandomRange( 1, 1, 1, 1, 0, 1, -1, 0);
					}
					else if(errtype==2)
					{
						fr = pD->genRandomRange( 1, 1, 0, 1, 0, 1, -1, 0);
					}
					else if(errtype==3)
					{
						fr = pD->genRandomRange( 1, 1, 1, 0, 0, 1, -1, 0);
					}
					else if(errtype==4)
					{
						fr = pD->genRandomRange( 1, 1, 0, 0, 0, 1, -1, 0);
					}
					else if(errtype==5)
					{
						fr = pD->genRandomRange( 1, 0, 0, 0, 0, 1, -1, 0);
					}
					else if(errtype==6)
					{
						fr = pD->genRandomRange( 0, 0, 0, 0, 0, 1, -1, 0);
					}
					else if(errtype==7)
					{
						fr = pD->genRandomRange( 1, 1, 1, 1, 1, 0, -1, 0);
					}
					else if(errtype==8)
					{
						fr = pD->genRandomRange( 1, 1, 1, 1, 0, 0, -1, 0);
					}
					else if(errtype==9)
					{
						fr = pD->genRandomRange( 1, 1, 0, 1, 0, 0, -1, 0);
					}
					else if(errtype==10)
					{
						fr = pD->genRandomRange( 1, 1, 1, 0, 0, 0, -1, 0);
					}
					else if(errtype==11)
					{
						fr = pD->genRandomRange( 1, 1, 0, 0, 0, 0, -1, 0);
					}
					else if(errtype==12)
					{
						fr = pD->genRandomRange( 1, 0, 0, 0, 0, 0, -1, 0);
					}
					else if(errtype==13)
					{
						fr = pD->genRandomRange( 0, 0, 0, 0, 0, 0, -1, 0);
					}

					fr->timestamp = timestamp;
					if( fr->transient ) fr->m_pDRAM->n_faults_transient++;
					else fr->m_pDRAM->n_faults_permanent++;
					q1.push( fr );
					//iter_num_errors++;
					err_inserted=1;
				}
			}
		}

		devices++;
	}

	// Step through the event list, injecting a fault into corresponding chip at each event, and invoking ECC
	uint64_t n_undetected = 0;
	uint64_t n_uncorrected = 0;
	uint64_t errors=0;
	uint64_t old_scrubid = 0;
	uint64_t new_scrubid = 0;

	//Run the Repair function: This will check the correctability/ detectability of the fault(s); Repairing is also done instantaneously
	while( !q1.empty() ) {
      //  printf("calling repair\n");
		FaultRange *fr = q1.top();
		DRAMDomain *pDRAM = fr->m_pDRAM;
		pDRAM->m_faultRanges.push_back( fr );

		if( verbose == 2 ) {
			// Dump all FaultRanges before
			cout << "FAULTS INSERTED: BEFORE REPAIR\n";
			// DR DEBUG check all domains, not just the first one
			m_domains.front()->dumpState();
		}

        	errors=0;
		m_domains.front()->repair( n_undetected, n_uncorrected );//Calls repair  function
		if( verbose == 2 ) {
			// Dump all FaultRanges after
			cout << "FAULTS INSERTED: AFTER REPAIR\n";
			m_domains.front()->dumpState();
		}
		q1.pop();
         
       		// printf("ECC Undetected %d Uncorrected %d \n", n_undetected, n_uncorrected); 

		if (!cont_running)
		    {
				if( n_undetected || n_uncorrected ) {
				// if any iteration fails to repair, halt the simulation and report failure
				finalize();
				//Update the appropriate Bin to log into the output file
				bin = fr->timestamp/bin_length;
				fail_time_bins[bin]++;
		    
			if(n_uncorrected>0)
			fail_uncorrectable[bin]++;
			if(n_undetected>0)
			fail_undetectable[bin]++;

				return 1;
				 }
		    }
		else 
		{
		    if(n_undetected ||n_uncorrected)
			{
			errors++;
			bin = fr->timestamp/bin_length;
				fail_time_bins[bin]++;
			if(n_uncorrected>0)
			fail_uncorrectable[bin]++;
			if(n_undetected>0)
			fail_undetectable[bin]++;
			}
		}

		//Scrubbing is performed after the fault has occured and the system isnt failed
		//Timeline analysis of the scrubbing operation
		//-------------*-----|--------*-------*---------------|---------------------/
		//* indicates faults and | indicates the scrub interval
		//If the scrub id (interval id) for scrub between any subsequent faults is the same, we cannot invoke scrubbing again (middle
		// region in the timeline)
		
		new_scrubid = fr->timestamp/m_scrub_interval;	
                if(new_scrubid!=old_scrubid) {
                        for(list<FaultDomain*>::iterator it = m_domains.begin(); it != m_domains.end(); it++ ) {
                                (*it)->scrub();
                                if((*it)->fill_repl()){
                                        finalize();
                                        return 1;
                                }
			}
		}
		old_scrubid = new_scrubid;
	} //End of the loop for all faults
	/***********************************************/
	//   printf("ECC Undetected %d Uncorrected %d \n", n_undetected, n_uncorrected); 

	finalize();
    	if(errors>0)
    	return 1;
    	else
	return 0;
}
