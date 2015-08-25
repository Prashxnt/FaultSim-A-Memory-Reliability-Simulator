/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include "GroupDomain_dimm.hh"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>

GroupDomain_dimm::GroupDomain_dimm( const char *name, uint64_t chips_t, uint64_t banks_t, uint64_t burst_size_t) : GroupDomain( name )
, dist(0,1)
, gen(eng,dist)
{
	chips=chips_t; //Total Chips in a DIMM
	banks=banks_t; //Total Banks per Chip
	burst_size = burst_size_t; //The burst length per access, this determines the number of TSVs or number of DATA pins coming out of a Chip in a DIMM

	struct timeval tv;
	gettimeofday (&tv, NULL);
	gen.engine().seed(tv.tv_sec * 1000000 + (tv.tv_usec));
}

int GroupDomain_dimm::update( uint test_mode_t )
{
	return FaultDomain::update(test_mode_t);
}

void GroupDomain_dimm::setFIT( int faultClass, bool isTransient, double FIT )
{
}

void GroupDomain_dimm::init( uint64_t interval, uint64_t max_s, double fit_factor )
{
	FaultDomain::init( interval, max_s, fit_factor );
}

void GroupDomain_dimm::generateRanges( int faultClass )
{

}
