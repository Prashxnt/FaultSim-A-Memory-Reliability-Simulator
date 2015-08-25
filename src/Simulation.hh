/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
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
