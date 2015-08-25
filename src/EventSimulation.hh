/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef EVENTSIMULATION_HH_
#define EVENTSIMULATION_HH_

#include "Simulation.hh"

class EventSimulation : public Simulation {
public:
	EventSimulation( uint64_t interval_t, uint64_t scrub_interval_t, double fit_factor_t, uint test_mode_t, bool debug_mode_t,
				     bool cont_running_t, uint64_t output_bucket_t );	
	// Simulation loop for a single simulation in Event Driven mode
	virtual uint64_t runOne( uint64_t max_time, int verbose, uint64_t bin_length );
};


#endif /* SIMULATION_HH_ */
