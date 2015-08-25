/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef GROUPDOMAIN_DIMM_HH_
#define GROUPDOMAIN_DIMM_HH_

#include "GroupDomain.hh"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::mt19937_64                     ENG;    // Mersenne Twister
typedef boost::random::uniform_real_distribution<double> DIST;
typedef boost::random::variate_generator<ENG,DIST> GEN;    // Variate generator

class GroupDomain_dimm : public GroupDomain
{
	public:
	GroupDomain_dimm( const char *name, uint64_t chips_t, uint64_t banks_t, uint64_t burst_length );

	void setFIT( int faultClass, bool isTransient, double FIT );
	void init( uint64_t interval, uint64_t max_s, double fit_factor );
	int update( uint test_mode_t );	// perform one iteration
	protected:
	void generateRanges( int faultClass ); // based on a fault, create all faulty address ranges
	
	ENG  eng;
	DIST dist;
	GEN  gen;
};


#endif /* GROUPDOMAIN_DIMM_HH_ */
