/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
