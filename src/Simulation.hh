/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SIMULATION_HH_
#define SIMULATION_HH_

#include "FaultDomain.hh"

class Simulation {
public:
	Simulation( uint64_t interval_t, uint64_t scrub_interval_t, double fit_factor_t, uint test_mode_t, bool debug_mode_t, bool cont_running_t, uint64_t output_bucket_t );
	void init( uint64_t max_s );
	void reset( void );
	void finalize( void );
	void simulate( uint64_t max_time, uint64_t n_sims, int verbose, std::string output_file);
	virtual uint64_t runOne( uint64_t max_time, int verbose, uint64_t bin_length );
	void addDomain( FaultDomain *domain );
	void getFaultCounts( uint64_t *pTrans, uint64_t *pPerm );
	void resetStats( void );
	void printStats( void );	// output end-of-run stats

protected:
	uint64_t m_interval;
	uint64_t m_iteration;
	uint64_t m_scrub_interval;
	double m_fit_factor;
	uint	test_mode;
	bool debug_mode;
    bool cont_running;
    uint64_t m_output_bucket;


	uint64_t stat_total_failures, stat_total_sims, stat_sim_seconds;
	
    uint64_t *fail_time_bins;
	uint64_t *fail_uncorrectable;
    uint64_t *fail_undetectable;
    
    list<FaultDomain*> m_domains;
};


#endif /* SIMULATION_HH_ */
