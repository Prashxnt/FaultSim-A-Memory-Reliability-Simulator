/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef GROUPDOMAIN_HH_
#define GROUPDOMAIN_HH_

#include "FaultDomain.hh"

class GroupDomain : public FaultDomain
{
	public:
	GroupDomain( const char *name);
	virtual void setFIT(){};
	virtual void init(){};
	virtual void update(){};
};


#endif /* GROUPDOMAIN_HH_ */
