/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
